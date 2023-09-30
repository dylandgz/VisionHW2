cmake_minimum_required(VERSION 3.26)
project(VisionHW2)

set(CMAKE_CXX_STANDARD 14)

add_executable(VisionHW2 main.cpp
        image.cc
        DisjSets.cc
        p1.cpp
        p2.cpp
        p3.cpp
        p4.cpp
)
