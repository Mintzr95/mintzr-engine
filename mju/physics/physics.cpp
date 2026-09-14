#include "physics.h"
#include <algorithm>
namespace mju::physics {
Body& World::add_body(EntityId id,BodyType type){auto& b=bodies_[id]; b.entity=id;b.type=type;return b;}
Body* World::get_body(EntityId id){auto it=bodies_.find(id);return it==bodies_.end()?nullptr:&it->second;}
void World::remove_body(EntityId id){bodies_.erase(id);} void World::clear(){bodies_.clear();}
void World::step(Scene& scene,float dt,Vec2 gravity){ if(dt<=0) return; for(auto& [id,b]:bodies_){auto* e=scene.find(id);if(!e||b.type==BodyType::Static||!e->active)continue; if(b.type==BodyType::Dynamic){if(b.useGravity)b.acceleration=gravity*b.gravityScale;b.velocity+=b.acceleration*dt;e->transform.position+=b.velocity*dt;} if(b.type==BodyType::Kinematic){e->transform.position+=b.velocity*dt;} b.acceleration={0,0};}}
bool World::overlaps(const Scene& scene,EntityId a,EntityId b) const {auto* A=scene.find(a);auto* B=scene.find(b);if(!A||!B)return false;return intersects({A->transform.position,A->sprite.size*0.5f},{B->transform.position,B->sprite.size*0.5f});}
}
