#include "renderer_gles2.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <limits>

namespace mju {
namespace {
constexpr std::size_t kMaxVertices = 16384;
constexpr float kMinZoom = 0.01f;
}

bool GLES2Renderer::check_shader(GLuint shader, const char* stage) {
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_TRUE) return true;

    char log[1024]{};
    GLsizei length = 0;
    glGetShaderInfoLog(shader, sizeof(log), &length, log);
    std::fprintf(stderr, "MJU GLES2 %s shader compile failed: %.*s\n",
                 stage, static_cast<int>(length), log);
    return false;
}

bool GLES2Renderer::check_program(GLuint program) {
    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok == GL_TRUE) return true;

    char log[1024]{};
    GLsizei length = 0;
    glGetProgramInfoLog(program, sizeof(log), &length, log);
    std::fprintf(stderr, "MJU GLES2 program link failed: %.*s\n",
                 static_cast<int>(length), log);
    return false;
}

bool GLES2Renderer::initialize() {
    const char* vertex_source = R"GLSL(
attribute vec2 aPos;
attribute vec2 aUV;
attribute vec4 aColor;
uniform vec2 uViewport;
varying vec2 vUV;
varying vec4 vColor;
void main() {
    vec2 ndc = vec2(aPos.x / uViewport.x * 2.0 - 1.0,
                    1.0 - aPos.y / uViewport.y * 2.0);
    gl_Position = vec4(ndc, 0.0, 1.0);
    vUV = aUV;
    vColor = aColor;
}
)GLSL";

    const char* fragment_source = R"GLSL(
precision mediump float;
uniform sampler2D uTexture;
varying vec2 vUV;
varying vec4 vColor;
void main() {
    gl_FragColor = texture2D(uTexture, vUV) * vColor;
}
)GLSL";

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!vertex_shader || !fragment_shader) return false;

    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);
    if (!check_shader(vertex_shader, "vertex") || !check_shader(fragment_shader, "fragment")) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return false;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glBindAttribLocation(program_, 0, "aPos");
    glBindAttribLocation(program_, 1, "aUV");
    glBindAttribLocation(program_, 2, "aColor");
    glLinkProgram(program_);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    if (!check_program(program_)) {
        shutdown();
        return false;
    }

    pos_ = 0;
    uv_ = 1;
    color_ = 2;
    viewport_ = glGetUniformLocation(program_, "uViewport");
    texture_sampler_ = glGetUniformLocation(program_, "uTexture");

    glGenBuffers(1, &vertex_buffer_);
    glGenBuffers(1, &index_buffer_);
    if (!vertex_buffer_ || !index_buffer_) {
        shutdown();
        return false;
    }

    const std::uint8_t white[4] = {255, 255, 255, 255};
    glGenTextures(1, &white_texture_);
    glBindTexture(GL_TEXTURE_2D, white_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glBindTexture(GL_TEXTURE_2D, 0);

    batch_.reserve(2048);
    upload_indices_.reserve(24576);
    return white_texture_ != 0;
}

void GLES2Renderer::resize(int w, int h) {
    width_ = w > 0 ? w : 1;
    height_ = h > 0 ? h : 1;
    glViewport(0, 0, width_, height_);
}

void GLES2Renderer::begin() {
    ++frame_id_;
    last_quad_count_ = 0;
    last_vertex_count_ = 0;
    last_batch_count_ = 0;
    glClearColor(0.035f, 0.045f, 0.065f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (program_) glUseProgram(program_);
    if (viewport_ >= 0) glUniform2f(viewport_, static_cast<float>(width_), static_cast<float>(height_));
    batch_.begin();
}

void GLES2Renderer::trim_gpu_textures(std::size_t incoming_bytes) {
    while (gpu_bytes_ + incoming_bytes > gpu_budget_bytes_ && !gpu_textures_.empty()) {
        auto victim = gpu_textures_.end();
        std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();
        for (auto it = gpu_textures_.begin(); it != gpu_textures_.end(); ++it) {
            if (it->second.last_used < oldest) {
                oldest = it->second.last_used;
                victim = it;
            }
        }
        if (victim == gpu_textures_.end() || victim->second.last_used == frame_id_) break;
        if (victim->second.id) glDeleteTextures(1, &victim->second.id);
        gpu_bytes_ -= std::min(gpu_bytes_, victim->second.bytes);
        gpu_textures_.erase(victim);
    }
}

GLuint GLES2Renderer::texture_for(const std::string& path) {
    if (path.empty()) return white_texture_;

    auto existing = gpu_textures_.find(path);
    if (existing != gpu_textures_.end()) {
        existing->second.last_used = frame_id_;
        return existing->second.id;
    }

    const auto* image = texture_cache_.load(path);
    if (!image || !image->valid()) {
        if (missing_textures_.insert(path).second) {
            std::fprintf(stderr, "MJU renderer: missing texture '%s'\n", path.c_str());
        }
        return white_texture_;
    }

    const std::size_t bytes = static_cast<std::size_t>(image->width) *
                              static_cast<std::size_t>(image->height) * 4u;
    if (bytes > gpu_budget_bytes_) {
        if (missing_textures_.insert(path).second) {
            std::fprintf(stderr, "MJU renderer: texture '%s' exceeds GPU budget\n", path.c_str());
        }
        return white_texture_;
    }

    trim_gpu_textures(bytes);
    if (gpu_bytes_ + bytes > gpu_budget_bytes_) return white_texture_;

    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (!texture) return white_texture_;
    glBindTexture(GL_TEXTURE_2D, texture);

    const GLenum filter = pixel_art_ ? GL_NEAREST : GL_LINEAR;
    const GLenum min_filter = mipmaps_ ? GL_LINEAR_MIPMAP_LINEAR : filter;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels.data());
    if (mipmaps_) glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    gpu_textures_.emplace(path, GpuTexture{texture, bytes, frame_id_});
    gpu_bytes_ += bytes;
    return texture;
}

