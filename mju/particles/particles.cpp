#include "particles.h"
#include <algorithm>
#include <cmath>

namespace mju::particles {
void Emitter::configure(const EmitterConfig& c) {
    cfg_ = c;
    cfg_.maxParticles = std::max(1, cfg_.maxParticles);
    cfg_.emissionRate = std::max(0.0f, cfg_.emissionRate);
    cfg_.lifetime = std::max(0.001f, cfg_.lifetime);
    particles_.clear();
    particles_.reserve(static_cast<std::size_t>(cfg_.maxParticles));
    accumulator_ = 0.0f;
}
void Emitter::start(){running_=true;}
void Emitter::stop(){running_=false;}
void Emitter::clear(){particles_.clear();accumulator_=0.0f;}
float Emitter::rand01(){seed_=1664525u*seed_+1013904223u;return (seed_&0x00ffffffu)/16777215.0f;}
void Emitter::emit(Vec2 o){
    if(static_cast<int>(particles_.size())>=cfg_.maxParticles)return;
    const float a=(rand01()-0.5f)*cfg_.spread;
    const float s=cfg_.speed*(0.75f+0.5f*rand01());
    Particle p; p.position=o; p.velocity={std::cos(a)*s,std::sin(a)*s}; p.color=cfg_.startColor;
    p.life=p.maxLife=cfg_.lifetime; p.size=cfg_.size; particles_.push_back(p);
}
void Emitter::update(float dt,Vec2 o){
    if(dt<=0) return;
    if(running_&&cfg_.emissionRate>0){
        accumulator_+=dt*cfg_.emissionRate;
        const int emit_limit=cfg_.maxParticles-static_cast<int>(particles_.size());
        int emitted=0;
        while(accumulator_>=1.0f&&emitted<emit_limit){emit(o);accumulator_-=1.0f;++emitted;}
        // Avoid an ever-growing backlog if the app was backgrounded.
        if(emitted==emit_limit&&accumulator_>2.0f) accumulator_=2.0f;
    }
    for(std::size_t i=0;i<particles_.size();) {
        Particle& p=particles_[i];
        p.position+=p.velocity*dt;
        p.velocity.y+=80.0f*dt;
        p.life-=dt;
        p.color.a=p.life>0.0f?std::clamp(p.life/p.maxLife,0.0f,1.0f):0.0f;
        if(p.life<=0.0f){
            particles_[i]=std::move(particles_.back());
            particles_.pop_back();
            continue;
        }
        ++i;
    }
}
}
