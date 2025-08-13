cmake_minimum_required(VERSION 3.10)

project(Zephyr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include_directories(include)

# Add all .cpp files from the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(zephyr ${SOURCES})
