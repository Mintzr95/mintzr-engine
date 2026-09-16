#include <android/log.h>
#include "../../mju/core/application.h"
#include "../../mju/render/renderer_gles2.h"
static mju::Application g_app; static mju::GLES2Renderer g_renderer;
extern "C" __attribute__((visibility("default"))) void mju_android_start(int w,int h){
 if(!g_app.start(w,h)){__android_log_print(ANDROID_LOG_ERROR,"MJU","Engine init failed");return;}
 if(!g_renderer.initialize()){__android_log_print(ANDROID_LOG_ERROR,"MJU","Renderer init failed");return;}
 g_renderer.resize(w,h);
}
extern "C" __attribute__((visibility("default"))) void mju_android_resize(int w,int h){g_app.resize(w,h);g_renderer.resize(w,h);}
extern "C" __attribute__((visibility("default"))) void mju_android_tick(float dt){
 g_app.tick(dt);
 g_renderer.begin();
 g_renderer.draw(g_app.engine().scene(),g_app.engine().camera());
 g_renderer.end();
}
extern "C" __attribute__((visibility("default"))) void mju_android_touch(int action,int id,float x,float y){
 auto&i=g_app.engine().input(); if(action==0)i.pointer_down(id,x,y); else if(action==1)i.pointer_move(id,x,y); else if(action==2)i.pointer_up(id);
}
extern "C" __attribute__((visibility("default"))) void mju_android_stop(){g_app.stop();g_renderer.shutdown();}
