#pragma once
#include "../core/scene.h"
#include "../project/project.h"
#include "undo.h"
#include "../assets/asset_manager.h"
#include "../tilemap/tilemap.h"
#include <string>

namespace mju::editor {

enum class Tool { Select, Move, Scale, Rotate };

struct ProjectSettings {
    std::string name = "My MJU Game";
    std::string main_scene = "main.mjuscene";
    int width = 1280;
    int height = 720;
    int target_fps = 60;
    bool portrait = true;
    bool vsync = true;
    int max_entities = 65536;
    std::string renderer = "gles2";
};

class EditorState {
public:
    Scene scene;
    ProjectSettings project;
    Tool tool = Tool::Select;
    EntityId selected = 0;
    bool playing = false;
    History history;
    assets::AssetManager assets;
    tilemap::TileMap tilemap;
    bool grid_enabled = true;
    float grid_size = 32.0f;
    float zoom = 1.0f;
    bool snap_to_grid = true;
    void set_zoom(float z) { zoom = z < 0.25f ? 0.25f : (z > 4.0f ? 4.0f : z); }
    void snap_position(Entity& e);

    Entity* create(const std::string& name, EntityId parent = 0);
    bool select(EntityId id);
    Entity* selected_entity();
    bool delete_selected();
    Entity* duplicate_selected();
    void clear_selection();
    void toggle_play();
    bool save(const std::string& path);
    bool load(const std::string& path);
    bool undo();
    bool redo();
    void checkpoint();

    void begin_transform_edit();
    bool set_transform(EntityId id, float x, float y);
    void end_transform_edit();
    bool transform_edit_active() const { return transform_edit_active_; }

private:
    bool transform_edit_active_ = false;
};

bool save_project(const ProjectSettings& settings, const std::string& path);
bool load_project(ProjectSettings& settings, const std::string& path);

} // namespace mju::editor
