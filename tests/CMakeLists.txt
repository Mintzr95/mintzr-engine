add_executable(mju_core_test core_test.cpp)
target_include_directories(mju_core_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mju_core_test PRIVATE mju_core)
add_test(NAME mju_core_test COMMAND mju_core_test)

add_executable(mju_scene_json_test scene_json_test.cpp)
target_include_directories(mju_scene_json_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mju_scene_json_test PRIVATE mju_core)
add_test(NAME mju_scene_json_test COMMAND mju_scene_json_test)
