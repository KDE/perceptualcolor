cmake_minimum_required(VERSION 3.16)

project(example)

find_package(
    ECM ${KF5_MIN_VERSION}
    CONFIG
    REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)

find_package(
    "perceptualcolor-0"
    REQUIRED)

add_executable(example)
target_sources(example PRIVATE example.cpp)
target_link_libraries(
    example
    PRIVATE PerceptualColor::perceptualcolor-0)
