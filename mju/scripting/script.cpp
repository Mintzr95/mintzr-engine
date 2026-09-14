#include "script.h"
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace mju::scripting {
std::vector<std::string> ScriptVM::split(const std::string& line){
    std::istringstream ss(line); std::vector<std::string> out; std::string s;
    while(ss>>s) out.push_back(s); return out;
}
ScriptResult ScriptVM::execute(Scene& scene, EntityId self, const std::string& source){
    Entity* e=scene.find(self); if(!e) return {false,"entity_not_found"};
    std::istringstream lines(source); std::string line; std::size_t n=0;
    while(std::getline(lines,line)){
        ++n; auto t=split(line); if(t.empty()||t[0].rfind("#",0)==0) continue;
        auto num=[&](std::size_t i,float def=0.f){ return i<t.size()?std::strtof(t[i].c_str(),nullptr):def; };
        if(t[0]=="move" && t.size()>=3){ e->transform.position.x+=num(1); e->transform.position.y+=num(2); }
        else if(t[0]=="setpos" && t.size()>=3){ e->transform.position.x=num(1); e->transform.position.y=num(2); }
        else if(t[0]=="rotate" && t.size()>=2){ e->transform.rotation+=num(1); }
        else if(t[0]=="scale" && t.size()>=3){ e->transform.scale.x=num(1,1); e->transform.scale.y=num(2,1); }
        else if(t[0]=="visible" && t.size()>=2){ e->visible=(t[1]!="0"&&t[1]!="false"); e->sprite.visible=e->visible; }
        else if(t[0]=="color" && t.size()>=4){ e->sprite.color.r=num(1); e->sprite.color.g=num(2); e->sprite.color.b=num(3); if(t.size()>=5)e->sprite.color.a=num(4,1); }
        else if(t[0]=="name" && t.size()>=2){ e->name=line.substr(line.find(' ')+1); }
        else return {false,"line_"+std::to_string(n)+"_unknown_command"};
    }
    return {true,"ok"};
}
}
