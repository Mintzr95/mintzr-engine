#include "mju/core/scene.h"
#include "mju/scripting/script.h"
#include "mju/physics/physics.h"
#include <cassert>

using namespace mju;

int main() {
    Scene scene;
    auto& entity = scene.create_entity("scripted");

    scripting::ScriptVM vm;
    physics::World physics;
    auto& body = physics.add_body(entity.id, physics::BodyType::Dynamic);
    body.useGravity = false;

    scripting::ScriptContext context;
    context.physics = &physics;

    const std::string source =
        "set speed 2\n"
        "repeat 3 move speed 0\n"
        "fn jump\n"
        "set hops 2\n"
        "while hops > 0 sub hops 1\n"
        "velocity 0 -10\n"
        "endfn\n"
        "event start\n"
        "call jump\n"
        "endevent\n";

    const auto result = vm.execute(scene, entity.id, source, context);
    assert(result.ok);
    assert(entity.transform.position.x == 6.0f);

    const auto event_result = vm.execute_event(scene, entity.id, source, "start", context);
    assert(event_result.ok);
    assert(body.velocity.y == -10.0f);

    const auto missing_event = vm.execute_event(scene, entity.id, source, "missing", context);
    assert(!missing_event.ok);

    const auto unsafe = vm.execute(scene, entity.id, "while 1 == 1 add counter 1", context);
    assert(!unsafe.ok);
    assert(unsafe.message == "loop_limit");
    return 0;
}
