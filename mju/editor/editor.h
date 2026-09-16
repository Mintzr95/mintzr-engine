#pragma once
#include "../core/scene.h"
#include "undo.h"
#include "../assets/asset_manager.h"
#include "../tilemap/tilemap.h"
#include <string>

namespace mju::editor {

enum class Tool { Select, Move, Scale, Rotate };

struct ProjectSettings {
    std::string name = "My MJU Game";
    std::string main_scene = "main.mju";
    int width = 1280;
    int height = 720;
    bool portrait = true;
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
    void begin_transform();
    void end_transform();
    bool transform_editing() const { return transform_editing_; }

private:
    bool transform_editing_ = false;
};

bool save_project(const ProjectSettings& settings, const std::string& path);
bool load_project(ProjectSettings& settings, const std::string& path);

} // namespace mju::editor
