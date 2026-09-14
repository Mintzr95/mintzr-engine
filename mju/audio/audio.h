#pragma once
#include <string>
#include <unordered_map>
namespace mju {
struct AudioBus { float volume=1.0f; bool mute=false; };
struct AudioClipState { std::string path; float volume=1; bool loop=false; bool playing=false; };
class AudioSystem { public: bool initialize(){initialized_=true;return true;} void shutdown(){clips_.clear();initialized_=false;} void update(float){} bool initialized()const{return initialized_;} void set_master_volume(float v){master_.volume=v<0?0:v>1?1:v;} float master_volume()const{return master_.volume;} bool load(const std::string& id,const std::string& path){if(id.empty()||path.empty())return false;clips_[id]={path,1,false,false};return true;} bool play(const std::string&id,bool loop=false){auto it=clips_.find(id);if(it==clips_.end())return false;it->second.loop=loop;it->second.playing=true;return true;} void stop(const std::string&id){auto it=clips_.find(id);if(it!=clips_.end())it->second.playing=false;} bool playing(const std::string&id)const{auto it=clips_.find(id);return it!=clips_.end()&&it->second.playing;} void play_sfx(const char* path){if(path)last_sfx_=path;} const std::string& last_sfx()const{return last_sfx_;} private: bool initialized_=false; AudioBus master_{}; std::unordered_map<std::string,AudioClipState> clips_; std::string last_sfx_;}; }
