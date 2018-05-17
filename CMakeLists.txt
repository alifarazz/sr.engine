cmake_minimum_required(VERSION 3.10)
project(sre)

set(CMAKE_CXX_STANDARD 17)

add_executable(sre src/main.cpp src/objmesh.cpp src/tgaimage.cpp src/h/geometry.h src/h/tgaimage.h src/h/objmesh.h)