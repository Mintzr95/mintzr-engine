#include "mju/core/scene.h"
#include "mju/physics/physics.h"
#include "mju/particles/particles.h"
#include <cassert>
#include <cmath>

using namespace mju;

int main(){
    Scene scene(64);
    Entity& root=scene.create_entity("root");
    Entity& child=scene.create_entity("child",root.id);
    child.transform.position={10,5};root.transform.position={100,50};root.transform.scale={2,2};root.transform.rotation=90;
    const auto world=scene.world_transform(child.id);
    assert(std::fabs(world.position.x-90.0f)<0.01f);
    assert(std::fabs(world.position.y-70.0f)<0.01f);
    assert(scene.set_parent(child.id,0));
    assert(!scene.set_parent(root.id,child.id));

    Entity* duplicate=scene.duplicate_entity(root.id);
    assert(duplicate!=nullptr);
    assert(scene.children_of(duplicate->id).size()==1);
    assert(scene.destroy_entity(root.id));
    assert(scene.find(root.id)==nullptr);
    assert(scene.find(child.id)==nullptr);

    Scene physics_scene;
    auto& a=physics_scene.create_entity("a");auto& b=physics_scene.create_entity("b");
    a.transform.position={0,0};b.transform.position={40,0};a.sprite.size={50,50};b.sprite.size={50,50};
    physics::World world_physics;
    auto& ba=world_physics.add_body(a.id,physics::BodyType::Dynamic);ba.useGravity=false;ba.restitution=0.5f;
    auto& bb=world_physics.add_body(b.id,physics::BodyType::Static);
    ba.velocity={100,0};
    world_physics.step(physics_scene,0.5f,{0,0});
    assert(world_physics.overlaps(physics_scene,a.id,b.id)==false || ba.velocity.x<=0.0f);
    (void)bb;

    particles::Emitter emitter;particles::EmitterConfig cfg;cfg.maxParticles=32;cfg.emissionRate=100;cfg.lifetime=0.05f;emitter.configure(cfg);emitter.start();
    emitter.update(0.02f,{0,0});const auto first_capacity=emitter.particles().capacity();assert(!emitter.particles().empty());
    emitter.update(0.2f,{0,0});assert(emitter.particles().capacity()==first_capacity);assert(emitter.particles().size()<=32);
    return 0;
}
