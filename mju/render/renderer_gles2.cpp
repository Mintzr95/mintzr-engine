#include "renderer_gles2.h"
#include <cmath>
#include <cstdio>
#include <vector>

namespace mju {

namespace {
struct Vertex { float x, y, r, g, b, a; };
constexpr std::size_t kMaxVertices = 16384;
}

bool GLES2Renderer::check_shader(GLuint shader, const char* stage) {
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_TRUE) return true;
    char log[1024]{};
    GLsizei length = 0;
    glGetShaderInfoLog(shader, sizeof(log), &length, log);
    std::fprintf(stderr, "MJU GLES2 %s shader compile failed: %.*s\n", stage, static_cast<int>(length), log);
    return false;
}

bool GLES2Renderer::check_program(GLuint program) {
    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok == GL_TRUE) return true;
    char log[1024]{};
    GLsizei length = 0;
    glGetProgramInfoLog(program, sizeof(log), &length, log);
    std::fprintf(stderr, "MJU GLES2 program link failed: %.*s\n", static_cast<int>(length), log);
    return false;
}

bool GLES2Renderer::initialize() {
    const char* vsrc = R"GLSL(
        attribute vec2 aPos;
        attribute vec4 aColor;
        varying vec4 vColor;
        void main(){ gl_Position = vec4(aPos,0.0,1.0); vColor = aColor; }
    )GLSL";
    const char* fsrc = R"GLSL(
        precision mediump float;
        varying vec4 vColor;
        void main(){ gl_FragColor = vColor; }
    )GLSL";

    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    if (!v || !f) return false;
    glShaderSource(v, 1, &vsrc, nullptr);
    glCompileShader(v);
    glShaderSource(f, 1, &fsrc, nullptr);
    glCompileShader(f);
    if (!check_shader(v, "vertex") || !check_shader(f, "fragment")) {
        glDeleteShader(v);
        glDeleteShader(f);
        return false;
    }

    program_ = glCreateProgram();
    glAttachShader(program_, v);
    glAttachShader(program_, f);
    glBindAttribLocation(program_, 0, "aPos");
    glBindAttribLocation(program_, 1, "aColor");
    glLinkProgram(program_);
    glDeleteShader(v);
    glDeleteShader(f);
    if (!check_program(program_)) {
        shutdown();
        return false;
    }

    pos_ = 0;
    color_ = 1;
    glGenBuffers(1, &vertex_buffer_);
    glGenBuffers(1, &index_buffer_);
    if (!vertex_buffer_ || !index_buffer_) {
        shutdown();
        return false;
    }
    return true;
}

void GLES2Renderer::resize(int w, int h) {
    width_ = w > 0 ? w : 1;
    height_ = h > 0 ? h : 1;
    glViewport(0, 0, width_, height_);
}

void GLES2Renderer::begin() {
    last_quad_count_ = 0;
    last_vertex_count_ = 0;
    last_batch_count_ = 0;
    glClearColor(0.035f, 0.045f, 0.065f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (program_) glUseProgram(program_);
}

void GLES2Renderer::draw(const Scene& scene) {
    if (!program_) return;

    batch_.begin();
    for (const auto& e : scene.entities()) {
        if (!e.active || !e.visible || !e.sprite.visible) continue;
        batch_.submit({
            e.transform.position,
            {e.sprite.size.x * e.transform.scale.x, e.sprite.size.y * e.transform.scale.y},
            {0.0f, 0.0f}, {1.0f, 1.0f},
            e.transform.rotation,
            e.sprite.color,
            0,
            e.layer
        });
    }
    batch_.end();

    const auto& src = batch_.vertices();
    const auto& indices = batch_.indices();
    if (src.empty() || indices.empty()) return;

    if (src.size() > kMaxVertices || indices.size() > 65535) {
        std::fprintf(stderr, "MJU GLES2 batch exceeds 16-bit index budget; reduce sprite batch capacity\n");
        return;
    }

    std::vector<Vertex> vertices;
    vertices.resize(src.size());
    for (std::size_t i = 0; i < src.size(); ++i) {
        const auto& v = src[i];
        vertices[i] = {
            v.position.x / static_cast<float>(width_) * 2.0f - 1.0f,
            1.0f - v.position.y / static_cast<float>(height_) * 2.0f,
            v.color.r, v.color.g, v.color.b, v.color.a
        };
    }

    std::vector<GLushort> index16(indices.size());
    for (std::size_t i = 0; i < indices.size(); ++i) {
        index16[i] = static_cast<GLushort>(indices[i]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(index16.size() * sizeof(GLushort)),
                 index16.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(static_cast<GLuint>(pos_));
    glEnableVertexAttribArray(static_cast<GLuint>(color_));
    glVertexAttribPointer(static_cast<GLuint>(pos_), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, x)));
    glVertexAttribPointer(static_cast<GLuint>(color_), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, r)));
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index16.size()), GL_UNSIGNED_SHORT, nullptr);
    glDisableVertexAttribArray(static_cast<GLuint>(pos_));
    glDisableVertexAttribArray(static_cast<GLuint>(color_));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    last_quad_count_ = batch_.stats().sprites;
    last_vertex_count_ = batch_.stats().vertices;
    last_batch_count_ = batch_.stats().batches;
}

void GLES2Renderer::end() {}

void GLES2Renderer::shutdown() {
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
