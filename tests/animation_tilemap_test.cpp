#include "mju/core/animation.h"
#include "mju/tilemap/tilemap.h"
#include <cassert>
int main(){
 mju::AnimationPlayer p;p.play(4,10.0f,true,true);p.update(0.31f);assert(p.frame==3);p.update(0.10f);assert(p.frame==0);
 mju::tilemap::TileMap map;assert(map.resize(32,16));map.set_tileset("tiles.png",16,16,4);map.set(1,2,{3,1});assert(map.get(1,2).id==3);auto cells=map.visible_cells({{16*2.0f,16*2.0f},{64,64}},16.0f);assert(!cells.empty());
 return 0;
}
