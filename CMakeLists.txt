cmake_minimum_required(VERSION 3.16)
project(image_craft LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(imagecraft
    src/main.cpp
    src/bmp.cpp
    src/image.cpp
    src/filter_factory.cpp
    src/filters/crop.cpp
    src/filters/gs.cpp
    src/filters/neg.cpp
    src/filters/sharp.cpp
    src/filters/edge.cpp
    src/filters/blur.cpp
    src/filters/med.cpp
    src/filters/gamma.cpp
    src/filters/hist_eq.cpp
)

target_include_directories(imagecraft PRIVATE include)