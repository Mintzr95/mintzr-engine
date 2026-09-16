#include "script.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <unordered_map>

namespace mju::scripting {
std::vector<std::string> ScriptVM::split(const std::string&line){std::istringstream ss(line);std::vector<std::string>out;std::string s;while(ss>>s)out.push_back(s);return out;}
ScriptResult ScriptVM::execute(Scene&scene,EntityId self,const std::string&source){
 Entity*e=scene.find(self);if(!e)return{false,"entity_not_found"};std::unordered_map<std::string,float>vars;std::size_t operations=0;constexpr std::size_t max_operations=4096;constexpr int max_depth=8;
 std::function<bool(const std::vector<std::string>&,int)>run;
 auto value=[&](const std::string&s,float def=0.0f){auto it=vars.find(s);if(it!=vars.end())return it->second;char*end=nullptr;const float v=std::strtof(s.c_str(),&end);return end&&*end=='\0'?v:def;};
 auto truth=[&](const std::string&op,float a,float b){if(op=="==")return a==b;if(op=="!=")return a!=b;if(op=="<")return a<b;if(op=="<=")return a<=b;if(op==">")return a>b;if(op==">=")return a>=b;return false;};
 run=[&](const std::vector<std::string>&t,int depth)->bool{
  if(++operations>max_operations||depth>max_depth)return false;if(t.empty())return true;const auto&cmd=t[0];auto num=[&](std::size_t i,float def=0.0f){return i<t.size()?value(t[i],def):def;};
  if(cmd=="set"&&t.size()>=3){vars[t[1]]=num(2);return true;}if(cmd=="add"&&t.size()>=3){vars[t[1]]+=num(2);return true;}if(cmd=="sub"&&t.size()>=3){vars[t[1]]-=num(2);return true;}if(cmd=="mul"&&t.size()>=3){vars[t[1]]*=num(2);return true;}if(cmd=="div"&&t.size()>=3){const float d=num(2);if(std::fabs(d)<1e-7f)return false;vars[t[1]]/=d;return true;}
  if(cmd=="if"&&t.size()>=5){if(!truth(t[2],num(1),num(3)))return true;return run(std::vector<std::string>(t.begin()+4,t.end()),depth+1);}if(cmd=="repeat"&&t.size()>=3){const int count=std::clamp((int)num(1),0,256);std::vector<std::string>tail(t.begin()+2,t.end());for(int i=0;i<count;++i)if(!run(tail,depth+1))return false;return true;}
  if(cmd=="move"&&t.size()>=3){e->transform.position.x+=num(1);e->transform.position.y+=num(2);return true;}if(cmd=="setpos"&&t.size()>=3){e->transform.position={num(1),num(2)};return true;}if(cmd=="rotate"&&t.size()>=2){e->transform.rotation+=num(1);return true;}if(cmd=="scale"&&t.size()>=3){e->transform.scale={num(1,1),num(2,1)};return true;}if(cmd=="visible"&&t.size()>=2){e->visible=num(1)!=0.0f;e->sprite.visible=e->visible;return true;}if(cmd=="color"&&t.size()>=4){e->sprite.color.r=num(1);e->sprite.color.g=num(2);e->sprite.color.b=num(3);if(t.size()>=5)e->sprite.color.a=num(4,1);return true;}if(cmd=="name"&&t.size()>=2){std::string joined=t[1];for(std::size_t i=2;i<t.size();++i)joined+=' '+t[i];e->name=joined;return true;}return false;
 };
 std::istringstream lines(source);std::string line;std::size_t n=0;while(std::getline(lines,line)){++n;auto t=split(line);if(t.empty()||t[0].rfind("#",0)==0)continue;if(!run(t,0)){if(operations>max_operations)return{false,"execution_limit"};return{false,"line_"+std::to_string(n)+"_invalid_or_unsafe"};}}
 return{true,"ok"};
}
}
