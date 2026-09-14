add_executable(mju_core_test core_test.cpp)
target_include_directories(mju_core_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mju_core_test PRIVATE mju_core)
add_test(NAME mju_core_test COMMAND mju_core_test)
