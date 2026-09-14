cmake_minimum_required(VERSION 3.22)
project(mju_android_host LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
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
 ../../../../../mju/tilemap/tilemap.cpp
 ../../../../../mju/ui/ui.cpp
 ../../../../../mju/physics/physics.cpp
 ../../../../../mju/particles/particles.cpp
 ../../../../../mju/resources/resources.cpp
 ../../../../../mju/scripting/script.cpp
 ../../../../../mju/diagnostics/debug.cpp
 ../../../../../mju/project/project.cpp
 ../../../../../mju/render/renderer_gles2.cpp)
target_include_directories(mju_android PRIVATE ../../../../../mju ../../../../../mju/core ../../../../../mju/render)
target_link_libraries(mju_android android log GLESv2)
