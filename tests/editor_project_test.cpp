#include "test_harness.h"
#include "../mju/editor/editor.h"
#include "../mju/io/scene_io.h"
#include <filesystem>
#include <fstream>

int main() {
    mju::test::Runner t;

    mju::editor::EditorState editor;
    auto* entity = editor.create("Player");
    MJU_EXPECT(t, entity != nullptr);
    if (entity) {
        entity->transform.position = {10.0f, 20.0f};
        editor.history.reset(editor.scene);
        editor.begin_transform_edit();
        MJU_EXPECT(t, editor.set_transform(entity->id, 100.0f, 200.0f));
        MJU_EXPECT(t, editor.set_transform(entity->id, 140.0f, 240.0f));
        editor.end_transform_edit();
        MJU_EXPECT(t, editor.history.undo_count() == 1);
        MJU_EXPECT(t, editor.undo());
        auto* restored = editor.scene.find(entity->id);
        MJU_EXPECT(t, restored != nullptr);
        if (restored) {
            MJU_EXPECT(t, restored->transform.position.x == 10.0f);
            MJU_EXPECT(t, restored->transform.position.y == 20.0f);
        }
    }

    const auto path = std::filesystem::temp_directory_path() / "mju_project_editor_test.mjuproject";
    mju::editor::ProjectSettings settings;
    settings.name = "Transaction Test";
    settings.main_scene = "world.mjuscene";
    settings.width = 1920;
    settings.height = 1080;
    settings.target_fps = 90;
    settings.vsync = false;
    settings.max_entities = 2048;
    MJU_EXPECT(t, mju::editor::save_project(settings, path.string()));

    mju::editor::ProjectSettings loaded;
    MJU_EXPECT(t, mju::editor::load_project(loaded, path.string()));
    MJU_EXPECT(t, loaded.name == settings.name);
    MJU_EXPECT(t, loaded.main_scene == settings.main_scene);
    MJU_EXPECT(t, loaded.width == 1920 && loaded.height == 1080);
    MJU_EXPECT(t, loaded.target_fps == 90 && !loaded.vsync);
    MJU_EXPECT(t, loaded.max_entities == 2048);

    std::ifstream project_file(path, std::ios::binary);
    std::string first_chunk(32, '\0');
    project_file.read(first_chunk.data(), static_cast<std::streamsize>(first_chunk.size()));
    MJU_EXPECT(t, first_chunk.find("\"format\": \"mju-project\"") != std::string::npos);
    project_file.close();
    std::error_code ec;
    std::filesystem::remove(path, ec);

    const auto scene_path = std::filesystem::temp_directory_path() / "mju_scene_io_test.mjuscene";
    mju::Scene scene;
    auto& player = scene.create_entity("Player");
    player.transform.position = {12.0f, 34.0f};
    MJU_EXPECT(t, mju::save_scene(scene, scene_path.string()));
    std::ifstream scene_file(scene_path, std::ios::binary);
    char first = '\0';
    scene_file.read(&first, 1);
    MJU_EXPECT(t, first == '{');
    scene_file.close();

    mju::Scene loaded_scene;
    MJU_EXPECT(t, mju::load_scene(loaded_scene, scene_path.string()));
    MJU_EXPECT(t, loaded_scene.size() == 1);
    auto* loaded_player = loaded_scene.find(player.id);
    MJU_EXPECT(t, loaded_player != nullptr);
    if (loaded_player) {
        MJU_EXPECT(t, loaded_player->transform.position.x == 12.0f);
        MJU_EXPECT(t, loaded_player->transform.position.y == 34.0f);
    }
    std::filesystem::remove(scene_path, ec);

    return t.finish();
}
