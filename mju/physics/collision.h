#pragma once
#include "../core/math.h"
namespace mju {
struct AABB { Vec2 center{}; Vec2 half{0.5f,0.5f}; };
inline bool intersects(const AABB&a,const AABB&b){return a.center.x-a.half.x < b.center.x+b.half.x && a.center.x+a.half.x > b.center.x-b.half.x && a.center.y-a.half.y < b.center.y+b.half.y && a.center.y+a.half.y > b.center.y-b.half.y;}
}
