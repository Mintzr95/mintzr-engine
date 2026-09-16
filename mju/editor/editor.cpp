#include "editor.h"
#include "../io/scene_io.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>

namespace mju::editor {

Entity* EditorState::create(const std::string& name, EntityId parent) {
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
    playing = !playing;
}

bool EditorState::save(const std::string& path) {
    return save_scene(scene, path);
}

bool EditorState::load(const std::string& path) {
    const bool ok = load_scene(scene, path);
    if (ok) {
        selected = 0;
        history.reset(scene);
    }
    return ok;
}

bool EditorState::undo() {
    const bool ok = history.undo(scene);
    if (ok) selected = 0;
    return ok;
}

bool EditorState::redo() {
    const bool ok = history.redo(scene);
    if (ok) selected = 0;
    return ok;
}

void EditorState::checkpoint() {
    history.checkpoint(scene);
}

void EditorState::snap_position(Entity& entity) {
    if (!snap_to_grid || grid_size <= 0.0f) return;
    entity.transform.position.x = std::round(entity.transform.position.x / grid_size) * grid_size;
    entity.transform.position.y = std::round(entity.transform.position.y / grid_size) * grid_size;
}

bool save_project(const ProjectSettings& settings, const std::string& path) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) return false;
    file << "MJU_PROJECT 1\n"
         << "name " << std::quoted(settings.name) << "\n"
         << "main_scene " << std::quoted(settings.main_scene) << "\n"
         << "width " << std::clamp(settings.width, 64, 16384) << "\n"
         << "height " << std::clamp(settings.height, 64, 16384) << "\n"
         << "portrait " << settings.portrait << "\n";
    return static_cast<bool>(file);
}

bool load_project(ProjectSettings& settings, const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    std::string header;
    int version = 0;
    if (!(file >> header >> version) || header != "MJU_PROJECT" || version != 1) return false;

    std::string key;
    while (file >> key) {
        if (key == "name") {
            file >> std::quoted(settings.name);
        } else if (key == "main_scene") {
            file >> std::quoted(settings.main_scene);
        } else if (key == "width") {
            file >> settings.width;
        } else if (key == "height") {
            file >> settings.height;
        } else if (key == "portrait") {
            file >> settings.portrait;
        } else {
            std::string ignored;
            std::getline(file, ignored);
        }
    }

    settings.width = std::clamp(settings.width, 64, 16384);
    settings.height = std::clamp(settings.height, 64, 16384);
    if (settings.name.empty()) settings.name = "My MJU Game";
    if (settings.main_scene.empty()) settings.main_scene = "main.mju";
    return true;
}
}
