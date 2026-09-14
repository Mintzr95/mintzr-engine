#include "project.h"
#include <fstream>
#include <sstream>
namespace mju::project {
bool save_settings(const Settings&s,const std::string&p){std::ofstream f(p);if(!f)return false;f<<"MJU_PROJECT 1\n"<<"name="<<s.name<<"\nwidth="<<s.width<<"\nheight="<<s.height<<"\ntarget_fps="<<s.target_fps<<"\nportrait="<<(s.portrait?1:0)<<"\n";return true;}
bool load_settings(Settings&s,const std::string&p){std::ifstream f(p);if(!f)return false;std::string l;while(std::getline(f,l)){auto eq=l.find('=');if(eq==std::string::npos)continue;auto k=l.substr(0,eq),v=l.substr(eq+1);if(k=="name")s.name=v;else if(k=="width")s.width=std::stoi(v);else if(k=="height")s.height=std::stoi(v);else if(k=="target_fps")s.target_fps=std::stoi(v);else if(k=="portrait")s.portrait=v!="0";}return true;}
}
