#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "../core/math.h"
namespace mju::tilemap {
struct Rect2{Vec2 position{};Vec2 size{};};
struct Tile{std::int32_t id=-1;std::uint8_t flip=0;};
class TileMap {
public:
 bool resize(std::int32_t width,std::int32_t height);std::int32_t width()const{return width_;}std::int32_t height()const{return height_;}
 Tile get(std::int32_t x,std::int32_t y)const;void set(std::int32_t x,std::int32_t y,Tile tile);bool fill(std::int32_t tile_id);
 bool save(const std::string&path)const;bool load(const std::string&path);std::vector<Rect2> visible_cells(const Rect2&world,float cell_size)const;
 void set_tileset(std::string texture_path,int tile_width,int tile_height,int columns){texture_path_=std::move(texture_path);tile_width_=tile_width;tile_height_=tile_height;columns_=columns;}
 const std::string& texture_path()const{return texture_path_;}int tile_width()const{return tile_width_;}int tile_height()const{return tile_height_;}int columns()const{return columns_;}
private:bool inside(std::int32_t x,std::int32_t y)const;std::int32_t width_=0,height_=0;std::vector<Tile>tiles_;std::string texture_path_;int tile_width_=32,tile_height_=32,columns_=1;
};
}
