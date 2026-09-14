#include "renderer_gles2.h"
#include <cmath>
namespace mju {
static GLuint compile(GLenum t,const char*s){GLuint sh=glCreateShader(t);glShaderSource(sh,1,&s,nullptr);glCompileShader(sh);return sh;}
bool GLES2Renderer::initialize(){const char* vsrc="attribute vec2 aPos; void main(){gl_Position=vec4(aPos,0.0,1.0);}";
const char* fsrc="precision mediump float; uniform vec4 uColor; void main(){gl_FragColor=uColor;}";
GLuint v=compile(GL_VERTEX_SHADER,vsrc),f=compile(GL_FRAGMENT_SHADER,fsrc);program_=glCreateProgram();glAttachShader(program_,v);glAttachShader(program_,f);glBindAttribLocation(program_,0,"aPos");glLinkProgram(program_);glDeleteShader(v);glDeleteShader(f);pos_=0;color_=glGetUniformLocation(program_,"uColor");return program_!=0;}
void GLES2Renderer::resize(int w,int h){width_=w>0?w:1;height_=h>0?h:1;glViewport(0,0,width_,height_);}
void GLES2Renderer::begin(){glClearColor(0.035f,0.045f,0.065f,1);glClear(GL_COLOR_BUFFER_BIT);glUseProgram(program_);}
void GLES2Renderer::draw(const Scene& s){for(const auto&e:s.entities()){if(!e.active||!e.sprite.visible)continue;float cx=e.transform.position.x/width_*2-1;float cy=1-e.transform.position.y/height_*2;float hw=e.sprite.size.x*0.5f/width_*2;float hh=e.sprite.size.y*0.5f/height_*2;float a=e.transform.rotation*3.14159265f/180.0f;float c=std::cos(a),sn=std::sin(a);float pts[8]={-hw,-hh,hw,-hh,hw,hh,-hw,hh};for(int i=0;i<4;i++){float x=pts[i*2],y=pts[i*2+1];pts[i*2]=cx+x*c-y*sn;pts[i*2+1]=cy+x*sn+y*c;}glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,pts);glEnableVertexAttribArray(0);glUniform4f(color_,e.sprite.color.r,e.sprite.color.g,e.sprite.color.b,e.sprite.color.a);glDrawArrays(GL_TRIANGLE_FAN,0,4);}}
void GLES2Renderer::end(){glDisableVertexAttribArray(0);}
void GLES2Renderer::shutdown(){if(program_){glDeleteProgram(program_);program_=0;}}
}
