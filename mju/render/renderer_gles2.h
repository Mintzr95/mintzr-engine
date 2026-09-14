#pragma once
#include <GLES2/gl2.h>
#include "../core/scene.h"
namespace mju { class GLES2Renderer { public: bool initialize(); void resize(int w,int h); void begin(); void draw(const Scene& scene); void end(); void shutdown(); private: GLuint program_=0; GLint pos_= -1, color_=-1; int width_=1,height_=1; }; }
