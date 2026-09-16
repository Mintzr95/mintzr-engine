#pragma once
#include <algorithm>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace mju {
struct AnimationClip {
    std::string name;
    std::vector<int> frames;
    float fps=8.0f;
    bool looping=true;
};

class AnimationLibrary {
public:
    void clear(){clips_.clear();}
    void add(AnimationClip clip){if(!clip.name.empty()&&!clip.frames.empty())clips_[clip.name]=std::move(clip);}
    const AnimationClip* find(const std::string& name) const{auto it=clips_.find(name);return it==clips_.end()?nullptr:&it->second;}
    std::size_t size() const{return clips_.size();}
private:
    std::unordered_map<std::string,AnimationClip> clips_;
};

struct AnimationPlayer {
    int frame_count=1; float fps=8.0f; float time=0.0f; int frame=0; bool looping=true; bool playing=true;
    void play(int count,float rate,bool loop=true,bool restart=true){frame_count=std::max(1,count);fps=std::max(0.0f,rate);looping=loop;if(restart){time=0.0f;frame=0;}playing=true;}
    void stop(){playing=false;} void pause(){playing=false;} void resume(){playing=true;}
    void update(float dt){if(!playing||frame_count<2||fps<=0.0f||dt<=0.0f)return;time+=dt;const float step=1.0f/fps;while(time>=step){time-=step;++frame;if(frame>=frame_count){if(looping)frame=0;else{frame=frame_count-1;playing=false;break;}}}}
};
}
