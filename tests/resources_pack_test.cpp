#include "mju/resources/resources.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

int main() {
    namespace fs = std::filesystem;
    const fs::path root = "mju_pack_test_dir";
    const fs::path nested = root / "nested";
    const fs::path pack = "mju_pack_test.mjupack";
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::remove(pack, ec);
    fs::create_directories(nested, ec);
    assert(!ec);

    {
        std::ofstream out(root / "hello.txt", std::ios::binary);
        out << "hello-mju";
    }
    {
        std::ofstream out(nested / "data.bin", std::ios::binary);
        const unsigned char bytes[] = {1, 2, 3, 4, 5};
        out.write(reinterpret_cast<const char*>(bytes), sizeof(bytes));
    }

    mju::resources::AssetPack asset_pack;
    assert(asset_pack.build(root.string(), pack.string()));
    assert(asset_pack.is_open());
    assert(asset_pack.version() == 1);
    assert(asset_pack.contains("hello.txt"));
    assert(asset_pack.contains("nested/data.bin"));
    assert(!asset_pack.contains("missing.txt"));

    std::vector<std::uint8_t> bytes;
    assert(asset_pack.read("hello.txt", bytes));
    const std::string hello(bytes.begin(), bytes.end());
    assert(hello == "hello-mju");

    bytes.clear();
    assert(asset_pack.read("nested/data.bin", bytes));
    assert(bytes.size() == 5);
    assert(bytes[0] == 1 && bytes[4] == 5);

    asset_pack.close();
    assert(!asset_pack.is_open());
    fs::remove_all(root, ec);
    fs::remove(pack, ec);
    return 0;
}