void GLES2Renderer::upload_and_draw_batches() {
    const auto& vertices = batch_.vertices();
    const auto& indices = batch_.indices();
    if (vertices.empty() || indices.empty()) return;
    if (vertices.size() > kMaxVertices || indices.size() > 65535) {
        std::fprintf(stderr, "MJU GLES2 batch exceeds mobile index budget\n");
        return;
    }

    upload_indices_.resize(indices.size());
    for (std::size_t i = 0; i < indices.size(); ++i) {
        upload_indices_[i] = static_cast<GLushort>(indices[i]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(render::SpriteVertex)),
                 vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(upload_indices_.size() * sizeof(GLushort)),
                 upload_indices_.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(pos_);
    glEnableVertexAttribArray(uv_);
    glEnableVertexAttribArray(color_);
    glVertexAttribPointer(pos_, 2, GL_FLOAT, GL_FALSE, sizeof(render::SpriteVertex),
                          reinterpret_cast<const void*>(offsetof(render::SpriteVertex, position)));
    glVertexAttribPointer(uv_, 2, GL_FLOAT, GL_FALSE, sizeof(render::SpriteVertex),
                          reinterpret_cast<const void*>(offsetof(render::SpriteVertex, uv)));
    glVertexAttribPointer(color_, 4, GL_FLOAT, GL_FALSE, sizeof(render::SpriteVertex),
                          reinterpret_cast<const void*>(offsetof(render::SpriteVertex, color)));

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(texture_sampler_, 0);
    for (const auto& range : batch_.ranges()) {
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(range.texture_id));
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(range.index_count),
                       GL_UNSIGNED_SHORT,
                       reinterpret_cast<const void*>(range.first_index * sizeof(GLushort)));
    }

    glDisableVertexAttribArray(pos_);
    glDisableVertexAttribArray(uv_);
    glDisableVertexAttribArray(color_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    last_quad_count_ = batch_.stats().sprites;
    last_vertex_count_ = batch_.stats().vertices;
    last_batch_count_ = batch_.stats().batches;
}

void GLES2Renderer::draw(const Scene& scene) {
    draw(scene, Camera2D{});
}

void GLES2Renderer::draw(const Scene& scene, const Camera2D& camera) {
    if (!program_) return;

    const float zoom = std::max(kMinZoom, camera.zoom);
    const float half_width = static_cast<float>(width_) * 0.5f / zoom;
    const float half_height = static_cast<float>(height_) * 0.5f / zoom;
    const float angle = -camera.rotation * 3.14159265358979323846f / 180.0f;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);

    for (const auto& entity : scene.entities()) {
        if (!entity.active || !entity.visible || !entity.sprite.visible) continue;

        const Transform2D world = scene.world_transform(entity.id);
        const float scale_x = std::fabs(world.scale.x);
        const float scale_y = std::fabs(world.scale.y);
        const float half_entity_width = std::max(1.0f, entity.sprite.size.x * scale_x * 0.5f);
        const float half_entity_height = std::max(1.0f, entity.sprite.size.y * scale_y * 0.5f);
        const float dx = std::fabs(world.position.x - camera.position.x);
        const float dy = std::fabs(world.position.y - camera.position.y);
        if (dx > half_width + half_entity_width || dy > half_height + half_entity_height) continue;

        const float relative_x = world.position.x - camera.position.x;
        const float relative_y = world.position.y - camera.position.y;
        const Vec2 screen{
            relative_x * cosine - relative_y * sine + static_cast<float>(width_) * 0.5f,
            relative_x * sine + relative_y * cosine + static_cast<float>(height_) * 0.5f
        };

        Vec2 uv_min = entity.sprite.uv_min;
        Vec2 uv_max = entity.sprite.uv_max;
        const GLuint texture = texture_for(entity.sprite.texture);
        const auto* image = entity.sprite.texture.empty()
            ? nullptr
            : texture_cache_.find(entity.sprite.texture);
        if (image && entity.sprite.frame_width > 0 && entity.sprite.frame_height > 0) {
            const int columns = std::max(1, image->width / entity.sprite.frame_width);
            const int total_frames = std::max(1, entity.sprite.frame_count);
            const int frame = std::clamp(entity.sprite.frame, 0, total_frames - 1);
            const int frame_x = frame % columns;
            const int frame_y = frame / columns;
            uv_min = {
                static_cast<float>(frame_x * entity.sprite.frame_width) / image->width,
                static_cast<float>(frame_y * entity.sprite.frame_height) / image->height
            };
            uv_max = {
                static_cast<float>((frame_x + 1) * entity.sprite.frame_width) / image->width,
                static_cast<float>((frame_y + 1) * entity.sprite.frame_height) / image->height
            };
        }

        batch_.submit({
            screen,
            {entity.sprite.size.x * scale_x * zoom, entity.sprite.size.y * scale_y * zoom},
            uv_min,
            uv_max,
            world.rotation - camera.rotation,
            entity.sprite.color,
            static_cast<std::uint32_t>(texture),
            entity.layer
        });
    }
}

