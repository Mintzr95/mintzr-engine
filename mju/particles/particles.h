#pragma once
#include "../core/math.h"
#include <vector>
#include <cstdint>
namespace mju::particles {
struct Particle { Vec2 position{},velocity{}; Color color{}; float life=0,maxLife=1,size=8; };
struct EmitterConfig { int maxParticles=128; float emissionRate=40; float lifetime=1.2f; float speed=80; float spread=6.2831853f; Color startColor{1,1,1,1}; float size=8; };
class Emitter { public: void configure(const EmitterConfig& c); void start(); void stop(); void clear(); void update(float dt,Vec2 origin); const std::vector<Particle>& particles()const{return particles_;} bool running()const{return running_;}
private: EmitterConfig cfg_{}; std::vector<Particle> particles_; float accumulator_=0; bool running_=false; std::uint32_t seed_=1; float rand01(); void emit(Vec2 origin);}; }
