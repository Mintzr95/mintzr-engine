#include <android/log.h>
#include <jni.h>
#include <sstream>
#include <string>

#include "../../mju/core/application.h"
#include "../../mju/editor/editor.h"
#include "../../mju/render/renderer_gles2.h"

namespace {
mju::Application g_app;
mju::GLES2Renderer g_renderer;
mju::editor::EditorState g_editor;
}

extern "C" void mju_android_start(int w, int h) {
    if (!g_app.start(w, h)) {
        __android_log_print(ANDROID_LOG_ERROR, "MJU", "Engine init failed");
        return;
    }
    if (!g_renderer.initialize()) {
        __android_log_print(ANDROID_LOG_ERROR, "MJU", "Renderer init failed");
        return;
    }
    g_renderer.resize(w, h);
}

extern "C" void mju_android_resize(int w, int h) {
    g_app.resize(w, h);
    g_renderer.resize(w, h);
}

extern "C" void mju_android_tick(float dt) {
    g_app.tick(dt);
    g_renderer.draw_scene_and_tilemap(
        g_app.engine().scene(),
        g_app.engine().tilemap(),
        g_app.engine().camera());
}

extern "C" void mju_android_touch(int action, int id, float x, float y) {
    auto& input = g_app.engine().input();
    if (action == 0) input.pointer_down(id, x, y);
    else if (action == 1) input.pointer_move(id, x, y);
    else if (action == 2) input.pointer_up(id);
}

extern "C" void mju_android_stop() {
    g_app.stop();
    g_renderer.shutdown();
}

static jstring to_jstring(JNIEnv* env, const std::string& value) {
    return env->NewStringUTF(value.c_str());
}

static void ensure_editor_seed() {
    if (g_editor.scene.size() == 0) {
        auto* root = g_editor.create("Node");
        if (root) root->transform.position = {160, 160};
        auto* child = g_editor.create("Child", root ? root->id : 0);
        if (child) child->transform.position = {64, 64};
    }
}

static std::string editor_snapshot() {
    ensure_editor_seed();
    std::ostringstream out;
    for (const auto& entity : g_editor.scene.entities()) {
        out << entity.id << '|'
            << entity.name << '|'
            << entity.transform.position.x << '|'
            << entity.transform.position.y << '|'
            << (entity.visible ? 1 : 0) << '|'
            << (entity.locked ? 1 : 0) << '|'
            << entity.layer << '\n';
    }
    return out.str();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_MainActivity_nativeStart(JNIEnv*, jclass, jint w, jint h) {
    mju_android_start(w, h);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_MainActivity_nativeResize(JNIEnv*, jclass, jint w, jint h) {
    mju_android_resize(w, h);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_MainActivity_nativeTick(JNIEnv*, jclass, jfloat dt) {
    mju_android_tick(dt);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_MainActivity_nativeTouch(JNIEnv*, jclass, jint action, jint id, jfloat x, jfloat y) {
    mju_android_touch(action, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_MainActivity_nativeStop(JNIEnv*, jclass) {
    mju_android_stop();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorStart(JNIEnv*, jclass) {
    g_editor.history.reset(g_editor.scene);
    ensure_editor_seed();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorSnapshot(JNIEnv* env, jclass) {
    return to_jstring(env, editor_snapshot());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorCreate(JNIEnv*, jclass) {
    ensure_editor_seed();
    auto* entity = g_editor.create("Node");
    return entity ? static_cast<jint>(entity->id) : 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorDuplicate(JNIEnv*, jclass, jint id) {
    g_editor.select(static_cast<mju::EntityId>(id));
    auto* entity = g_editor.duplicate_selected();
    return entity ? static_cast<jint>(entity->id) : 0;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorDelete(JNIEnv*, jclass, jint id) {
    if (!g_editor.select(static_cast<mju::EntityId>(id))) return JNI_FALSE;
    return g_editor.delete_selected() ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorUndo(JNIEnv*, jclass) {
    return g_editor.undo() ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorRedo(JNIEnv*, jclass) {
    return g_editor.redo() ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorBeginTransform(JNIEnv*, jclass, jint id) {
    if (!g_editor.select(static_cast<mju::EntityId>(id))) return JNI_FALSE;
    g_editor.begin_transform();
    return g_editor.transform_editing() ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorEndTransform(JNIEnv*, jclass) {
    const bool was_editing = g_editor.transform_editing();
    g_editor.end_transform();
    return was_editing ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorSetTransform(JNIEnv*, jclass, jint id, jfloat x, jfloat y) {
    auto* entity = g_editor.scene.find(static_cast<mju::EntityId>(id));
    if (!entity || entity->locked) return JNI_FALSE;
    if (g_editor.selected != static_cast<mju::EntityId>(id)) g_editor.select(static_cast<mju::EntityId>(id));
    if (!g_editor.transform_editing()) g_editor.begin_transform();
    entity->transform.position = {x, y};
    return JNI_TRUE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorSetName(JNIEnv* env, jclass, jint id, jstring name) {
    auto* entity = g_editor.scene.find(static_cast<mju::EntityId>(id));
    if (!entity || entity->locked || !name) return JNI_FALSE;
    if (g_editor.transform_editing()) g_editor.end_transform();
    const char* chars = env->GetStringUTFChars(name, nullptr);
    if (!chars) return JNI_FALSE;
    g_editor.checkpoint();
    entity->name = chars;
    env->ReleaseStringUTFChars(name, chars);
    g_editor.history.commit(g_editor.scene);
    return JNI_TRUE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorSetVisibility(JNIEnv*, jclass, jint id, jboolean visible) {
    auto* entity = g_editor.scene.find(static_cast<mju::EntityId>(id));
    if (!entity || entity->locked) return JNI_FALSE;
    if (g_editor.transform_editing()) g_editor.end_transform();
    g_editor.checkpoint();
    entity->visible = visible == JNI_TRUE;
    entity->sprite.visible = entity->visible;
    g_editor.history.commit(g_editor.scene);
    return JNI_TRUE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorSetLocked(JNIEnv*, jclass, jint id, jboolean locked) {
    auto* entity = g_editor.scene.find(static_cast<mju::EntityId>(id));
    if (!entity) return JNI_FALSE;
    if (g_editor.transform_editing()) g_editor.end_transform();
    g_editor.checkpoint();
    entity->locked = locked == JNI_TRUE;
    g_editor.history.commit(g_editor.scene);
    return JNI_TRUE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorSave(JNIEnv* env, jclass, jstring path) {
    if (!path) return JNI_FALSE;
    if (g_editor.transform_editing()) g_editor.end_transform();
    const char* chars = env->GetStringUTFChars(path, nullptr);
    if (!chars) return JNI_FALSE;
    const bool ok = g_editor.save(chars);
    env->ReleaseStringUTFChars(path, chars);
    return ok ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_mju_engine_EditorActivity_nativeEditorClear(JNIEnv*, jclass) {
    if (g_editor.transform_editing()) g_editor.end_transform();
    g_editor.scene.clear();
    g_editor.selected = 0;
    g_editor.history.reset(g_editor.scene);
}
