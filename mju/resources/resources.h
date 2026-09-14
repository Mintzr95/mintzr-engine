#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
namespace mju::resources {

enum class Type { Unknown, Texture, Audio, Font, Scene, Script, Data };
struct Texture { std::string path; int width=0,height=0; bool hasAlpha=true; };
struct AudioClip { std::string path; bool streaming=false; float volume=1; };

class ResourceCatalog {
public:
    void clear();
    bool register_resource(std::string path,Type type);
    Type type_of(const std::string& path) const;
    std::vector<std::string> list(Type type) const;
    bool import_directory(const std::string& root);
private:
    static Type classify(const std::filesystem::path& p);
    std::unordered_map<std::string,Type> items_;
};
}
