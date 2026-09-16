#include "project.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace mju::project {
using json=nlohmann::json;

static json to_json(const Settings& s){
    return {{"format","mju-project"},{"version",2},{"name",s.name},{"start_scene",s.start_scene},
            {"width",s.width},{"height",s.height},{"target_fps",s.target_fps},{"portrait",s.portrait},
            {"vsync",s.vsync},{"max_entities",s.max_entities},{"renderer",s.renderer}};
}

bool save_settings(const Settings& input,const std::string& path){
    if(path.empty()) return false;
    Settings s=input;
    s.width=std::clamp(s.width,64,16384); s.height=std::clamp(s.height,64,16384);
    s.target_fps=std::clamp(s.target_fps,15,240); s.max_entities=std::clamp(s.max_entities,1,1000000);
    const std::string tmp=path+".tmp";
    std::ofstream out(tmp,std::ios::binary|std::ios::trunc); if(!out) return false;
    out<<to_json(s).dump(2)<<'\n'; out.flush();
    if(!out) return false;
    out.close();
    std::remove(path.c_str());
    return std::rename(tmp.c_str(),path.c_str())==0;
}

bool load_settings(Settings& s,const std::string& path){
    if(path.empty()) return false;
    std::ifstream in(path,std::ios::binary); if(!in) return false;
    try{
        const json root=json::parse(in);
        const std::string format=root.value("format","");
        const int version=root.value("version",1);
        if(format=="mju-project" && version>=2){
            s.name=root.value("name",s.name); s.start_scene=root.value("start_scene",s.start_scene);
            s.width=std::clamp(root.value("width",s.width),64,16384);
            s.height=std::clamp(root.value("height",s.height),64,16384);
            s.target_fps=std::clamp(root.value("target_fps",s.target_fps),15,240);
            s.portrait=root.value("portrait",s.portrait); s.vsync=root.value("vsync",s.vsync);
            s.max_entities=std::clamp(root.value("max_entities",s.max_entities),1,1000000);
            s.renderer=root.value("renderer",s.renderer); return true;
        }
        // Backward-compatible migration from the original line format.
        if(format.empty() && (version==1 || root.is_object())) return false;
    }catch(...){return false;}
    return false;
}
}
