cmake_minimum_required(VERSION 2.6)
project(psyche-test-threadpool)
include_directories(${CMAKE_SOURCE_DIR})
cmake_policy(SET CMP0015 NEW)
add_executable(psyche-test-threadpool threadpool_test.cpp)
target_link_libraries(psyche-test-threadpool psyche)