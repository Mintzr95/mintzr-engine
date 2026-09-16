#include "engine.h"
#include <algorithm>
namespace mju {
bool Engine::initialize(int w,int h){
 if(w<=0||h<=0)return false;width_=w;height_=h;frame_count_=0;scene_.clear();camera_={};audio_.initialize();physics_.clear();particles_.clear();resources_.clear();ui_.clear();
 ui::Widget&play=ui_.add("play","PLAY");play.rect.size={180,72};play.anchor=ui::Anchor::Bottom;play.on_click=[this](){camera_.zoom=camera_.zoom>0.75f?0.75f:1.0f;};ui_.layout((float)w,(float)h);
 particles::EmitterConfig pc;pc.maxParticles=256;pc.emissionRate=20;pc.lifetime=0.8f;pc.speed=55;pc.size=7;pc.startColor={0.25f,0.8f,1,1};particles_.configure(pc);particles_.start();
 auto&root=scene_.create_entity("MJU Demo");root.transform.position={w*0.5f,h*0.5f};root.sprite.size={140,140};root.sprite.color={0.15f,0.75f,1.0f,1.0f};
 auto&button=scene_.create_entity("Touch Button");button.transform.position={100.0f,h-100.0f};button.sprite.size={160,80};button.sprite.color={0.25f,0.5f,0.95f,1.0f};console_.info("MJU Engine initialized");initialized_=true;return true;
}
void Engine::resize(int w,int h){if(w>0&&h>0){width_=w;height_=h;}}
void Engine::update(float dt){
 if(!initialized_)return;
 dt=std::clamp(dt,0.0f,0.1f);
 for(auto&e:scene_.entities()){
  if(e.name=="MJU Demo")e.transform.rotation+=dt*30.0f;
  auto&s=e.sprite;
  if(s.animation_playing&&s.frame_count>1&&s.fps>0.0f){s.animation_time+=dt;const float step=1.0f/s.fps;while(s.animation_time>=step){s.animation_time-=step;++s.frame;if(s.frame>=s.frame_count){if(s.animation_loop)s.frame=0;else{s.frame=s.frame_count-1;s.animation_playing=false;break;}}}}
 }
 auto*root=scene_.find(1);if(root)physics_.add_body(root->id,physics::BodyType::Kinematic);const auto&t=input_.primary();auto*button=scene_.find(2);
 if(button){const float hx=button->sprite.size.x*0.5f,hy=button->sprite.size.y*0.5f;const bool inside=t.down&&t.position.x>=button->transform.position.x-hx&&t.position.x<=button->transform.position.x+hx&&t.position.y>=button->transform.position.y-hy&&t.position.y<=button->transform.position.y+hy;button->sprite.color=inside?Color{0.8f,0.35f,0.2f,1.0f}:Color{0.25f,0.5f,0.95f,1.0f};}
 ui_.layout((float)width_,(float)height_);if(t.pressed)ui_.pointer_down(t.position);audio_.update(dt);physics_.step(scene_,dt,{0,0});particles_.update(dt,root?root->transform.position:Vec2{});++frame_count_;console_.next_frame();input_.end_frame();
}
void Engine::shutdown(){audio_.shutdown();physics_.clear();particles_.clear();resources_.clear();ui_.clear();scene_.clear();console_.info("MJU Engine shutdown");initialized_=false;}
}
