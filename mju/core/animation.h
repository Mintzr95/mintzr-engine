#pragma once
#include <cstddef>
namespace mju {
struct AnimationPlayer {
 int frame_count=1; float fps=8.0f; float time=0.0f; int frame=0; bool looping=true; bool playing=true;
 void update(float dt){ if(!playing||frame_count<2||fps<=0)return; time+=dt; const float step=1.0f/fps; while(time>=step){time-=step; ++frame; if(frame>=frame_count){if(looping)frame=0;else{frame=frame_count-1;playing=false;break;}}}}
};
}
