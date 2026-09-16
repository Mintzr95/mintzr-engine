#pragma once
#include <string>
#include <unordered_map>

namespace mju {
struct AudioBus { float volume=1.0f; bool mute=false; };
struct AudioClipState { std::string path; float volume=1.0f; bool loop=false; bool playing=false; };

class AudioSystem {
public:
    AudioSystem() = default;
    ~AudioSystem();
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;
    bool initialize();
    void shutdown();
    void update(float dt);
    bool initialized() const { return initialized_; }
    void set_master_volume(float v);
    float master_volume() const { return master_.volume; }
    bool load(const std::string& id,const std::string& path);
    bool unload(const std::string& id);
    bool play(const std::string& id,bool loop=false);
    void stop(const std::string& id);
    bool playing(const std::string& id) const;
    bool set_volume(const std::string& id,float v);
    void play_sfx(const char* path);
    const std::string& last_sfx() const { return last_sfx_; }
private:
    struct Impl;
    Impl* impl_=nullptr;
    bool initialized_=false;
    AudioBus master_{};
    std::unordered_map<std::string,AudioClipState> clips_;
    std::string last_sfx_;
};
}
