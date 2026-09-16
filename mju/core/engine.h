#pragma once
#include "scene.h"
#include "scene_manager.h"
#include "camera.h"
#include "input.h"
#include "animation.h"
#include "../audio/audio.h"
#include "../physics/physics.h"
#include "../particles/particles.h"
#include "../resources/resources.h"
#include "../tilemap/tilemap.h"
#include "../ui/ui.h"
#include "../scripting/script.h"
#include "../diagnostics/debug.h"
#include "../project/project.h"

namespace mju {

class Engine {
public:
    bool initialize(int w, int h);
    void resize(int w, int h);
    void update(float dt);
    void shutdown();

    Scene& scene() { return *active_scene_; }
    const Scene& scene() const { return *active_scene_; }
    SceneManager& scene_manager() { return scene_manager_; }
    const SceneManager& scene_manager() const { return scene_manager_; }
    Scene* create_scene(const std::string& name);
    bool open_scene(const std::string& name);
    Camera2D& camera() { return camera_; }
    const Camera2D& camera() const { return camera_; }
    InputState& input() { return input_; }
    const InputState& input() const { return input_; }
    AnimationLibrary& animations() { return animations_; }
    const AnimationLibrary& animations() const { return animations_; }
    AudioSystem& audio() { return audio_; }
    physics::World& physics() { return physics_; }
    particles::Emitter& particles() { return particles_; }
    resources::ResourceCatalog& resources() { return resources_; }
    tilemap::TileMap& tilemap() { return tilemap_; }
    const tilemap::TileMap& tilemap() const { return tilemap_; }
    ui::Canvas& ui() { return ui_; }
    const ui::Canvas& ui() const { return ui_; }
    scripting::ScriptVM& scripts() { return scripts_; }
    debug::Console& console() { return console_; }
    project::Settings& settings() { return settings_; }
    const project::Settings& settings() const { return settings_; }

    int width() const { return width_; }
    int height() const { return height_; }
    bool initialized() const { return initialized_; }
    unsigned long long frame_count() const { return frame_count_; }

private:
    int width_ = 0;
    int height_ = 0;
    bool initialized_ = false;
    unsigned long long frame_count_ = 0;
    EntityId demo_entity_ = 0;
    EntityId button_entity_ = 0;
    Scene* active_scene_ = nullptr;

    SceneManager scene_manager_;
    Camera2D camera_;
    InputState input_;
    AnimationLibrary animations_;
    AudioSystem audio_;
    physics::World physics_;
    particles::Emitter particles_;
    resources::ResourceCatalog resources_;
    tilemap::TileMap tilemap_;
    ui::Canvas ui_;
    scripting::ScriptVM scripts_;
    debug::Console console_;
    project::Settings settings_;
};

}
