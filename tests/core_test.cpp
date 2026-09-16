#include "mju/core/engine.h"
#include "mju/io/scene_io.h"
#include <cassert>
#include <iostream>
#include <string>

int main() {
    mju::Engine engine;
    assert(engine.initialize(800, 480));
    assert(engine.scene_manager().current_name() == "main");

    auto* secondary = engine.create_scene("secondary");
    assert(secondary != nullptr);
    secondary->create_entity("SecondaryEntity");
    assert(engine.open_scene("secondary"));
    assert(engine.scene().find(1) != nullptr);
    assert(engine.open_scene("main"));

    auto& a = engine.scene().create_entity("A");
    auto& b = engine.scene().create_entity("B");
    assert(a.id != b.id);
    engine.update(1.0f / 60.0f);

    const std::string src = "move 10 20\nrotate 5\ncolor 1 0 0 1";
    auto result = engine.scripts().execute(engine.scene(), a.id, src);
    assert(result.ok);
    assert(a.transform.position.x == 10 && a.transform.position.y == 20);

    const std::string path = "mju_core_test_scene.mju";
    assert(mju::save_scene(engine.scene(), path));
    mju::Scene loaded;
    assert(mju::load_scene(loaded, path));
    assert(loaded.entities().size() == engine.scene().entities().size());

    std::remove(path.c_str());
    engine.shutdown();
    std::cout << "MJU core regression test: PASS\n";
    return 0;
}
