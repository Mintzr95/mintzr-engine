cmake_minimum_required(VERSION 3.22)
project(MJUEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# MJU owns its runtime API. Optional third-party backends are opt-in unless the
# corresponding subsystem is actually used by the target.
option(MJU_USE_SDL "Use SDL3 for optional desktop platform/window/input plumbing" OFF)
option(MJU_USE_BOX2D "Use Box2D as an optional physics backend" OFF)
option(MJU_USE_ENTT "Use EnTT as an optional ECS backend" OFF)
option(MJU_USE_RAYLIB_TOOLS "Download raylib for optional 2D reference tooling" OFF)
option(MJU_USE_IMGUI_EDITOR "Download Dear ImGui for the desktop editor" ON)
option(MJU_USE_MINIAUDIO "Use miniaudio for the audio backend" ON)
option(MJU_USE_JSON "Use nlohmann/json for project and scene serialization" ON)
option(MJU_USE_STB_IMAGE "Use stb_image for texture decoding" ON)
option(MJU_USE_TMXLITE "Use tmxlite for Tiled TMX import" ON)

if(MJU_USE_SDL)
    FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-3.2.28 GIT_SHALLOW TRUE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)
endif()
if(MJU_USE_BOX2D)
    FetchContent_Declare(box2d GIT_REPOSITORY https://github.com/erincatto/box2d.git GIT_TAG v3.1.1 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(box2d)
endif()
if(MJU_USE_ENTT)
    FetchContent_Declare(entt GIT_REPOSITORY https://github.com/skypjack/entt.git GIT_TAG v3.16.0 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(entt)
endif()
if(MJU_USE_RAYLIB_TOOLS)
    FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG 5.5 GIT_SHALLOW TRUE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()
if(MJU_USE_IMGUI_EDITOR AND NOT ANDROID)
    FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.92.9 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(imgui)
    add_library(mju_imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp ${imgui_SOURCE_DIR}/imgui_demo.cpp ${imgui_SOURCE_DIR}/imgui_draw.cpp ${imgui_SOURCE_DIR}/imgui_tables.cpp ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
    target_include_directories(mju_imgui PUBLIC ${imgui_SOURCE_DIR})
endif()
if(MJU_USE_MINIAUDIO)
    FetchContent_Declare(miniaudio GIT_REPOSITORY https://github.com/mackron/miniaudio.git GIT_TAG 0.11.25 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(miniaudio)
    add_library(mju_miniaudio STATIC third_party/miniaudio_wrapper.c)
    target_include_directories(mju_miniaudio PRIVATE ${miniaudio_SOURCE_DIR})
    target_compile_features(mju_miniaudio PUBLIC c_std_99)
endif()
if(MJU_USE_JSON)
    FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.12.0 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(nlohmann_json)
endif()
if(MJU_USE_STB_IMAGE)
    FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG f0569113c93ad095470c54bf34a17b36646bbbb5 GIT_SHALLOW FALSE)
    FetchContent_MakeAvailable(stb)
endif()
if(MJU_USE_TMXLITE)
    FetchContent_Declare(tmxlite GIT_REPOSITORY https://github.com/fallahn/tmxlite.git GIT_TAG 1.2.2 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(tmxlite)
    add_subdirectory(${tmxlite_SOURCE_DIR}/tmxlite ${tmxlite_BINARY_DIR})
endif()

add_library(mju_core STATIC
    mju/core/application.cpp mju/core/engine.cpp mju/core/scene.cpp
    mju/io/scene_io.cpp mju/io/scene_json.cpp mju/editor/editor.cpp
    mju/assets/asset_manager.cpp mju/assets/image_loader.cpp mju/assets/texture_cache.cpp
    mju/tilemap/tilemap.cpp mju/tilemap/tmx_importer.cpp mju/ui/ui.cpp
    mju/physics/physics.cpp mju/particles/particles.cpp mju/resources/resources.cpp
    mju/scripting/script.cpp mju/diagnostics/debug.cpp mju/project/project.cpp
    mju/render/sprite_batch.cpp mju/performance/mobile_profile.cpp)

target_include_directories(mju_core PUBLIC mju)
target_compile_features(mju_core PUBLIC cxx_std_20)
target_compile_definitions(mju_core PUBLIC
    $<$<BOOL:${MJU_USE_SDL}>:MJU_HAS_SDL=1>
    $<$<BOOL:${MJU_USE_BOX2D}>:MJU_HAS_BOX2D=1>
    $<$<BOOL:${MJU_USE_ENTT}>:MJU_HAS_ENTT=1>
    $<$<BOOL:${MJU_USE_RAYLIB_TOOLS}>:MJU_HAS_RAYLIB_TOOLS=1>
    $<$<BOOL:${MJU_USE_MINIAUDIO}>:MJU_HAS_MINIAUDIO=1>
    $<$<BOOL:${MJU_USE_JSON}>:MJU_HAS_JSON=1>
    $<$<BOOL:${MJU_USE_STB_IMAGE}>:MJU_HAS_STB_IMAGE=1>
    $<$<BOOL:${MJU_USE_TMXLITE}>:MJU_HAS_TMXLITE=1>)
if(MJU_USE_SDL) target_link_libraries(mju_core PUBLIC SDL3::SDL3) endif()
if(MJU_USE_BOX2D) target_link_libraries(mju_core PUBLIC box2d) endif()
if(MJU_USE_ENTT) target_link_libraries(mju_core PUBLIC EnTT::EnTT) endif()
if(MJU_USE_IMGUI_EDITOR AND NOT ANDROID) target_link_libraries(mju_core PUBLIC mju_imgui) endif()
if(MJU_USE_MINIAUDIO) target_link_libraries(mju_core PUBLIC mju_miniaudio) endif()
if(MJU_USE_JSON) target_link_libraries(mju_core PUBLIC nlohmann_json::nlohmann_json) endif()
if(MJU_USE_STB_IMAGE) target_include_directories(mju_core PRIVATE ${stb_SOURCE_DIR}) endif()
if(MJU_USE_TMXLITE) target_link_libraries(mju_core PUBLIC tmxlite) target_include_directories(mju_core PRIVATE ${tmxlite_SOURCE_DIR}/tmxlite/include) endif()

if(ANDROID)
    add_library(mju_android_native SHARED platform/android/android_entry.cpp mju/render/renderer_gles2.cpp)
    target_include_directories(mju_android_native PRIVATE mju)
    target_link_libraries(mju_android_native PRIVATE mju_core android log GLESv2)
endif()

option(MJU_BUILD_TESTS "Build MJU core tests" ON)
if(MJU_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
