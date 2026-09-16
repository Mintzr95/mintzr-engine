#include "engine.h"
#include <algorithm>

namespace mju {

bool Engine::initialize(int w, int h) {
    if (w <= 0 || h <= 0) return false;
    width_ = w;
    height_ = h;
    frame_count_ = 0;
    demo_entity_ = button_entity_ = 0;
    scene_.clear();
    camera_ = {};
    audio_.initialize();
    physics_.clear();
    particles_.clear();
    resources_.clear();
    ui_.clear();

    ui::Widget& play = ui_.add("play", "PLAY");
    play.rect.size = {180, 72};
    play.anchor = ui::Anchor::Bottom;
    play.on_click = [this]() {
        camera_.zoom = camera_.zoom > 0.75f ? 0.75f : 1.0f;
    };
    ui_.layout(static_cast<float>(w), static_cast<float>(h));

    particles::EmitterConfig pc;
    pc.maxParticles = 256;
    pc.emissionRate = 20;
    pc.lifetime = 0.8f;
    pc.speed = 55;
    pc.size = 7;
    pc.startColor = {0.25f, 0.8f, 1, 1};
    particles_.configure(pc);
    particles_.start();

    auto& root = scene_.create_entity("MJU Demo");
    demo_entity_ = root.id;
    root.transform.position = {w * 0.5f, h * 0.5f};
    root.sprite.size = {140, 140};
    root.sprite.color = {0.15f, 0.75f, 1.0f, 1.0f};

    auto& button = scene_.create_entity("Touch Button");
    button_entity_ = button.id;
    button.transform.position = {100.0f, h - 100.0f};
    button.sprite.size = {160, 80};
    button.sprite.color = {0.25f, 0.5f, 0.95f, 1.0f};

    console_.info("MJU Engine initialized");
    initialized_ = true;
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
    if (!initialized_) return;
    dt = std::clamp(dt, 0.0f, 0.1f);

    for (auto& entity : scene_.entities()) {
        if (entity.id == demo_entity_) {
            entity.transform.rotation += dt * 30.0f;
        }

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

    auto* root = scene_.find(demo_entity_);
    const auto touch = input_.primary();
    auto* button = scene_.find(button_entity_);
    if (button) {
        const float half_x = button->sprite.size.x * 0.5f;
        const float half_y = button->sprite.size.y * 0.5f;
        const bool inside = touch.down &&
            touch.position.x >= button->transform.position.x - half_x &&
            touch.position.x <= button->transform.position.x + half_x &&
            touch.position.y >= button->transform.position.y - half_y &&
            touch.position.y <= button->transform.position.y + half_y;
        button->sprite.color = inside
            ? Color{0.8f, 0.35f, 0.2f, 1.0f}
            : Color{0.25f, 0.5f, 0.95f, 1.0f};
    }

    ui_.layout(static_cast<float>(width_), static_cast<float>(height_));
    if (touch.pressed) ui_.pointer_down(touch.position);

    audio_.update(dt);
    const bool has_tiles = tilemap_.width() > 0 && tilemap_.height() > 0;
    physics_.step(scene_, dt, {0, 0}, has_tiles ? &tilemap_ : nullptr);
    particles_.update(dt, root ? root->transform.position : Vec2{});
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
    scene_.clear();
    demo_entity_ = button_entity_ = 0;
    console_.info("MJU Engine shutdown");
    initialized_ = false;
}
}