void GLES2Renderer::draw_tilemap(const tilemap::TileMap& map, const Camera2D& camera) {
    if (!program_ || map.width() <= 0 || map.height() <= 0 || map.texture_path().empty()) return;

    const auto* image = texture_cache_.load(map.texture_path());
    if (!image || !image->valid()) return;

    const int tile_width = std::max(1, map.tile_width());
    const int tile_height = std::max(1, map.tile_height());
    const int columns = std::max(1, map.columns());
    const int rows = std::max(1, image->height / tile_height);
    const float zoom = std::max(kMinZoom, camera.zoom);
    const float view_width = static_cast<float>(width_) / zoom;
    const float view_height = static_cast<float>(height_) / zoom;

    const int x0 = std::max(0, static_cast<int>(std::floor((camera.position.x - view_width * 0.5f) / tile_width)) - 1);
    const int y0 = std::max(0, static_cast<int>(std::floor((camera.position.y - view_height * 0.5f) / tile_height)) - 1);
    const int x1 = std::min(map.width() - 1, static_cast<int>(std::ceil((camera.position.x + view_width * 0.5f) / tile_width)) + 1);
    const int y1 = std::min(map.height() - 1, static_cast<int>(std::ceil((camera.position.y + view_height * 0.5f) / tile_height)) + 1);
    if (x1 < x0 || y1 < y0) return;

    const GLuint texture = texture_for(map.texture_path());
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const auto tile = map.get(x, y);
            if (tile.id < 0) continue;
            const int id = tile.id;
            const int tx = id % columns;
            const int ty = id / columns;
            if (ty >= rows) continue;

            float u0 = static_cast<float>(tx * tile_width) / image->width;
            float v0 = static_cast<float>(ty * tile_height) / image->height;
            float u1 = static_cast<float>((tx + 1) * tile_width) / image->width;
            float v1 = static_cast<float>((ty + 1) * tile_height) / image->height;
            if (tile.flip & 1) std::swap(u0, u1);
            if (tile.flip & 2) std::swap(v0, v1);

            Vec2 position{
                (x + 0.5f) * tile_width,
                (y + 0.5f) * tile_height
            };
            position.x = (position.x - camera.position.x) * zoom + width_ * 0.5f;
            position.y = (position.y - camera.position.y) * zoom + height_ * 0.5f;

            batch_.submit({
                position,
                {static_cast<float>(tile_width) * zoom, static_cast<float>(tile_height) * zoom},
                {u0, v0},
                {u1, v1},
                0.0f,
                {1, 1, 1, 1},
                texture,
                0
            });
        }
    }
}

void GLES2Renderer::draw_scene_and_tilemap(const Scene& scene,
                                            const tilemap::TileMap& map,
                                            const Camera2D& camera) {
    begin();
    draw(scene, camera);
    draw_tilemap(map, camera);
    end();
}

void GLES2Renderer::end() {
    batch_.end();
    upload_and_draw_batches();
    glDisable(GL_BLEND);
}

void GLES2Renderer::destroy_textures() {
    for (auto& [key, texture] : gpu_textures_) {
        (void)key;
        if (texture.id) glDeleteTextures(1, &texture.id);
    }
    gpu_textures_.clear();
    gpu_bytes_ = 0;
    missing_textures_.clear();
    texture_cache_.clear();
}

void GLES2Renderer::shutdown() {
    destroy_textures();
    if (white_texture_) {
        glDeleteTextures(1, &white_texture_);
        white_texture_ = 0;
    }
    if (vertex_buffer_) {
        glDeleteBuffers(1, &vertex_buffer_);
        vertex_buffer_ = 0;
    }
    if (index_buffer_) {
        glDeleteBuffers(1, &index_buffer_);
        index_buffer_ = 0;
    }
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}
}
