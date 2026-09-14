#include <jni.h>
extern "C" void mju_android_start(int,int); extern "C" void mju_android_resize(int,int); extern "C" void mju_android_tick(float); extern "C" void mju_android_touch(int,int,float,float); extern "C" void mju_android_stop();
extern "C" JNIEXPORT void JNICALL Java_com_mju_engine_MainActivity_nativeStart(JNIEnv*,jclass,jint w,jint h){mju_android_start(w,h);} 
extern "C" JNIEXPORT void JNICALL Java_com_mju_engine_MainActivity_nativeResize(JNIEnv*,jclass,jint w,jint h){mju_android_resize(w,h);} 
extern "C" JNIEXPORT void JNICALL Java_com_mju_engine_MainActivity_nativeTick(JNIEnv*,jclass,jfloat dt){mju_android_tick(dt);} 
extern "C" JNIEXPORT void JNICALL Java_com_mju_engine_MainActivity_nativeTouch(JNIEnv*,jclass,jint action,jint id,jfloat x,jfloat y){mju_android_touch(action,id,x,y);} 
extern "C" JNIEXPORT void JNICALL Java_com_mju_engine_MainActivity_nativeStop(JNIEnv*,jclass){mju_android_stop();}
