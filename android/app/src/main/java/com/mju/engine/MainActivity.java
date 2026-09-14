package com.mju.engine;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.opengl.GLSurfaceView;
import android.content.Context;
import android.util.AttributeSet;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {
 private MjuView view;
 @Override public void onCreate(Bundle state){super.onCreate(state);view=new MjuView(this);setContentView(view);}
 @Override protected void onPause(){super.onPause();view.onPause();}
 @Override protected void onResume(){super.onResume();view.onResume();}
 @Override protected void onDestroy(){view.shutdown();super.onDestroy();}
 public static class MjuView extends GLSurfaceView {
  private final Renderer renderer;
  public MjuView(Context c){super(c);setEGLContextClientVersion(2);renderer=new Renderer();setRenderer(renderer);setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);}
  public MjuView(Context c,AttributeSet a){this(c);}
  @Override public boolean onTouchEvent(MotionEvent e){int action=e.getActionMasked();int id=e.getPointerId(e.getActionIndex());int a=(action==MotionEvent.ACTION_DOWN||action==MotionEvent.ACTION_POINTER_DOWN)?0:(action==MotionEvent.ACTION_UP||action==MotionEvent.ACTION_POINTER_UP||action==MotionEvent.ACTION_CANCEL)?2:1;nativeTouch(a,id,e.getX(e.getActionIndex()),e.getY(e.getActionIndex()));return true;}
  void shutdown(){queueEvent(MainActivity::nativeStop);}
  private class Renderer implements GLSurfaceView.Renderer{
   long last=0;
   public void onSurfaceCreated(GL10 gl,EGLConfig c){last=System.nanoTime();nativeStart(getWidth(),getHeight());}
   public void onSurfaceChanged(GL10 gl,int w,int h){nativeResize(w,h);}
   public void onDrawFrame(GL10 gl){long now=System.nanoTime();float dt=(now-last)/1_000_000_000f;last=now;if(dt>0.1f)dt=0.1f;nativeTick(dt);}
  }
 }
 public static native void nativeStart(int w,int h);public static native void nativeResize(int w,int h);public static native void nativeTick(float dt);public static native void nativeTouch(int action,int id,float x,float y);public static native void nativeStop();
 static {System.loadLibrary("mju_android");}
}
