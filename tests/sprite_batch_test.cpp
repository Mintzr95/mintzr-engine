#include "mju/render/sprite_batch.h"
#include <cassert>

int main() {
    mju::render::SpriteBatch batch(8);
    batch.begin();

    mju::render::SpriteQuad back;
    back.position = {50, 50};
    back.size = {20, 10};
    back.texture_id = 2;
    back.layer = 0;
    batch.submit(back);

    mju::render::SpriteQuad front;
    front.position = {100, 50};
    front.size = {16, 16};
    front.texture_id = 1;
    front.layer = 1;
    batch.submit(front);

    batch.end();

    const auto& stats = batch.stats();
    assert(stats.sprites == 2);
    assert(stats.vertices == 8);
    assert(stats.indices == 12);
    assert(stats.batches == 2);

    const auto& vertices = batch.vertices();
    assert(vertices.size() == 8);
    assert(vertices[0].position.x == 40.0f);
    assert(vertices[0].position.y == 45.0f);

    mju::render::SpriteBatch capped(1);
    capped.begin();
    capped.submit(back);
    capped.submit(front);
    capped.end();
    assert(capped.stats().sprites == 1);
    assert(capped.stats().vertices == 4);
    assert(capped.stats().indices == 6);

    return 0;
}
