#include "renderer_gles2.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <vector>
namespace mju {
namespace { struct Vertex{float x,y,u,v,r,g,b,a;}; constexpr std::size_t kMaxVertices=16384; constexpr float kMinZoom=0.01f; }
bool GLES2Renderer::check_shader(GLuint shader,const char*stage){GLint ok=GL_FALSE;glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);if(ok==GL_TRUE)return true;char log[1024]{};GLsizei len=0;glGetShaderInfoLog(shader,sizeof(log),&len,log);std::fprintf(stderr,"MJU GLES2 %s shader compile failed: %.*s\n",stage,(int)len,log);return false;}
bool GLES2Renderer::check_program(GLuint program){GLint ok=GL_FALSE;glGetProgramiv(program,GL_LINK_STATUS,&ok);if(ok==GL_TRUE)return true;char log[1024]{};GLsizei len=0;glGetProgramInfoLog(program,sizeof(log),&len,log);std::fprintf(stderr,"MJU GLES2 program link failed: %.*s\n",(int)len,log);return false;}
bool GLES2Renderer::initialize(){
 const char*vsrc=R"GLSL(attribute vec2 aPos;attribute vec2 aUV;attribute vec4 aColor;varying vec2 vUV;varying vec4 vColor;void main(){gl_Position=vec4(aPos,0.0,1.0);vUV=aUV;vColor=aColor;})GLSL";
 const char*fsrc=R"GLSL(precision mediump float;uniform sampler2D uTexture;varying vec2 vUV;varying vec4 vColor;void main(){gl_FragColor=texture2D(uTexture,vUV)*vColor;})GLSL";
 GLuint v=glCreateShader(GL_VERTEX_SHADER),f=glCreateShader(GL_FRAGMENT_SHADER);if(!v||!f)return false;glShaderSource(v,1,&vsrc,nullptr);glCompileShader(v);glShaderSource(f,1,&fsrc,nullptr);glCompileShader(f);if(!check_shader(v,"vertex")||!check_shader(f,"fragment")){glDeleteShader(v);glDeleteShader(f);return false;}
 program_=glCreateProgram();glAttachShader(program_,v);glAttachShader(program_,f);glBindAttribLocation(program_,0,"aPos");glBindAttribLocation(program_,1,"aUV");glBindAttribLocation(program_,2,"aColor");glLinkProgram(program_);glDeleteShader(v);glDeleteShader(f);if(!check_program(program_)){shutdown();return false;}
 pos_=0;uv_=1;color_=2;texture_sampler_=glGetUniformLocation(program_,"uTexture");glGenBuffers(1,&vertex_buffer_);glGenBuffers(1,&index_buffer_);if(!vertex_buffer_||!index_buffer_){shutdown();return false;}
 const std::uint8_t white[4]={255,255,255,255};glGenTextures(1,&white_texture_);glBindTexture(GL_TEXTURE_2D,white_texture_);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,white);glBindTexture(GL_TEXTURE_2D,0);return white_texture_!=0;
}
void GLES2Renderer::resize(int w,int h){width_=w>0?w:1;height_=h>0?h:1;glViewport(0,0,width_,height_);}
void GLES2Renderer::begin(){last_quad_count_=last_vertex_count_=last_batch_count_=0;glClearColor(0.035f,0.045f,0.065f,1.0f);glClear(GL_COLOR_BUFFER_BIT);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);if(program_)glUseProgram(program_);}
GLuint GLES2Renderer::texture_for(const std::string&path){
 if(path.empty())return white_texture_;auto it=gpu_textures_.find(path);if(it!=gpu_textures_.end())return it->second;const auto*img=texture_cache_.load(path);if(!img||!img->valid())return white_texture_;
 GLuint tex=0;glGenTextures(1,&tex);if(!tex)return white_texture_;glBindTexture(GL_TEXTURE_2D,tex);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img->width,img->height,0,GL_RGBA,GL_UNSIGNED_BYTE,img->pixels.data());glBindTexture(GL_TEXTURE_2D,0);gpu_textures_.emplace(path,tex);return tex;
}
void GLES2Renderer::draw(const Scene&scene){draw(scene,Camera2D{});}
void GLES2Renderer::draw(const Scene&scene,const Camera2D&camera){
 if(!program_)return;const float zoom=std::max(kMinZoom,camera.zoom),hw=(float)width_*0.5f/zoom,hh=(float)height_*0.5f/zoom;const float ang=-camera.rotation*3.14159265358979323846f/180.0f,cc=std::cos(ang),ss=std::sin(ang);batch_.begin();
 for(auto&e:scene.entities()){
  if(!e.active||!e.visible||!e.sprite.visible)continue;const float sx=std::fabs(e.transform.scale.x),sy=std::fabs(e.transform.scale.y);const float wh=std::max(1.0f,e.sprite.size.x*sx*0.5f),wv=std::max(1.0f,e.sprite.size.y*sy*0.5f);float dx=std::fabs(e.transform.position.x-camera.position.x),dy=std::fabs(e.transform.position.y-camera.position.y);if(dx>hw+wh||dy>hh+wv)continue;
  float rx=e.transform.position.x-camera.position.x,ry=e.transform.position.y-camera.position.y;Vec2 screen{rx*cc-ry*ss+(float)width_*0.5f,rx*ss+ry*cc+(float)height_*0.5f};
  Vec2 uv0=e.sprite.uv_min,uv1=e.sprite.uv_max;GLuint tex=texture_for(e.sprite.texture);const auto*img=e.sprite.texture.empty()?nullptr:texture_cache_.find(e.sprite.texture);if(img&&e.sprite.frame_width>0&&e.sprite.frame_height>0){int cols=std::max(1,img->width/e.sprite.frame_width),total=std::max(1,e.sprite.frame_count);int frame=std::clamp(e.sprite.frame,0,total-1);int fx=frame%cols,fy=frame/cols;uv0={(float)fx*e.sprite.frame_width/img->width,(float)fy*e.sprite.frame_height/img->height};uv1={(float)(fx+1)*e.sprite.frame_width/img->width,(float)(fy+1)*e.sprite.frame_height/img->height};}
  batch_.submit({screen,{e.sprite.size.x*sx*zoom,e.sprite.size.y*sy*zoom},uv0,uv1,e.transform.rotation-camera.rotation,e.sprite.color,(std::uint32_t)tex,e.layer});
 }
 batch_.end();const auto&src=batch_.vertices();const auto&idx=batch_.indices();if(src.empty()||idx.empty())return;if(src.size()>kMaxVertices||idx.size()>65535){std::fprintf(stderr,"MJU GLES2 batch exceeds mobile index budget\n");return;}
 std::vector<Vertex>verts(src.size());for(std::size_t i=0;i<src.size();++i){const auto&v=src[i];verts[i]={v.position.x/(float)width_*2.0f-1.0f,1.0f-v.position.y/(float)height_*2.0f,v.uv.x,v.uv.y,v.color.r,v.color.g,v.color.b,v.color.a};}
 std::vector<GLushort>idx16(idx.size());for(std::size_t i=0;i<idx.size();++i)idx16[i]=(GLushort)idx[i];glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_);glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)(verts.size()*sizeof(Vertex)),verts.data(),GL_DYNAMIC_DRAW);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer_);glBufferData(GL_ELEMENT_ARRAY_BUFFER,(GLsizeiptr)(idx16.size()*sizeof(GLushort)),idx16.data(),GL_DYNAMIC_DRAW);
 glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_);glEnableVertexAttribArray(0);glEnableVertexAttribArray(1);glEnableVertexAttribArray(2);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,x));glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,u));glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,r));glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_buffer_);
 glActiveTexture(GL_TEXTURE0);glUniform1i(texture_sampler_,0);for(const auto&r:batch_.ranges()){glBindTexture(GL_TEXTURE_2D,(GLuint)r.texture_id);glDrawElements(GL_TRIANGLES,(GLsizei)r.index_count,GL_UNSIGNED_SHORT,(const void*)(r.first_index*sizeof(GLushort)));}
 glDisableVertexAttribArray(0);glDisableVertexAttribArray(1);glDisableVertexAttribArray(2);glBindBuffer(GL_ARRAY_BUFFER,0);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);glBindTexture(GL_TEXTURE_2D,0);last_quad_count_=batch_.stats().sprites;last_vertex_count_=batch_.stats().vertices;last_batch_count_=batch_.stats().batches;
}
void GLES2Renderer::end(){glDisable(GL_BLEND);}
void GLES2Renderer::destroy_textures(){for(auto&[k,t]:gpu_textures_)if(t)glDeleteTextures(1,&t);gpu_textures_.clear();texture_cache_.clear();}
void GLES2Renderer::shutdown(){destroy_textures();if(white_texture_){glDeleteTextures(1,&white_texture_);white_texture_=0;}if(vertex_buffer_){glDeleteBuffers(1,&vertex_buffer_);vertex_buffer_=0;}if(index_buffer_){glDeleteBuffers(1,&index_buffer_);index_buffer_=0;}if(program_){glDeleteProgram(program_);program_=0;}}
}
