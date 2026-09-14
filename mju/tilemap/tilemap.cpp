#include "tilemap.h"
#include <fstream>
#include <cmath>
#include <algorithm>
namespace mju::tilemap {
bool TileMap::resize(std::int32_t w,std::int32_t h){if(w<0||h<0)return false;width_=w;height_=h;tiles_.assign(size_t(w)*size_t(h),{});return true;}
bool TileMap::inside(std::int32_t x,std::int32_t y) const{return x>=0&&y>=0&&x<width_&&y<height_;}
Tile TileMap::get(std::int32_t x,std::int32_t y) const{return inside(x,y)?tiles_[size_t(y)*size_t(width_)+size_t(x)]:Tile{};}
void TileMap::set(std::int32_t x,std::int32_t y,Tile t){if(inside(x,y))tiles_[size_t(y)*size_t(width_)+size_t(x)]=t;}
bool TileMap::fill(std::int32_t id){for(auto&t:tiles_)t=Tile{id,0};return true;}
bool TileMap::save(const std::string&p)const{std::ofstream f(p);if(!f)return false;f<<"MJU_TILEMAP 1\n"<<width_<<' '<<height_<<'\n';for(auto&t:tiles_)f<<t.id<<' '<<unsigned(t.flip)<<'\n';return bool(f);}
bool TileMap::load(const std::string&p){std::ifstream f(p);if(!f)return false;std::string h;int v;std::int32_t w,hg;if(!(f>>h>>v)||h!="MJU_TILEMAP"||v!=1||!(f>>w>>hg)||w<0||hg<0)return false;std::vector<Tile> n(size_t(w)*size_t(hg));for(auto&t:n){unsigned fl;if(!(f>>t.id>>fl)||fl>255)return false;t.flip=std::uint8_t(fl);}width_=w;height_=hg;tiles_=std::move(n);return true;}
std::vector<Rect2> TileMap::visible_cells(const Rect2& world,float cell_size) const{std::vector<Rect2> out;if(cell_size<=0)return out;int x0=std::max(0,(int)std::floor((world.position.x-world.size.x*0.5f)/cell_size));int y0=std::max(0,(int)std::floor((world.position.y-world.size.y*0.5f)/cell_size));int x1=std::min(width_-1,(int)std::ceil((world.position.x+world.size.x*0.5f)/cell_size));int y1=std::min(height_-1,(int)std::ceil((world.position.y+world.size.y*0.5f)/cell_size));for(int y=y0;y<=y1;y++)for(int x=x0;x<=x1;x++)if(get(x,y).id>=0)out.push_back({{(x+0.5f)*cell_size,(y+0.5f)*cell_size},{cell_size,cell_size}});return out;}
}
