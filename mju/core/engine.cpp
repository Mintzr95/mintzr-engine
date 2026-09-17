#include "engine.h"
#include <algorithm>

namespace mju {

bool Engine::initialize(int w, int h) {
    if (w <= 0 || h <= 0) return false;
    width_ = w;
    height_ = h;
    frame_count_ = 0;

    scene_manager_.clear();
    active_scene_ = scene_manager_.create("main");
    if (!active_scene_) return false;

    camera_ = {};
    audio_.initialize();
    physics_.clear();
    particles_.clear();
    resources_.clear();
    ui_.clear();

    console_.info("MJU Engine initialized");
    initialized_ = true;
    return true;
}

Scene* Engine::create_scene(const std::string& name) {
    return scene_manager_.create(name);
}

bool Engine::open_scene(const std::string& name) {
    if (!scene_manager_.set_current(name)) return false;
    active_scene_ = scene_manager_.current();
    if (!active_scene_) return false;
    physics_.clear();
    return true;
}

void Engine::resize(int w, int h) {
    if (w > 0 && h > 0) {
        width_ = w;
        height_ = h;
        ui_.layout(static_cast<float>(w), static_cast<float>(h));
    }
}

void Engine::update(float dt) {
    if (!initialized_ || !active_scene_) return;
    dt = std::clamp(dt, 0.0f, 0.1f);

    for (auto& entity : active_scene_->entities()) {
        auto& sprite = entity.sprite;
        if (!sprite.animation_playing || sprite.frame_count <= 1 || sprite.fps <= 0.0f) continue;
        sprite.animation_time += dt;
        const float frame_step = 1.0f / sprite.fps;
        while (sprite.animation_time >= frame_step) {
            sprite.animation_time -= frame_step;
            ++sprite.frame;
            if (sprite.frame >= sprite.frame_count) {
                if (sprite.animation_loop) {
                    sprite.frame = 0;
                } else {
                    sprite.frame = sprite.frame_count - 1;
                    sprite.animation_playing = false;
                    break;
                }
            }
        }
    }

    ui_.layout(static_cast<float>(width_), static_cast<float>(height_));
    const auto touch = input_.primary();
    if (touch.pressed) ui_.pointer_down(touch.position);

    audio_.update(dt);
    const bool has_tiles = tilemap_.width() > 0 && tilemap_.height() > 0;
    physics_.step(*active_scene_, dt, {0, 0}, has_tiles ? &tilemap_ : nullptr);
    particles_.update(dt, camera_.position);
    ++frame_count_;
    console_.next_frame();
    input_.end_frame();
}

void Engine::shutdown() {
    audio_.shutdown();
    physics_.clear();
    particles_.clear();
    resources_.clear();
    ui_.clear();
    scene_manager_.clear();
    active_scene_ = nullptr;
    console_.info("MJU Engine shutdown");
    initialized_ = false;
}
}
