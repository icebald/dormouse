cmake_minimum_required(VERSION 3.10)

project(dm)

add_compile_options(-std=c++11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/include/libevent)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/third/libs)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third)
