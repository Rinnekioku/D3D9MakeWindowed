cmake_minimum_required(VERSION 3.8)
project(D3D9MakeWindowed)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rdparty)

add_library(D3D9MakeWindowed SHARED MakeWindowed.cpp)
target_link_libraries(D3D9MakeWindowed PRIVATE lib_detours)
