#include "particles.h"
#include <cmath>
#include <algorithm>
namespace mju::particles { void Emitter::configure(const EmitterConfig& c){cfg_=c;particles_.reserve((size_t)c.maxParticles);} void Emitter::start(){running_=true;} void Emitter::stop(){running_=false;} void Emitter::clear(){particles_.clear();accumulator_=0;}
float Emitter::rand01(){seed_=1664525u*seed_+1013904223u;return (seed_&0x00ffffff)/16777215.0f;}
void Emitter::emit(Vec2 o){if((int)particles_.size()>=cfg_.maxParticles)return;float a=(rand01()-0.5f)*cfg_.spread;float s=cfg_.speed*(0.75f+0.5f*rand01());Particle p;p.position=o;p.velocity={std::cos(a)*s,std::sin(a)*s};p.color=cfg_.startColor;p.life=p.maxLife=cfg_.lifetime;p.size=cfg_.size;particles_.push_back(p);}
void Emitter::update(float dt,Vec2 o){if(dt<=0)return;if(running_&&cfg_.emissionRate>0){accumulator_+=dt*cfg_.emissionRate;while(accumulator_>=1){emit(o);accumulator_-=1;}}for(auto&p:particles_){p.position+=p.velocity*dt;p.velocity.y+=80*dt;p.life-=dt;float t=p.life>0?p.life/p.maxLife:0;p.color.a=t;}particles_.erase(std::remove_if(particles_.begin(),particles_.end(),[](const Particle&p){return p.life<=0;}),particles_.end());}}
