cmake_minimum_required(VERSION 3.16)
project(lab1-imagecraft CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(imagecraft
    src/main.cpp
    src/bmp.cpp
    src/filter_factory.cpp

    src/filters/blur.cpp
    src/filters/crop.cpp
    src/filters/edge.cpp
    src/filters/gamma.cpp
    src/filters/gs.cpp
    src/filters/hist_eq.cpp
    src/filters/med.cpp
    src/filters/neg.cpp
    src/filters/sharp.cpp
)

target_include_directories(imagecraft PRIVATE include)