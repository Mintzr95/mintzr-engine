#include "demo_scene.h"

namespace mju::samples {

void install_basic_demo(Engine& engine) {
    auto& scene = engine.scene();
    if (scene.size() != 0) return;

    auto& root = scene.create_entity("MJU Demo");
    root.transform.position = {
        static_cast<float>(engine.width()) * 0.5f,
        static_cast<float>(engine.height()) * 0.5f
    };
    root.sprite.size = {140, 140};
    root.sprite.color = {0.15f, 0.75f, 1.0f, 1.0f};

    auto& button = scene.create_entity("Touch Button");
    button.transform.position = {100.0f, static_cast<float>(engine.height()) - 100.0f};
    button.sprite.size = {160, 80};
    button.sprite.color = {0.25f, 0.5f, 0.95f, 1.0f};

    auto& play = engine.ui().add("play", "PLAY");
    play.rect.size = {180, 72};
    play.anchor = ui::Anchor::Bottom;
    play.on_click = [&engine]() {
        engine.camera().zoom = engine.camera().zoom > 0.75f ? 0.75f : 1.0f;
    };
    engine.ui().layout(static_cast<float>(engine.width()), static_cast<float>(engine.height()));

    particles::EmitterConfig particles;
    particles.maxParticles = 256;
    particles.emissionRate = 20;
    particles.lifetime = 0.8f;
    particles.speed = 55;
    particles.size = 7;
    particles.startColor = {0.25f, 0.8f, 1.0f, 1.0f};
    engine.particles().configure(particles);
    engine.particles().start();
}

} // namespace mju::samples
