#include "sprite_batch.h"
#include <algorithm>
#include <cmath>
namespace mju::render {
SpriteBatch::SpriteBatch(std::size_t capacity):capacity_(std::max<std::size_t>(1,capacity)){reserve(capacity_);}
void SpriteBatch::reserve(std::size_t capacity){capacity_=std::max<std::size_t>(1,capacity);queued_.reserve(capacity_);vertices_.reserve(capacity_*4);indices_.reserve(capacity_*6);ranges_.reserve(capacity_);}
void SpriteBatch::begin(){queued_.clear();vertices_.clear();indices_.clear();ranges_.clear();stats_={};active_=true;}
void SpriteBatch::submit(const SpriteQuad&q){if(active_&&queued_.size()<capacity_)queued_.push_back(q);}
void SpriteBatch::end(){if(!active_)return;rebuild_geometry();active_=false;}
void SpriteBatch::rebuild_geometry(){
 std::stable_sort(queued_.begin(),queued_.end(),[](const SpriteQuad&a,const SpriteQuad&b){if(a.layer!=b.layer)return a.layer<b.layer;return a.texture_id<b.texture_id;});
 std::uint32_t base=0; stats_.sprites=queued_.size();
 for(const auto&q:queued_){
  const auto index_start=indices_.size();
  const float hx=q.size.x*0.5f,hy=q.size.y*0.5f,r=q.rotation*3.14159265358979323846f/180.0f,c=std::cos(r),s=std::sin(r);
  const Vec2 p[4]={{-hx,-hy},{hx,-hy},{hx,hy},{-hx,hy}}; const Vec2 uv[4]={{q.uv_min.x,q.uv_min.y},{q.uv_max.x,q.uv_min.y},{q.uv_max.x,q.uv_max.y},{q.uv_min.x,q.uv_max.y}};
  for(int i=0;i<4;++i){const float x=p[i].x,y=p[i].y;vertices_.push_back({{q.position.x+x*c-y*s,q.position.y+x*s+y*c},uv[i],q.color});}
  indices_.insert(indices_.end(),{base,base+1,base+2,base,base+2,base+3}); base+=4;
  if(ranges_.empty()||ranges_.back().texture_id!=q.texture_id||ranges_.back().index_count==0)ranges_.push_back({q.texture_id,index_start,6});else ranges_.back().index_count+=6;
 }
 stats_.batches=ranges_.size();stats_.vertices=vertices_.size();stats_.indices=indices_.size();
}
}
