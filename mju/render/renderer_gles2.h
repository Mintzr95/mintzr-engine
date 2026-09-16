#pragma once
#include <GLES2/gl2.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../core/scene.h"
#include "../core/camera.h"
#include "../assets/texture_cache.h"
#include "../tilemap/tilemap.h"
#include "sprite_batch.h"

namespace mju {

class GLES2Renderer {
public:
    bool initialize();
    void resize(int w, int h);
    void begin();
    void draw(const Scene& scene);
    void draw(const Scene& scene, const Camera2D& camera);
    void draw_tilemap(const tilemap::TileMap& map, const Camera2D& camera);
    void end();
    void shutdown();

    void set_pixel_art(bool enabled) { pixel_art_ = enabled; }
    void set_mipmaps(bool enabled) { mipmaps_ = enabled; }
    void set_gpu_texture_budget_bytes(std::size_t bytes) {
        gpu_budget_bytes_ = std::max<std::size_t>(1, bytes);
        trim_gpu_textures();
    }
    bool pixel_art() const { return pixel_art_; }
    bool mipmaps() const { return mipmaps_; }
    std::size_t last_quad_count() const { return last_quad_count_; }
    std::size_t last_vertex_count() const { return last_vertex_count_; }
    std::size_t last_batch_count() const { return last_batch_count_; }
    std::size_t loaded_texture_count() const { return gpu_textures_.size(); }
    std::size_t gpu_texture_bytes() const { return gpu_bytes_; }
    std::size_t gpu_texture_budget_bytes() const { return gpu_budget_bytes_; }

private:
    struct GpuTexture {
        GLuint id = 0;
        std::size_t bytes = 0;
        std::uint64_t last_used = 0;
    };

    bool check_shader(GLuint shader, const char* stage);
    bool check_program(GLuint program);
    GLuint texture_for(const std::string& path);
    void trim_gpu_textures(std::size_t incoming_bytes = 0);
    void destroy_textures();
    void upload_and_draw_batches();

    GLuint program_ = 0;
    GLint pos_ = -1;
    GLint color_ = -1;
    GLint uv_ = -1;
    GLint viewport_ = -1;
    GLint texture_sampler_ = -1;
    GLuint vertex_buffer_ = 0;
    GLuint index_buffer_ = 0;
    GLuint white_texture_ = 0;
    int width_ = 1;
    int height_ = 1;
    std::size_t last_quad_count_ = 0;
    std::size_t last_vertex_count_ = 0;
    std::size_t last_batch_count_ = 0;
    std::uint64_t frame_id_ = 0;
    std::size_t gpu_bytes_ = 0;
    std::size_t gpu_budget_bytes_ = 64u * 1024u * 1024u;
    bool pixel_art_ = false;
    bool mipmaps_ = false;
    render::SpriteBatch batch_{2048};
    assets::TextureCache texture_cache_;
    std::unordered_map<std::string, GpuTexture> gpu_textures_;
    std::unordered_set<std::string> missing_textures_;
    std::vector<GLushort> upload_indices_;
};

}
