#include <cassert>
#include <string>
#include "mju/core/scene.h"
#include "mju/io/scene_json.h"

int main() {
    mju::Scene source;
    auto& player = source.create_entity("Player");
    player.transform.position = {120.0f, 80.0f};
    player.transform.scale = {1.5f, 1.25f};
    player.transform.rotation = 17.0f;
    player.sprite.texture = "characters/player.png";
    player.sprite.size = {64.0f, 96.0f};
    player.sprite.frame_width = 64;
    player.sprite.frame_height = 96;
    player.sprite.frame = 3;
    player.sprite.frame_count = 8;
    player.sprite.fps = 12.0f;
    player.sprite.animation_time = 0.25f;
    player.sprite.animation_loop = false;
    player.sprite.animation_playing = true;
    player.layer = 4;

    const std::string text = mju::io::scene_to_json(source, 0);
    mju::Scene restored;
    assert(mju::io::scene_from_json(restored, text));
    assert(restored.entities().size() == 1);
    const auto& copy = restored.entities().front();
    assert(copy.name == "Player");
    assert(copy.sprite.texture == "characters/player.png");
    assert(copy.layer == 4);
    assert(copy.sprite.size.x == 64.0f && copy.sprite.size.y == 96.0f);
    assert(copy.transform.position.x == 120.0f && copy.transform.position.y == 80.0f);
    assert(copy.sprite.frame == 3);
    assert(copy.sprite.frame_count == 8);
    assert(copy.sprite.fps == 12.0f);
    assert(copy.sprite.animation_time == 0.25f);
    assert(!copy.sprite.animation_loop);
    assert(copy.sprite.animation_playing);

    std::string tampered = text;
    const auto marker = tampered.find("Player");
    assert(marker != std::string::npos);
    tampered.replace(marker, 6, "Hacker");
    mju::Scene rejected;
    assert(!mju::io::scene_from_json(rejected, tampered));

    return 0;
}
