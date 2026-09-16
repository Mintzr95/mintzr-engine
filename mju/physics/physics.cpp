#include "physics.h"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace mju::physics {
namespace {
struct Aabb { Vec2 min{}, max{}; };
Aabb bounds(const Entity& e){const Vec2 h{std::fabs(e.sprite.size.x*e.transform.scale.x)*0.5f,std::fabs(e.sprite.size.y*e.transform.scale.y)*0.5f};return {e.transform.position-h,e.transform.position+h};}
bool overlap(const Aabb&a,const Aabb&b,float&px,float&py){const float ox=std::min(a.max.x,b.max.x)-std::max(a.min.x,b.min.x),oy=std::min(a.max.y,b.max.y)-std::max(a.min.y,b.min.y);if(ox<=0||oy<=0)return false;px=ox;py=oy;return true;}
std::int64_t cell_key(int x,int y){return (static_cast<std::int64_t>(x)<<32)^static_cast<std::uint32_t>(y);}
std::uint64_t pair_key(EntityId a,EntityId b){if(a>b)std::swap(a,b);return (static_cast<std::uint64_t>(a)<<32)|b;}
}
Body& World::add_body(EntityId id,BodyType type){auto&b=bodies_[id];b.entity=id;b.type=type;return b;}
Body* World::get_body(EntityId id){auto it=bodies_.find(id);return it==bodies_.end()?nullptr:&it->second;}
const Body* World::get_body(EntityId id)const{auto it=bodies_.find(id);return it==bodies_.end()?nullptr:&it->second;}
void World::remove_body(EntityId id){bodies_.erase(id);}void World::clear(){bodies_.clear();}
void World::step(Scene&scene,float dt,Vec2 gravity){
 if(dt<=0)return;constexpr float cell=128.0f;std::unordered_map<std::int64_t,std::vector<EntityId>>grid;grid.reserve(bodies_.size()*2+1);
 for(auto&[id,b]:bodies_){auto*e=scene.find(id);if(!e||!e->active)continue;if(b.type==BodyType::Dynamic){if(b.useGravity)b.acceleration=gravity*b.gravityScale;b.velocity+=b.acceleration*dt;e->transform.position+=b.velocity*dt;}else if(b.type==BodyType::Kinematic)e->transform.position+=b.velocity*dt;b.acceleration={0,0};const auto a=bounds(*e);const int x0=(int)std::floor(a.min.x/cell),y0=(int)std::floor(a.min.y/cell),x1=(int)std::floor(a.max.x/cell),y1=(int)std::floor(a.max.y/cell);for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x)grid[cell_key(x,y)].push_back(id);}
 std::unordered_set<std::uint64_t>tested;tested.reserve(bodies_.size()*2+1);
 for(const auto&[unused,ids]:grid){(void)unused;for(std::size_t i=0;i<ids.size();++i)for(std::size_t j=i+1;j<ids.size();++j){const auto a_id=ids[i],b_id=ids[j];if(!tested.insert(pair_key(a_id,b_id)).second)continue;auto*A=scene.find(a_id);auto*B=scene.find(b_id);if(!A||!B||!A->active||!B->active)continue;Body*ba=get_body(a_id);Body*bb=get_body(b_id);if(!ba||!bb||(ba->type==BodyType::Static&&bb->type==BodyType::Static))continue;float px,py;if(!overlap(bounds(*A),bounds(*B),px,py))continue;const Vec2 d=B->transform.position-A->transform.position;const bool xaxis=px<py;const float pen=xaxis?px:py;const Vec2 n=xaxis?Vec2{d.x>=0?1.0f:-1.0f,0}:Vec2{0,d.y>=0?1.0f:-1.0f};const bool ad=ba->type==BodyType::Dynamic,bd=bb->type==BodyType::Dynamic;if(ad&&bd){A->transform.position-=n*(pen*.5f);B->transform.position+=n*(pen*.5f);}else if(ad){A->transform.position-=n*pen;}else if(bd){B->transform.position+=n*pen;}const float rvx=bb->velocity.x-ba->velocity.x,rvy=bb->velocity.y-ba->velocity.y,reln=rvx*n.x+rvy*n.y;if(reln>=0)continue;const float invA=ad?1.0f/std::max(.001f,ba->mass):0.0f,invB=bd?1.0f/std::max(.001f,bb->mass):0.0f,invSum=invA+invB;if(invSum<=0)continue;const float rest=std::clamp(ba->restitution*bb->restitution,0.0f,1.0f),imp=-(1.0f+rest)*reln/invSum;const Vec2 impulse=n*imp;if(ad)ba->velocity-=impulse*invA;if(bd)bb->velocity+=impulse*invB;const Vec2 t{-n.y,n.x};const float relt=rvx*t.x+rvy*t.y,mu=std::clamp(std::sqrt(std::max(0.0f,ba->friction*bb->friction)),0.0f,1.0f),fj=std::clamp(-relt/invSum,-imp*mu,imp*mu);const Vec2 fr=t*fj;if(ad)ba->velocity-=fr*invA;if(bd)bb->velocity+=fr*invB;}}
}
bool World::overlaps(const Scene&scene,EntityId a,EntityId b)const{auto*A=scene.find(a);auto*B=scene.find(b);if(!A||!B)return false;float px,py;return overlap(bounds(*A),bounds(*B),px,py);}
}
