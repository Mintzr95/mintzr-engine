#include "tilemap.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <utility>
namespace mju::tilemap {
bool TileMap::resize(std::int32_t w,std::int32_t h){if(w<0||h<0)return false;width_=w;height_=h;tiles_.assign(size_t(w)*size_t(h),{});return true;}
bool TileMap::inside(std::int32_t x,std::int32_t y)const{return x>=0&&y>=0&&x<width_&&y<height_;}
Tile TileMap::get(std::int32_t x,std::int32_t y)const{return inside(x,y)?tiles_[size_t(y)*size_t(width_)+size_t(x)]:Tile{};}
void TileMap::set(std::int32_t x,std::int32_t y,Tile t){if(inside(x,y))tiles_[size_t(y)*size_t(width_)+size_t(x)]=t;}
bool TileMap::fill(std::int32_t id){for(auto&t:tiles_)t=Tile{id,0};return true;}
bool TileMap::save(const std::string&p)const{std::ofstream f(p);if(!f)return false;f<<"MJU_TILEMAP 2\n"<<width_<<' '<<height_<<'\n'<<tile_width_<<' '<<tile_height_<<' '<<columns_<<'\n'<<texture_path_<<'\n';for(auto&t:tiles_)f<<t.id<<' '<<unsigned(t.flip)<<'\n';return bool(f);}
bool TileMap::load(const std::string&p){std::ifstream f(p);if(!f)return false;std::string magic;int version;std::int32_t w,h;if(!(f>>magic>>version)||magic!="MJU_TILEMAP"||(version!=1&&version!=2)||!(f>>w>>h)||w<0||h<0)return false;int tw=32,th=32,cols=1;std::string texture;f.ignore(1024,'\n');if(version==2){if(!(f>>tw>>th>>cols))return false;f.ignore(1024,'\n');std::getline(f,texture);}std::vector<Tile>n(size_t(w)*size_t(h));for(auto&t:n){unsigned fl;if(!(f>>t.id>>fl)||fl>255)return false;t.flip=(std::uint8_t)fl;}width_=w;height_=h;tiles_=std::move(n);tile_width_=std::max(1,tw);tile_height_=std::max(1,th);columns_=std::max(1,cols);texture_path_=std::move(texture);return true;}
std::vector<Rect2> TileMap::visible_cells(const Rect2&world,float cell_size)const{std::vector<Rect2>out;if(cell_size<=0||width_<=0||height_<=0)return out;int x0=std::max(0,(int)std::floor((world.position.x-world.size.x*0.5f)/cell_size));int y0=std::max(0,(int)std::floor((world.position.y-world.size.y*0.5f)/cell_size));int x1=std::min(width_-1,(int)std::ceil((world.position.x+world.size.x*0.5f)/cell_size));int y1=std::min(height_-1,(int)std::ceil((world.position.y+world.size.y*0.5f)/cell_size));if(x1<x0||y1<y0)return out;out.reserve(size_t(x1-x0+1)*size_t(y1-y0+1));for(int y=y0;y<=y1;y++)for(int x=x0;x<=x1;x++)if(get(x,y).id>=0)out.push_back({{(x+0.5f)*cell_size,(y+0.5f)*cell_size},{cell_size,cell_size}});return out;}
}
