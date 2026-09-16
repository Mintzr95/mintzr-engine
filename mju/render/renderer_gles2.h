#pragma once
#include <GLES2/gl2.h>
#include <cstddef>
#include "../core/scene.h"
#include "../core/camera.h"
#include "sprite_batch.h"

namespace mju {

class GLES2Renderer {
public:
    bool initialize();
    void resize(int w, int h);
    void begin();
    void draw(const Scene& scene);
    void draw(const Scene& scene, const Camera2D& camera);
    void end();
    void shutdown();

    std::size_t last_quad_count() const { return last_quad_count_; }
    std::size_t last_vertex_count() const { return last_vertex_count_; }
    std::size_t last_batch_count() const { return last_batch_count_; }

private:
    bool check_shader(GLuint shader, const char* stage);
    bool check_program(GLuint program);
    GLuint program_ = 0;
    GLint pos_ = -1;
    GLint color_ = -1;
    GLuint vertex_buffer_ = 0;
    GLuint index_buffer_ = 0;
    int width_ = 1;
    int height_ = 1;
    std::size_t last_quad_count_ = 0;
    std::size_t last_vertex_count_ = 0;
    std::size_t last_batch_count_ = 0;
    render::SpriteBatch batch_{2048};
};

}
