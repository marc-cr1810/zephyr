cmake_minimum_required(VERSION 3.10)

# Get current date for version
string(TIMESTAMP CURRENT_YEAR "%Y" UTC)
string(TIMESTAMP CURRENT_MONTH "%m" UTC)
string(TIMESTAMP CURRENT_DAY "%d" UTC)

# Remove leading zeros from month and day
math(EXPR VERSION_MONTH "${CURRENT_MONTH}")
math(EXPR VERSION_DAY "${CURRENT_DAY}")

project(Zephyr VERSION ${CURRENT_YEAR}.${VERSION_MONTH}.${VERSION_DAY})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(include)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Find required packages
find_package(Threads REQUIRED)

# =============================================================================
# Core Zephyr Library
# =============================================================================

# Collect core source files (excluding main.cpp for library)
file(GLOB_RECURSE CORE_SOURCES
    "src/*.cpp"
)
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Create shared library
add_library(zephyr_shared SHARED ${CORE_SOURCES})
set_target_properties(zephyr_shared PROPERTIES
    OUTPUT_NAME "zephyr"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

# Link libraries for shared library
target_link_libraries(zephyr_shared
    Threads::Threads
    ${CMAKE_DL_LIBS}  # For dynamic loading (dlopen/dlsym)
)

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(zephyr_shared m)  # Math library
elseif(WIN32)
    target_link_libraries(zephyr_shared kernel32 user32)
endif()

# Create static library (optional)
add_library(zephyr_static STATIC ${CORE_SOURCES})
set_target_properties(zephyr_static PROPERTIES
    OUTPUT_NAME "zephyr_static"
    POSITION_INDEPENDENT_CODE ON
)
target_link_libraries(zephyr_static
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

# =============================================================================
# Zephyr Executable
# =============================================================================

# Main executable
add_executable(zephyr src/main.cpp)
target_link_libraries(zephyr zephyr_shared)
# Export symbols for plugin linking
target_link_options(zephyr PRIVATE -rdynamic)

# Generate version info
string(TIMESTAMP VERSION_MONTH_PADDED "%m" UTC)
string(TIMESTAMP VERSION_DAY_PADDED "%d" UTC)
set(ZEPHYR_VERSION_STRING "v${CURRENT_YEAR}.${VERSION_MONTH_PADDED}.${VERSION_DAY_PADDED}")

# Add version definitions to all targets that need them
target_compile_definitions(zephyr PRIVATE
    ZEPHYR_VERSION="${ZEPHYR_VERSION_STRING}"
    ZEPHYR_VERSION_STRING="${ZEPHYR_VERSION_STRING}"
    ZEPHYR_VERSION_MAJOR=${CURRENT_YEAR}
    ZEPHYR_VERSION_MINOR=${VERSION_MONTH}
    ZEPHYR_VERSION_PATCH=${VERSION_DAY}
    ZEPHYR_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
)

# Add version definitions to shared library
target_compile_definitions(zephyr_shared PRIVATE
    ZEPHYR_VERSION="${ZEPHYR_VERSION_STRING}"
    ZEPHYR_VERSION_STRING="${ZEPHYR_VERSION_STRING}"
    ZEPHYR_VERSION_MAJOR=${CURRENT_YEAR}
    ZEPHYR_VERSION_MINOR=${VERSION_MONTH}
    ZEPHYR_VERSION_PATCH=${VERSION_DAY}
    ZEPHYR_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
)

# Add version definitions to static library
target_compile_definitions(zephyr_static PRIVATE
    ZEPHYR_VERSION="${ZEPHYR_VERSION_STRING}"
    ZEPHYR_VERSION_STRING="${ZEPHYR_VERSION_STRING}"
    ZEPHYR_VERSION_MAJOR=${CURRENT_YEAR}
    ZEPHYR_VERSION_MINOR=${VERSION_MONTH}
    ZEPHYR_VERSION_PATCH=${VERSION_DAY}
    ZEPHYR_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
)

# Copy executable to bin directory
add_custom_command(TARGET zephyr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zephyr> ${CMAKE_SOURCE_DIR}/bin/
    COMMENT "Copying zephyr executable to bin directory"
)

# =============================================================================
# Plugin API Headers (for development)
# =============================================================================

# Install headers for plugin development
install(DIRECTORY include/zephyr/
    DESTINATION include/zephyr
    FILES_MATCHING PATTERN "*.hpp"
)

# =============================================================================
# Plugin Examples
# =============================================================================

option(BUILD_PLUGIN_EXAMPLES "Build plugin examples" OFF)

if(BUILD_PLUGIN_EXAMPLES)
    # Math plugin example
    add_subdirectory(examples/plugins)

    # Class export plugin example
    add_library(class_export_plugin SHARED examples/class_export_plugin.cpp)
    target_include_directories(class_export_plugin PRIVATE include)
    target_link_libraries(class_export_plugin zephyr_shared)

    # Set plugin properties
    set_target_properties(class_export_plugin PROPERTIES
        PREFIX ""
        OUTPUT_NAME "class_export_plugin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Improved class export plugin example with full member access
    add_library(class_export_plugin_improved SHARED
        examples/class_export_plugin_improved.cpp
        src/api/cpp_object_wrapper.cpp)
    target_include_directories(class_export_plugin_improved PRIVATE include)
    target_link_libraries(class_export_plugin_improved zephyr_shared)

    # Set plugin properties for improved plugin
    set_target_properties(class_export_plugin_improved PROPERTIES
        PREFIX ""
        OUTPUT_NAME "class_export_plugin_improved"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Working class export plugin with full functionality
    add_library(working_class_export SHARED examples/working_class_export.cpp)
    target_include_directories(working_class_export PRIVATE include)
    target_link_libraries(working_class_export zephyr_shared)

    # Set plugin properties for working plugin
    set_target_properties(working_class_export PROPERTIES
        PREFIX ""
        OUTPUT_NAME "working_class_export"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )

    # Proper class export plugin using native_class_t
    add_library(proper_class_export SHARED examples/proper_class_export_plugin.cpp)
    target_include_directories(proper_class_export PRIVATE include)
    target_link_libraries(proper_class_export zephyr_shared)

    # Set plugin properties for proper plugin
    set_target_properties(proper_class_export PROPERTIES
        PREFIX ""
        OUTPUT_NAME "proper_class_export"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )
endif()

# =============================================================================
# Embedding Example
# =============================================================================

option(BUILD_EMBEDDING_EXAMPLE "Build embedding example" OFF)

if(BUILD_EMBEDDING_EXAMPLE)
    # Note: embedding_example.cpp disabled due to API compatibility issues
    # add_executable(embedding_example examples/embedding_example.cpp)
    # target_link_libraries(embedding_example zephyr_shared)

    # Note: embedded_native_classes.cpp disabled due to API compatibility issues
    # add_executable(embedded_native_classes examples/embedded_native_classes.cpp)
    # target_link_libraries(embedded_native_classes zephyr_shared)

    # Comprehensive embedded example
    add_executable(comprehensive_embedded_example examples/comprehensive_embedded_example.cpp)
    target_link_libraries(comprehensive_embedded_example zephyr_shared)

    # Copy comprehensive embedded example to bin directory
    add_custom_command(TARGET comprehensive_embedded_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:comprehensive_embedded_example> ${CMAKE_SOURCE_DIR}/bin/
        COMMENT "Copying comprehensive embedded example to bin directory"
    )

    # Minimal embedded example (fully working)
    add_executable(minimal_embedded_example examples/minimal_embedded_example.cpp)
    target_link_libraries(minimal_embedded_example zephyr_shared)

    # Copy minimal embedded example to bin directory
    add_custom_command(TARGET minimal_embedded_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:minimal_embedded_example> ${CMAKE_SOURCE_DIR}/bin/
        COMMENT "Copying minimal embedded example to bin directory"
    )
endif()

# =============================================================================
# Testing
# =============================================================================

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()

    # Plugin API tests
    add_executable(test_plugin_api tests/test_plugin_api.cpp)
    target_link_libraries(test_plugin_api zephyr_shared)
    add_test(NAME plugin_api_test COMMAND test_plugin_api)
endif()

# =============================================================================
# Installation
# =============================================================================

# Install libraries
install(TARGETS zephyr_shared zephyr_static zephyr
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/zephyr
)

# Install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/zephyr.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/zephyr.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/zephyr.pc
    DESTINATION lib/pkgconfig
)

# Install plugin development files
install(FILES
    examples/plugins/CMakeLists.txt
    DESTINATION share/zephyr/examples
    RENAME plugin_template_CMakeLists.txt
)

# Install documentation
install(FILES
    README.md
    PLUGIN_API_README.md
    DESTINATION share/doc/zephyr
)

# =============================================================================
# Development Tools
# =============================================================================

# Create plugin directories in build
add_custom_target(create_plugin_dirs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/plugins
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/plugins
    COMMENT "Creating plugin directories"
)

# =============================================================================
# Summary
# =============================================================================

message(STATUS "")
message(STATUS "Zephyr Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Components:")
message(STATUS "  Core Library: ON")
message(STATUS "  Plugin API: ON")
message(STATUS "  Plugin Examples: ${BUILD_PLUGIN_EXAMPLES}")
message(STATUS "  Embedding Example: ${BUILD_EMBEDDING_EXAMPLE}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "")
message(STATUS "Plugin Development:")
message(STATUS "  Headers will be installed to: ${CMAKE_INSTALL_PREFIX}/include/zephyr")
message(STATUS "  Libraries will be installed to: ${CMAKE_INSTALL_PREFIX}/lib")
message(STATUS "  Plugin template: ${CMAKE_INSTALL_PREFIX}/share/zephyr/examples")
message(STATUS "")

# Create a simple plugin development script
file(WRITE ${CMAKE_BINARY_DIR}/create_plugin.sh
"#!/bin/bash
# Zephyr Plugin Development Helper Script
set -e

PLUGIN_NAME=\"\$1\"
if [ -z \"\$PLUGIN_NAME\" ]; then
    echo \"Usage: \$0 <plugin_name>\"
    exit 1
fi

echo \"Creating plugin: \$PLUGIN_NAME\"
mkdir -p \$PLUGIN_NAME
cd \$PLUGIN_NAME

# Create basic plugin structure
cat > \$PLUGIN_NAME.cpp << 'EOF'
#include <zephyr/api/plugin_interface.hpp>
#include <zephyr/api/native_module.hpp>

using namespace zephyr::api;

class my_plugin_t : public plugin_interface_t {
public:
    auto get_plugin_info() const -> plugin_info_t override {
        return {
            .name = \"${PLUGIN_NAME}\",
            .description = \"Example plugin\",
            .author = \"Your Name\",
            .version = {${CURRENT_YEAR}, ${VERSION_MONTH}, ${VERSION_DAY}},
            .min_zephyr_version = {${CURRENT_YEAR}, ${VERSION_MONTH}, ${VERSION_DAY}}
        };
    }

    auto initialize(engine_t* engine) -> plugin_result_t override {
        return plugin_result_t::success();
    }

    auto finalize() -> void override {}

    auto create_module() -> std::shared_ptr<native_module_t> override {
        auto module = std::make_shared<native_module_t>();

        ZEPHYR_FUNCTION(\"hello\", []() -> std::string {
            return \"Hello from ${PLUGIN_NAME}!\";
        });

        return module;
    }
};

ZEPHYR_PLUGIN(my_plugin_t)
EOF

# Create CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(${PLUGIN_NAME})

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZEPHYR REQUIRED zephyr)

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_NAME}.cpp)
target_include_directories(${PLUGIN_NAME} PRIVATE \${ZEPHYR_INCLUDE_DIRS})
target_link_libraries(${PLUGIN_NAME} \${ZEPHYR_LIBRARIES})

set_target_properties(${PLUGIN_NAME} PROPERTIES
    PREFIX \"\"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
EOF

echo \"Plugin \$PLUGIN_NAME created successfully!\"
echo \"To build: cd \$PLUGIN_NAME && mkdir build && cd build && cmake .. && make\"
"
)

# Make the script executable
if(UNIX)
    execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/create_plugin.sh)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/create_plugin.sh
        DESTINATION bin
        RENAME zephyr-create-plugin
    )
endif()
