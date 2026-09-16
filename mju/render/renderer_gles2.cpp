#include "renderer_gles2.h"
#include <array>
#include <cmath>
#include <cstdio>

namespace mju {

namespace {
struct Vertex { float x, y, r, g, b, a; };
constexpr std::size_t kMaxQuads = 4096;
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
    glClearColor(0.035f, 0.045f, 0.065f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (program_) glUseProgram(program_);
}

void GLES2Renderer::draw(const Scene& scene) {
    if (!program_) return;

    std::array<Vertex, kMaxQuads * 6> vertices{};
    std::size_t vertex_count = 0;

    for (const auto& e : scene.entities()) {
        if (!e.active || !e.visible || !e.sprite.visible) continue;
        if (vertex_count + 6 > vertices.size()) break;

        const float sx = e.transform.scale.x;
        const float sy = e.transform.scale.y;
        const float hw = e.sprite.size.x * sx * 0.5f / static_cast<float>(width_) * 2.0f;
        const float hh = e.sprite.size.y * sy * 0.5f / static_cast<float>(height_) * 2.0f;
        const float cx = e.transform.position.x / static_cast<float>(width_) * 2.0f - 1.0f;
        const float cy = 1.0f - e.transform.position.y / static_cast<float>(height_) * 2.0f;
        const float a = e.transform.rotation * 3.14159265358979323846f / 180.0f;
        const float c = std::cos(a), s = std::sin(a);
        const float x[4] = {-hw, hw, hw, -hw};
        const float y[4] = {-hh, -hh, hh, hh};
        const int order[6] = {0, 1, 2, 0, 2, 3};

        for (int i = 0; i < 6; ++i) {
            const int k = order[i];
            const float rx = x[k] * c - y[k] * s;
            const float ry = x[k] * s + y[k] * c;
            vertices[vertex_count++] = {cx + rx, cy + ry,
                e.sprite.color.r, e.sprite.color.g, e.sprite.color.b, e.sprite.color.a};
        }
        ++last_quad_count_;
    }

    if (vertex_count == 0) return;

    glEnableVertexAttribArray(static_cast<GLuint>(pos_));
    glEnableVertexAttribArray(static_cast<GLuint>(color_));
    glVertexAttribPointer(static_cast<GLuint>(pos_), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertices[0].x);
    glVertexAttribPointer(static_cast<GLuint>(color_), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertices[0].r);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count));
    glDisableVertexAttribArray(static_cast<GLuint>(pos_));
    glDisableVertexAttribArray(static_cast<GLuint>(color_));
    last_vertex_count_ = vertex_count;
}

void GLES2Renderer::end() {}

void GLES2Renderer::shutdown() {
    if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

}
