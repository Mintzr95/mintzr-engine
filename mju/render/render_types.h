#pragma once
#include "../core/math.h"
#include <string>
namespace mju::render {
struct SpriteMaterial { std::string texture; Vec2 uv_min{0,0}; Vec2 uv_max{1,1}; bool nearest=true; };
struct DrawCommand { Vec2 position{}; Vec2 size{1,1}; float rotation=0; Color color{1,1,1,1}; int layer=0; SpriteMaterial material{}; };
}
