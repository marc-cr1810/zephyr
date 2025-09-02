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

# Generate a version string from the current timestamp
string(TIMESTAMP BUILD_TIMESTAMP "%Y.%m.%d-%H%M%S" UTC)
set(APP_VERSION "v${BUILD_TIMESTAMP}")
target_compile_definitions(zephyr PRIVATE ZEPHYR_VERSION="${APP_VERSION}")
target_compile_definitions(zephyr PRIVATE ZEPHYR_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

add_custom_command(TARGET zephyr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:zephyr>
    ${CMAKE_SOURCE_DIR}/bin/zephyr
    COMMENT "Copying zephyr executable to project bin directory"
)
