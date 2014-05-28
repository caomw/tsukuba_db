cmake_minimum_required(VERSION 2.6)
project(parser)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# C++11
# add_definitions(-std=c++11)

# Tsukuba DB Library
add_subdirectory(tsukuba)

# Target
add_executable(parser main.cpp)

# Link
target_link_libraries(parser tsukuba_db ${OpenCV_LIBS})
