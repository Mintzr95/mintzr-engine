#include "audio.h"
#include <algorithm>
#include <memory>
#include <miniaudio.h>
#include <unordered_map>

namespace mju {
struct AudioSystem::Impl {
    ma_engine engine{};
    bool engine_ready=false;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> sounds;

    void clear_sounds(){
        for(auto& [id,sound]:sounds){
            if(sound){ ma_sound_stop(sound.get()); ma_sound_uninit(sound.get()); }
        }
        sounds.clear();
    }
};

AudioSystem::~AudioSystem(){ shutdown(); }

bool AudioSystem::initialize(){
    if(initialized_) return true;
    if(!impl_) impl_=new Impl();
    if(ma_engine_init(nullptr,&impl_->engine)!=MA_SUCCESS){
        delete impl_; impl_=nullptr; return false;
    }
    impl_->engine_ready=true;
    initialized_=true;
    ma_engine_set_volume(&impl_->engine,master_.volume);
    return true;
}

void AudioSystem::shutdown(){
    if(!impl_) { initialized_=false; return; }
    impl_->clear_sounds();
    if(impl_->engine_ready){ ma_engine_uninit(&impl_->engine); impl_->engine_ready=false; }
    delete impl_; impl_=nullptr;
    clips_.clear();
    last_sfx_.clear();
    initialized_=false;
}

void AudioSystem::update(float){
    if(!initialized_||!impl_) return;
    for(auto& [id,state]:clips_){
        auto it=impl_->sounds.find(id);
        if(it!=impl_->sounds.end() && it->second) state.playing=ma_sound_is_playing(it->second.get())!=MA_FALSE;
    }
}

void AudioSystem::set_master_volume(float v){
    master_.volume=std::clamp(v,0.0f,1.0f);
    if(initialized_&&impl_) ma_engine_set_volume(&impl_->engine,master_.mute?0.0f:master_.volume);
}

bool AudioSystem::load(const std::string& id,const std::string& path){
    if(id.empty()||path.empty()||!initialized_||!impl_) return false;
    unload(id);
    auto sound=std::make_unique<ma_sound>();
    if(ma_sound_init_from_file(&impl_->engine,path.c_str(),0,nullptr,nullptr,sound.get())!=MA_SUCCESS) return false;
    ma_sound_set_volume(sound.get(),1.0f);
    impl_->sounds.emplace(id,std::move(sound));
    clips_[id]={path,1.0f,false,false};
    return true;
}

bool AudioSystem::unload(const std::string& id){
    if(!impl_) return false;
    auto it=impl_->sounds.find(id); if(it==impl_->sounds.end()) return false;
    if(it->second){ ma_sound_stop(it->second.get()); ma_sound_uninit(it->second.get()); }
    impl_->sounds.erase(it); clips_.erase(id); return true;
}

bool AudioSystem::play(const std::string& id,bool loop){
    if(!initialized_||!impl_) return false;
    auto it=impl_->sounds.find(id); if(it==impl_->sounds.end()) return false;
    auto cit=clips_.find(id); if(cit==clips_.end()) return false;
    ma_sound_seek_to_pcm_frame(it->second.get(),0);
    ma_sound_set_looping(it->second.get(),loop?MA_TRUE:MA_FALSE);
    ma_sound_set_volume(it->second.get(),cit->second.volume);
    if(ma_sound_start(it->second.get())!=MA_SUCCESS) return false;
    cit->second.loop=loop; cit->second.playing=true; return true;
}

void AudioSystem::stop(const std::string& id){
    if(!impl_) return;
    auto it=impl_->sounds.find(id); if(it!=impl_->sounds.end() && it->second) ma_sound_stop(it->second.get());
    auto cit=clips_.find(id); if(cit!=clips_.end()) cit->second.playing=false;
}

bool AudioSystem::playing(const std::string& id) const{
    if(!impl_) return false;
    auto it=impl_->sounds.find(id); return it!=impl_->sounds.end() && it->second && ma_sound_is_playing(it->second.get())!=MA_FALSE;
}

bool AudioSystem::set_volume(const std::string& id,float v){
    auto cit=clips_.find(id); if(cit==clips_.end()) return false;
    cit->second.volume=std::clamp(v,0.0f,1.0f);
    if(impl_){ auto it=impl_->sounds.find(id); if(it!=impl_->sounds.end()&&it->second) ma_sound_set_volume(it->second.get(),cit->second.volume); }
    return true;
}

void AudioSystem::play_sfx(const char* path){
    if(!path||!*path||!initialized_||!impl_) return;
    last_sfx_=path;
    (void)ma_engine_play_sound(&impl_->engine,path,nullptr);
}
}
