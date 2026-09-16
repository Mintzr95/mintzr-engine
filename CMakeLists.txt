cmake_minimum_required(VERSION 3.22)
project(MJUEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(FetchContent)

option(MJU_USE_IMGUI_EDITOR "Build the optional desktop editor UI" ON)

FetchContent_Declare(miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(miniaudio)
add_library(mju_miniaudio STATIC third_party/miniaudio_wrapper.c)
target_include_directories(mju_miniaudio PRIVATE ${miniaudio_SOURCE_DIR})
target_compile_features(mju_miniaudio PUBLIC c_std_99)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG f0569113c93ad095470c54bf34a17b36646bbbb5
    GIT_SHALLOW FALSE)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(tmxlite
    GIT_REPOSITORY https://github.com/fallahn/tmxlite.git
    GIT_TAG 1.2.2
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(tmxlite)
add_subdirectory(${tmxlite_SOURCE_DIR}/tmxlite ${tmxlite_BINARY_DIR})

if(MJU_USE_IMGUI_EDITOR AND NOT ANDROID)
    FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.9
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(imgui)
    add_library(mju_imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
    target_include_directories(mju_imgui PUBLIC ${imgui_SOURCE_DIR})
endif()

add_library(mju_core STATIC
    mju/core/application.cpp
    mju/core/engine.cpp
    mju/core/scene.cpp
    mju/core/scene_manager.cpp
    mju/io/scene_io.cpp
    mju/io/scene_json.cpp
    mju/editor/editor.cpp
    mju/assets/asset_manager.cpp
    mju/assets/image_loader.cpp
    mju/assets/texture_cache.cpp
    mju/audio/audio.cpp
    mju/tilemap/tilemap.cpp
    mju/tilemap/tmx_importer.cpp
    mju/ui/ui.cpp
    mju/physics/physics.cpp
    mju/particles/particles.cpp
    mju/resources/resources.cpp
    mju/scripting/script.cpp
    mju/diagnostics/debug.cpp
    mju/project/project.cpp
    mju/render/sprite_batch.cpp
    mju/performance/mobile_profile.cpp)

target_include_directories(mju_core PUBLIC mju)
target_compile_features(mju_core PUBLIC cxx_std_20)
target_compile_definitions(mju_core PUBLIC
    MJU_HAS_MINIAUDIO=1
    MJU_HAS_JSON=1
    MJU_HAS_STB_IMAGE=1
    MJU_HAS_TMXLITE=1)

target_link_libraries(mju_core PUBLIC
    mju_miniaudio
    nlohmann_json::nlohmann_json
    tmxlite)
target_include_directories(mju_core PRIVATE ${stb_SOURCE_DIR} ${tmxlite_SOURCE_DIR}/tmxlite/include ${miniaudio_SOURCE_DIR})

if(MJU_USE_IMGUI_EDITOR AND NOT ANDROID)
    target_link_libraries(mju_core PUBLIC mju_imgui)
endif()

if(ANDROID)
    add_library(mju_android_native SHARED
        platform/android/android_entry.cpp
        mju/render/renderer_gles2.cpp)
    target_include_directories(mju_android_native PRIVATE mju)
    target_link_libraries(mju_android_native PRIVATE mju_core android log GLESv2)
endif()

option(MJU_BUILD_TESTS "Build MJU core tests" ON)
if(MJU_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
