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

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        view = new MjuView(this);
        setContentView(view);
    }

    @Override protected void onPause() {
        if (view != null) view.onPause();
        super.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        if (view != null) view.onResume();
    }

    @Override protected void onDestroy() {
        if (view != null) view.shutdown();
        super.onDestroy();
    }

    public static class MjuView extends GLSurfaceView {
        private final Renderer renderer;

        public MjuView(Context context) {
            super(context);
            setEGLContextClientVersion(2);
            setPreserveEGLContextOnPause(true);
            renderer = new Renderer();
            setRenderer(renderer);
            setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        }

        public MjuView(Context context, AttributeSet attrs) {
            this(context);
        }

        @Override public boolean onTouchEvent(MotionEvent event) {
            final int action = event.getActionMasked();
            final int actionIndex = event.getActionIndex();
            final int pointerId = event.getPointerId(actionIndex);
            final int nativeAction =
                (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) ? 0 :
                (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP ||
                 action == MotionEvent.ACTION_CANCEL) ? 2 : 1;
            nativeTouch(nativeAction, pointerId,
                        event.getX(actionIndex), event.getY(actionIndex));
            return true;
        }

        void shutdown() {
            queueEvent(MainActivity::nativeStop);
        }

        private class Renderer implements GLSurfaceView.Renderer {
            private long lastNanos;

            @Override public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                lastNanos = System.nanoTime();
                nativeStart(getWidth(), getHeight());
            }

            @Override public void onSurfaceChanged(GL10 gl, int width, int height) {
                nativeResize(width, height);
            }

            @Override public void onDrawFrame(GL10 gl) {
                final long now = System.nanoTime();
                float dt = (now - lastNanos) / 1_000_000_000.0f;
                lastNanos = now;
                if (dt <= 0.0f) dt = 1.0f / 60.0f;
                if (dt > 0.1f) dt = 0.1f;
                nativeTick(dt);
            }
        }
    }

    public static native void nativeStart(int width, int height);
    public static native void nativeResize(int width, int height);
    public static native void nativeTick(float dt);
    public static native void nativeTouch(int action, int id, float x, float y);
    public static native void nativeStop();

    static {
        System.loadLibrary("mju_android");
    }
}
