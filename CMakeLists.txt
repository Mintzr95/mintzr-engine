cmake_minimum_required(VERSION 3.22)
project(MJUEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# MJU is its own engine architecture. External projects are used only as
# replaceable low-level building blocks; editor, scene, resource, UI and
# runtime architecture remain MJU-owned.
option(MJU_USE_SDL "Use SDL3 for platform/window/input plumbing" ON)
option(MJU_USE_BOX2D "Use Box2D as the optional 2D physics backend" ON)
option(MJU_USE_ENTT "Use EnTT for the optional ECS backend" ON)

if(MJU_USE_SDL)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.28
        GIT_SHALLOW TRUE
    )
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)
endif()

if(MJU_USE_BOX2D)
    FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.1.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(box2d)
endif()

if(MJU_USE_ENTT)
    FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(entt)
endif()

add_library(mju_core STATIC
    mju/core/application.cpp
    mju/core/engine.cpp
    mju/core/scene.cpp
    mju/io/scene_io.cpp
    mju/editor/editor.cpp
    mju/assets/asset_manager.cpp
    mju/tilemap/tilemap.cpp
    mju/ui/ui.cpp
    mju/physics/physics.cpp
    mju/particles/particles.cpp
    mju/resources/resources.cpp
    mju/scripting/script.cpp
    mju/diagnostics/debug.cpp
    mju/project/project.cpp
)

target_include_directories(mju_core PUBLIC mju)
target_compile_features(mju_core PUBLIC cxx_std_20)

target_compile_definitions(mju_core PUBLIC
    $<$<BOOL:${MJU_USE_SDL}>:MJU_HAS_SDL=1>
    $<$<BOOL:${MJU_USE_BOX2D}>:MJU_HAS_BOX2D=1>
    $<$<BOOL:${MJU_USE_ENTT}>:MJU_HAS_ENTT=1>
)

if(MJU_USE_SDL)
    target_link_libraries(mju_core PUBLIC SDL3::SDL3)
endif()

if(MJU_USE_BOX2D)
    target_link_libraries(mju_core PUBLIC box2d)
endif()

if(MJU_USE_ENTT)
    target_link_libraries(mju_core PUBLIC EnTT::EnTT)
endif()

if(ANDROID)
    add_library(mju_android_native SHARED
        platform/android/android_entry.cpp
        mju/render/renderer_gles2.cpp
    )
    target_include_directories(mju_android_native PRIVATE mju)
    target_link_libraries(mju_android_native PRIVATE mju_core android log GLESv2)
endif()

option(MJU_BUILD_TESTS "Build MJU core tests" ON)
if(MJU_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
