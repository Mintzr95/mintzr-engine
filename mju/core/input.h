#pragma once
#include "math.h"
namespace mju {
struct TouchPoint { int id=-1; Vec2 position{}; bool down=false; bool pressed=false; };
class InputState {
public:
 void pointer_down(int id,float x,float y){primary_.id=id;primary_.position={x,y};primary_.down=true;primary_.pressed=true;}
 void pointer_move(int id,float x,float y){if(primary_.id==id)primary_.position={x,y};}
 void pointer_up(int id){if(primary_.id==id)primary_.down=false;}
 const TouchPoint& primary() const{return primary_;}
 void end_frame(){primary_.pressed=false;}
private: TouchPoint primary_{};
};
}
