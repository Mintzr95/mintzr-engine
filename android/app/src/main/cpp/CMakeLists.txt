cmake_minimum_required(VERSION 3.22)
project(mju_android_host LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
include(FetchContent)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG f0569113c93ad095470c54bf34a17b36646bbbb5 GIT_SHALLOW TRUE)
FetchContent_Populate(stb)
add_library(mju_android SHARED
 jni_bridge.cpp
 ../../../../../platform/android/android_entry.cpp
 ../../../../../mju/core/application.cpp
 ../../../../../mju/core/engine.cpp
 ../../../../../mju/core/scene.cpp
 ../../../../../mju/core/math.cpp
 ../../../../../mju/io/scene_io.cpp
 ../../../../../mju/editor/editor.cpp
 ../../../../../mju/assets/asset_manager.cpp
 ../../../../../mju/assets/image_loader.cpp
 ../../../../../mju/assets/texture_cache.cpp
 ../../../../../mju/tilemap/tilemap.cpp
 ../../../../../mju/ui/ui.cpp
 ../../../../../mju/physics/physics.cpp
 ../../../../../mju/particles/particles.cpp
 ../../../../../mju/resources/resources.cpp
 ../../../../../mju/scripting/script.cpp
 ../../../../../mju/diagnostics/debug.cpp
 ../../../../../mju/project/project.cpp
 ../../../../../mju/render/sprite_batch.cpp
 ../../../../../mju/render/renderer_gles2.cpp)
target_compile_definitions(mju_android PRIVATE MJU_HAS_STB_IMAGE=1)
target_include_directories(mju_android PRIVATE ../../../../../mju ../../../../../mju/core ../../../../../mju/render ${stb_SOURCE_DIR})
target_link_libraries(mju_android android log GLESv2)
