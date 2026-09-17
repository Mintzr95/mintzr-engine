#include "editor.h"
#include "../io/scene_io.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>

namespace mju::editor {

Entity* EditorState::create(const std::string& name, EntityId parent) {
    end_transform_edit();
    checkpoint();
    Entity& entity = scene.create_entity(name, parent);
    history.commit(scene);
    selected = entity.id;
    return &entity;
}

bool EditorState::select(EntityId id) {
    if (!scene.find(id)) return false;
    selected = id;
    return true;
}

Entity* EditorState::selected_entity() {
    return selected ? scene.find(selected) : nullptr;
}

bool EditorState::delete_selected() {
    if (!selected) return false;
    end_transform_edit();
    checkpoint();
    const bool ok = scene.destroy_entity(selected, true);
    if (ok) {
        history.commit(scene);
        selected = 0;
    }
    return ok;
}

Entity* EditorState::duplicate_selected() {
    if (!selected) return nullptr;
    end_transform_edit();
    checkpoint();
    Entity* entity = scene.duplicate_entity(selected);
    if (entity) {
        history.commit(scene);
        selected = entity->id;
    }
    return entity;
}

void EditorState::clear_selection() {
    selected = 0;
}

void EditorState::toggle_play() {
    end_transform_edit();
    playing = !playing;
}

bool EditorState::save(const std::string& path) {
    end_transform_edit();
    return save_scene(scene, path);
}

bool EditorState::load(const std::string& path) {
    end_transform_edit();
    const bool ok = load_scene(scene, path);
    if (ok) {
        selected = 0;
        history.reset(scene);
    }
    return ok;
}

bool EditorState::undo() {
    end_transform_edit();
    const bool ok = history.undo(scene);
    if (ok) selected = 0;
    return ok;
}

bool EditorState::redo() {
    end_transform_edit();
    const bool ok = history.redo(scene);
    if (ok) selected = 0;
    return ok;
}

void EditorState::checkpoint() {
    if (transform_edit_active_) return;
    history.checkpoint(scene);
}

void EditorState::begin_transform_edit() {
    if (transform_edit_active_) return;
    history.checkpoint(scene);
    transform_edit_active_ = true;
}

bool EditorState::set_transform(EntityId id, float x, float y) {
    auto* entity = scene.find(id);
    if (!entity || entity->locked) return false;
    const bool temporary_transaction = !transform_edit_active_;
    if (temporary_transaction) begin_transform_edit();
    entity->transform.position = {x, y};
    if (temporary_transaction) end_transform_edit();
    return true;
}

void EditorState::end_transform_edit() {
    if (!transform_edit_active_) return;
    history.commit(scene);
    transform_edit_active_ = false;
}

void EditorState::snap_position(Entity& entity) {
    if (!snap_to_grid || grid_size <= 0.0f) return;
    entity.transform.position.x = std::round(entity.transform.position.x / grid_size) * grid_size;
    entity.transform.position.y = std::round(entity.transform.position.y / grid_size) * grid_size;
}

bool save_project(const ProjectSettings& input, const std::string& path) {
    project::Settings settings;
    settings.name = input.name;
    settings.start_scene = input.main_scene;
    settings.width = input.width;
    settings.height = input.height;
    settings.target_fps = input.target_fps;
    settings.portrait = input.portrait;
    settings.vsync = input.vsync;
    settings.max_entities = input.max_entities;
    settings.renderer = input.renderer;
    return project::save_settings(settings, path);
}

bool load_project(ProjectSettings& output, const std::string& path) {
    project::Settings settings;
    if (!project::load_settings(settings, path)) return false;
    output.name = settings.name;
    output.main_scene = settings.start_scene;
    output.width = settings.width;
    output.height = settings.height;
    output.target_fps = settings.target_fps;
    output.portrait = settings.portrait;
    output.vsync = settings.vsync;
    output.max_entities = settings.max_entities;
    output.renderer = settings.renderer;
    return true;
}

} // namespace mju::editor
