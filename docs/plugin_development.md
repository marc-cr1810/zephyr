# Zephyr Plugin Development Guide

A comprehensive guide to creating dynamic plugins for the Zephyr programming language using C++. This guide covers everything from basic plugin creation to advanced features and best practices.

## Table of Contents

- [Introduction](#introduction)
- [Plugin Architecture](#plugin-architecture)
- [Quick Start](#quick-start)
- [Plugin Interface](#plugin-interface)
- [Module Creation](#module-creation)
- [Function Registration](#function-registration)
- [Type Conversion](#type-conversion)
- [Class Bindings](#class-bindings)
- [Error Handling](#error-handling)
- [Building Plugins](#building-plugins)
- [Advanced Features](#advanced-features)
- [Best Practices](#best-practices)
- [Testing Plugins](#testing-plugins)
- [Deployment](#deployment)
- [Examples](#examples)

## Introduction

Zephyr plugins are dynamic shared libraries that extend the functionality of Zephyr scripts. They allow you to:

- **Add Performance-Critical Functions** - Implement computationally intensive operations in C++
- **System Integration** - Access system APIs, hardware, or external libraries
- **Domain-Specific Features** - Create specialized functionality for specific use cases
- **Third-Party Integration** - Wrap existing C/C++ libraries for Zephyr use
- **Runtime Extension** - Add new capabilities without recompiling the main application

### Benefits of Plugins

- **Performance** - C++ implementation for speed-critical operations
- **Modularity** - Clean separation of concerns and optional features
- **Reusability** - Share plugins across multiple projects
- **Distribution** - Easy packaging and deployment
- **Dynamic Loading** - Load/unload plugins at runtime as needed

## Plugin Architecture

### Plugin Lifecycle

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Library       │    │    Plugin        │    │     Module      │
│   Loading       │───▶│  Initialization  │───▶│   Creation      │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                        │                      │
         │                        │                      ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│    Plugin       │    │    Plugin        │    │    Symbol       │
│   Finalization  │◀───│   Usage/Calls    │◀───│   Resolution    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

### Core Components

1. **Plugin Interface** (`plugin_interface_t`) - Base class for all plugins
2. **Native Module** (`native_module_t`) - Container for functions, classes, and constants
3. **Type Converter** - Automatic C++ ↔ Zephyr type conversion
4. **Dynamic Loader** - Runtime loading and management of shared libraries

## Quick Start

### 1. Basic Plugin Structure

```cpp
#include "zephyr/zephyr.hpp"

class my_plugin_t : public zephyr::api::plugin_interface_t {
public:
    // Plugin metadata
    auto get_plugin_info() const -> zephyr::api::plugin_info_t override {
        zephyr::api::plugin_info_t info;
        info.name = "my_plugin";
        info.description = "My awesome plugin";
        info.author = "Your Name";
        info.version = {1, 0, 0};
        info.min_zephyr_version = {1, 0, 0};
        info.license = "MIT";
        return info;
    }
    
    // Plugin initialization
    auto initialize(zephyr::api::engine_t* engine) -> zephyr::api::plugin_result_t override {
        m_engine = engine;
        return zephyr::api::plugin_result_t::success();
    }
    
    // Plugin cleanup
    auto finalize() -> void override {
        // Clean up resources if needed
    }
    
    // Create module with functions
    auto create_module() -> std::shared_ptr<zephyr::api::native_module_t> override {
        auto module = std::make_shared<zephyr::api::native_module_t>();
        module->set_name("my_plugin");
        
        // Register functions
        module->export_function("hello", zephyr::api::make_native_function(
            []() -> std::string {
                return "Hello from my plugin!";
            }, "hello"
        ));
        
        return module;
    }
    
private:
    zephyr::api::engine_t* m_engine = nullptr;
};

// Export the plugin
ZEPHYR_PLUGIN(my_plugin_t)
```

### 2. Build the Plugin

```cmake
# CMakeLists.txt
add_library(my_plugin SHARED my_plugin.cpp)
target_link_libraries(my_plugin zephyr::zephyr)
```

### 3. Use in Zephyr

```python
# Load and use the plugin
import my_plugin from "my_plugin.so"

message = my_plugin.hello()
print(message)  # Prints: "Hello from my plugin!"
```

## Plugin Interface

The `plugin_interface_t` is the base class that all plugins must inherit from.

### Required Methods

```cpp
class plugin_interface_t {
public:
    // Plugin metadata - REQUIRED
    virtual auto get_plugin_info() const -> plugin_info_t = 0;
    
    // Plugin lifecycle - REQUIRED
    virtual auto initialize(engine_t* engine) -> plugin_result_t = 0;
    virtual auto finalize() -> void = 0;
    
    // Module creation - REQUIRED
    virtual auto create_module() -> std::shared_ptr<native_module_t> = 0;
    
    // Optional override methods
    virtual auto get_symbol(const std::string& symbol_name) -> std::optional<value_t>;
    virtual auto get_all_symbols() -> std::optional<std::map<std::string, value_t>>;
    virtual auto configure(const std::map<std::string, std::string>& config) -> plugin_result_t;
    virtual auto health_check() -> plugin_result_t;
};
```

### Plugin Information

```cpp
auto get_plugin_info() const -> plugin_info_t override {
    plugin_info_t info;
    info.name = "math_utilities";
    info.description = "Advanced mathematical functions and utilities";
    info.author = "Math Team <math@example.com>";
    info.version = {2, 1, 0};  // major.minor.patch
    info.min_zephyr_version = {1, 0, 0};
    info.dependencies = {"core_math", "vector_ops"};
    info.license = "Apache-2.0";
    info.website = "https://github.com/example/math-plugin";
    return info;
}
```

### Initialization

```cpp
auto initialize(engine_t* engine) -> plugin_result_t override {
    if (!engine) {
        return plugin_result_t::error("Engine cannot be null");
    }
    
    m_engine = engine;
    
    // Perform initialization
    try {
        setup_internal_state();
        load_configuration();
        validate_dependencies();
        
        return plugin_result_t::success();
    } catch (const std::exception& e) {
        return plugin_result_t::error("Initialization failed: " + std::string(e.what()));
    }
}
```

## Module Creation

Native modules contain the actual functionality that plugins expose to Zephyr.

### Basic Module Setup

```cpp
auto create_module() -> std::shared_ptr<native_module_t> override {
    auto module = std::make_shared<native_module_t>();
    
    // Set module metadata
    module->set_name("math_utils");
    module->set_version("2.1.0");
    module->set_description("Mathematical utility functions");
    module->set_author("Math Team");
    
    // Add functions, classes, constants...
    register_functions(module);
    register_classes(module);
    register_constants(module);
    
    return module;
}
```

### Function Registration

```cpp
void register_functions(std::shared_ptr<native_module_t> module) {
    // Simple function
    module->export_function("add", zephyr::api::make_native_function(
        [](double a, double b) -> double {
            return a + b;
        }, "add"
    ));
    
    // Function with error handling
    module->export_function("sqrt", zephyr::api::make_native_function(
        [](double x) -> double {
            if (x < 0) {
                throw std::invalid_argument("Cannot take square root of negative number");
            }
            return std::sqrt(x);
        }, "sqrt"
    ));
    
    // Function with complex types
    module->export_function("sum_vector", zephyr::api::make_native_function(
        [](const std::vector<double>& numbers) -> double {
            return std::accumulate(numbers.begin(), numbers.end(), 0.0);
        }, "sum_vector"
    ));
    
    // Function with optional parameters (using overloads)
    module->export_function("power", zephyr::api::make_native_function(
        [](double base, double exp) -> double {
            return std::pow(base, exp);
        }, "power"
    ));
    
    // Set function documentation
    module->set_function_doc("add", "Adds two numbers and returns the result");
    module->set_function_doc("sqrt", "Returns the square root of a number (must be non-negative)");
}
```

### Constants Registration

```cpp
void register_constants(std::shared_ptr<native_module_t> module) {
    // Mathematical constants
    module->export_constant("PI", zephyr::api::to_zephyr(3.141592653589793));
    module->export_constant("E", zephyr::api::to_zephyr(2.718281828459045));
    module->export_constant("TAU", zephyr::api::to_zephyr(6.283185307179586));
    
    // String constants
    module->export_constant("VERSION", zephyr::api::to_zephyr(std::string("2.1.0")));
    
    // Complex constants
    std::vector<std::string> supported_ops = {"add", "sub", "mul", "div", "pow", "sqrt"};
    module->export_constant("SUPPORTED_OPERATIONS", zephyr::api::to_zephyr(supported_ops));
}
```

## Function Registration

### Supported Function Signatures

The plugin system supports functions with various signatures:

```cpp
// No parameters
module->export_function("get_time", zephyr::api::make_native_function(
    []() -> double {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
));

// Single parameter
module->export_function("abs", zephyr::api::make_native_function(
    [](double x) -> double { return std::abs(x); }
));

// Multiple parameters
module->export_function("clamp", zephyr::api::make_native_function(
    [](double value, double min_val, double max_val) -> double {
        return std::max(min_val, std::min(value, max_val));
    }
));

// Void return (returns none in Zephyr)
module->export_function("print_debug", zephyr::api::make_native_function(
    [](const std::string& message) -> void {
        std::cout << "[DEBUG] " << message << std::endl;
    }
));

// Complex return types
module->export_function("linspace", zephyr::api::make_native_function(
    [](double start, double stop, int num) -> std::vector<double> {
        std::vector<double> result;
        if (num <= 0) return result;
        if (num == 1) { result.push_back(start); return result; }
        
        double step = (stop - start) / (num - 1);
        for (int i = 0; i < num; ++i) {
            result.push_back(start + i * step);
        }
        return result;
    }
));
```

### Function Documentation

```cpp
// Add comprehensive documentation
module->export_function("interpolate", zephyr::api::make_native_function(
    [](const std::vector<double>& x, const std::vector<double>& y, double xi) -> double {
        // Linear interpolation implementation
        // ... implementation details ...
    }
));

module->set_function_doc("interpolate", R"(
    Linear interpolation between data points.
    
    Parameters:
        x (list): X coordinates of data points (must be sorted)
        y (list): Y coordinates of data points (same length as x)
        xi (float): X coordinate to interpolate at
    
    Returns:
        float: Interpolated Y value at xi
    
    Raises:
        ValueError: If x and y have different lengths or x is not sorted
        
    Example:
        x = [0, 1, 2, 3]
        y = [0, 1, 4, 9]
        result = interpolate(x, y, 1.5)  # Returns 2.5
)");
```

## Type Conversion

### Automatic Type Conversion

The plugin system automatically converts between C++ and Zephyr types:

| C++ Type | Zephyr Type | Notes |
|----------|-------------|-------|
| `int`, `long`, `size_t` | `int` | Automatic range checking |
| `float`, `double` | `float` | Full precision preserved |
| `std::string` | `string` | UTF-8 encoding supported |
| `bool` | `bool` | Direct mapping |
| `std::vector<T>` | `list` | Recursive conversion |
| `std::map<std::string, T>` | `dict` | String keys only |
| `std::optional<T>` | `T` or `none` | Null handling |

### Custom Type Conversions

```cpp
// Define custom type
struct Point2D {
    double x, y;
    Point2D(double x = 0, double y = 0) : x(x), y(y) {}
};

// Specialize conversion templates
namespace zephyr::api {
    template<>
    auto to_zephyr<Point2D>(const Point2D& point) -> value_t {
        std::map<std::string, value_t> dict;
        dict["x"] = to_zephyr(point.x);
        dict["y"] = to_zephyr(point.y);
        return to_zephyr(dict);
    }
    
    template<>
    auto from_zephyr<Point2D>(const value_t& value) -> result_t<Point2D> {
        auto dict_result = from_zephyr<std::map<std::string, value_t>>(value);
        if (!dict_result) {
            return result_t<Point2D>::error("Expected dictionary");
        }
        
        auto dict = dict_result.value();
        auto x_it = dict.find("x");
        auto y_it = dict.find("y");
        
        if (x_it == dict.end() || y_it == dict.end()) {
            return result_t<Point2D>::error("Missing x or y coordinate");
        }
        
        auto x = from_zephyr<double>(x_it->second);
        auto y = from_zephyr<double>(y_it->second);
        
        if (!x || !y) {
            return result_t<Point2D>::error("Invalid coordinate values");
        }
        
        return result_t<Point2D>::success(Point2D(x.value(), y.value()));
    }
}

// Use custom type in functions
module->export_function("distance", zephyr::api::make_native_function(
    [](const Point2D& p1, const Point2D& p2) -> double {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        return std::sqrt(dx*dx + dy*dy);
    }
));
```

## Class Bindings

### Basic Class Binding

```cpp
class Vector3D {
public:
    Vector3D(double x = 0, double y = 0, double z = 0) : m_x(x), m_y(y), m_z(z) {}
    
    // Getters
    double x() const { return m_x; }
    double y() const { return m_y; }
    double z() const { return m_z; }
    
    // Setters
    void set_x(double x) { m_x = x; }
    void set_y(double y) { m_y = y; }
    void set_z(double z) { m_z = z; }
    
    // Methods
    double magnitude() const {
        return std::sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
    }
    
    Vector3D normalized() const {
        double mag = magnitude();
        if (mag == 0) return Vector3D();
        return Vector3D(m_x/mag, m_y/mag, m_z/mag);
    }
    
    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
    }
    
    std::string to_string() const {
        return "Vector3D(" + std::to_string(m_x) + ", " + 
               std::to_string(m_y) + ", " + std::to_string(m_z) + ")";
    }

private:
    double m_x, m_y, m_z;
};

void register_classes(std::shared_ptr<native_module_t> module) {
    // Register class constructor
    module->export_function("Vector3D", zephyr::api::make_native_function(
        [](double x, double y, double z) -> Vector3D {
            return Vector3D(x, y, z);
        }
    ));
    
    // Register instance methods
    module->export_function("vector_magnitude", zephyr::api::make_native_function(
        [](const Vector3D& v) -> double {
            return v.magnitude();
        }
    ));
    
    module->export_function("vector_normalized", zephyr::api::make_native_function(
        [](const Vector3D& v) -> Vector3D {
            return v.normalized();
        }
    ));
    
    module->export_function("vector_add", zephyr::api::make_native_function(
        [](const Vector3D& a, const Vector3D& b) -> Vector3D {
            return a + b;
        }
    ));
    
    module->export_function("vector_to_string", zephyr::api::make_native_function(
        [](const Vector3D& v) -> std::string {
            return v.to_string();
        }
    ));
}
```

### Usage in Zephyr

```python
import my_plugin from "my_plugin.so"

# Create vector
v1 = my_plugin.Vector3D(1.0, 2.0, 3.0)
v2 = my_plugin.Vector3D(4.0, 5.0, 6.0)

# Use methods
magnitude = my_plugin.vector_magnitude(v1)
normalized = my_plugin.vector_normalized(v1)
sum_vector = my_plugin.vector_add(v1, v2)

print("v1 magnitude:", magnitude)
print("v1 + v2 =", my_plugin.vector_to_string(sum_vector))
```

## Error Handling

### Function Error Handling

```cpp
module->export_function("safe_divide", zephyr::api::make_native_function(
    [](double a, double b) -> double {
        if (b == 0.0) {
            throw std::invalid_argument("Division by zero");
        }
        if (std::isnan(a) || std::isnan(b)) {
            throw std::invalid_argument("Cannot divide NaN values");
        }
        return a / b;
    }
));

module->export_function("parse_number", zephyr::api::make_native_function(
    [](const std::string& str) -> double {
        try {
            size_t pos;
            double result = std::stod(str, &pos);
            if (pos != str.length()) {
                throw std::invalid_argument("Invalid number format");
            }
            return result;
        } catch (const std::exception& e) {
            throw std::invalid_argument("Failed to parse number: " + std::string(e.what()));
        }
    }
));
```

### Plugin-Level Error Handling

```cpp
auto initialize(engine_t* engine) -> plugin_result_t override {
    try {
        // Risky initialization
        load_external_library();
        validate_system_requirements();
        
        return plugin_result_t::success();
    } catch (const std::runtime_error& e) {
        return plugin_result_t::error("Runtime error during initialization: " + std::string(e.what()));
    } catch (const std::exception& e) {
        return plugin_result_t::error("Unexpected error: " + std::string(e.what()));
    }
}

auto health_check() -> plugin_result_t override {
    try {
        // Check if external dependencies are still available
        if (!check_external_service()) {
            return plugin_result_t::error("External service unavailable");
        }
        
        // Check internal state
        if (!validate_internal_state()) {
            return plugin_result_t::error("Internal state corruption detected");
        }
        
        return plugin_result_t::success();
    } catch (...) {
        return plugin_result_t::error("Health check failed with exception");
    }
}
```

## Building Plugins

### CMake Configuration

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyZephyrPlugin VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Zephyr
find_package(Zephyr REQUIRED)

# Plugin library
add_library(my_plugin SHARED
    src/my_plugin.cpp
    src/math_functions.cpp
    src/vector_operations.cpp
)

# Link with Zephyr
target_link_libraries(my_plugin
    zephyr::zephyr
)

# Set plugin properties
set_target_properties(my_plugin PROPERTIES
    PREFIX ""  # Remove 'lib' prefix on Unix systems
    SUFFIX ".zephyr"  # Custom extension (optional)
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

# Include directories
target_include_directories(my_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler-specific options
if(MSVC)
    target_compile_options(my_plugin PRIVATE /W4)
else()
    target_compile_options(my_plugin PRIVATE -Wall -Wextra -pedantic)
endif()

# Install plugin
install(TARGETS my_plugin
    LIBRARY DESTINATION lib/zephyr/plugins
    RUNTIME DESTINATION bin/zephyr/plugins
)
```

### Manual Build Commands

```bash
# Linux/macOS with GCC
g++ -std=c++17 -fPIC -shared -I/usr/local/include \
    my_plugin.cpp -L/usr/local/lib -lzephyr -o my_plugin.so

# Linux/macOS with Clang
clang++ -std=c++17 -fPIC -shared -I/usr/local/include \
    my_plugin.cpp -L/usr/local/lib -lzephyr -o my_plugin.so

# Windows with MSVC
cl /EHsc /std:c++17 /LD /I"C:\zephyr\include" \
    my_plugin.cpp /link "C:\zephyr\lib\zephyr.lib" /OUT:my_plugin.dll

# macOS specific (for proper loading)
clang++ -std=c++17 -fPIC -shared -undefined dynamic_lookup \
    my_plugin.cpp -o my_plugin.dylib
```

## Advanced Features

### Configuration Support

```cpp
class configurable_plugin_t : public zephyr::api::plugin_interface_t {
public:
    auto configure(const std::map<std::string, std::string>& config) -> plugin_result_t override {
        auto it = config.find("max_iterations");
        if (it != config.end()) {
            try {
                m_max_iterations = std::stoi(it->second);
            } catch (...) {
                return plugin_result_t::error("Invalid max_iterations value");
            }
        }
        
        it = config.find("precision");
        if (it != config.end()) {
            try {
                m_precision = std::stod(it->second);
            } catch (...) {
                return plugin_result_t::error("Invalid precision value");
            }
        }
        
        return plugin_result_t::success();
    }
    
private:
    int m_max_iterations = 1000;
    double m_precision = 1e-6;
};
```

### Named Symbol Export

```cpp
auto get_symbol(const std::string& symbol_name) -> std::optional<value_t> override {
    if (symbol_name == "VERSION") {
        return zephyr::api::to_zephyr(std::string("2.1.0"));
    }
    
    if (symbol_name == "author_info") {
        std::map<std::string, std::string> info = {
            {"name", "John Doe"},
            {"email", "john@example.com"},
            {"organization", "Math Corp"}
        };
        return zephyr::api::to_zephyr(info);
    }
    
    return std::nullopt;
}

auto get_all_symbols() -> std::optional<std::map<std::string, value_t>> override {
    std::map<std::string, value_t> symbols;
    
    symbols["VERSION"] = zephyr::api::to_zephyr(std::string("2.1.0"));
    symbols["BUILD_DATE"] = zephyr::api::to_zephyr(std::string(__DATE__));
    symbols["BUILD_TIME"] = zephyr::api::to_zephyr(std::string(__TIME__));
    
    return symbols;
}
```

### Plugin Dependencies

```cpp
auto get_plugin_info() const -> plugin_info_t override {
    plugin_info_t info;
    info.name = "advanced_math";
    info.version = {2, 0, 0};
    info.min_zephyr_version = {1, 0, 0};
    
    // Declare dependencies
    info.dependencies = {
        "core_math>=1.0.0",
        "linear_algebra>=2.1.0",
        "statistics>=1.5.0"
    };
    
    return info;
}

auto initialize(engine_t* engine) -> plugin_result_t override {
    // Verify dependencies are loaded
    if (!engine->is_plugin_loaded("core_math.so")) {
        return plugin_result_t::error("Required dependency 'core_math' not loaded");
    }
    
    // Access dependency functions
    auto core_math = engine->get_plugin("core_math.so");
    if (!core_math) {
        return plugin_result_t::error("Cannot access core_math plugin");
    }
    
    return plugin_result_t::success();
}
```

## Best Practices

### 1. Memory Management

```cpp
// ✅ Good: Use smart pointers
class good_plugin_t : public zephyr::api::plugin_interface_t {
    std::unique_ptr<heavy_resource_t> m_resource;
    std::shared_ptr<shared_data_t> m_shared_data;
};

// ❌ Bad: Raw pointers
class bad_plugin_t : public zephyr::api::plugin_interface_t {
    heavy_resource_t* m_resource;  // Potential memory leak
    shared_data_t* m_shared_data;  // Ownership unclear
};
```

### 2. Error Handling

```cpp
// ✅ Good: Comprehensive error handling
module->export_function("safe_operation", zephyr::api::make_native_function(
    [](const std::string& input) -> std::string {
        if (input.empty()) {
            throw std::invalid_argument("Input cannot be empty");
        }
        
        try {
            return process_input(input);
        } catch (const processing_error& e) {
            throw std::runtime_error("Processing failed: " + std::string(e.what()));
        } catch (const std::exception& e) {
            throw std::runtime_error("Unexpected error: " + std::string(e.what()));
        }
    }
));

// ❌ Bad: No error handling
module->export_function("unsafe_operation", zephyr::api::make_native_function(
    [](const std::string& input) -> std::string {
        return process_input(input);  // May throw unhandled exceptions
    }
));
```

### 3. Thread Safety

```cpp
// ✅ Good: Thread-safe implementation
class thread_safe_plugin_t : public zephyr::api::plugin_interface_t {
    mutable std::mutex m_mutex;
    std::map<std::string, int> m_state;
    
public:
    auto create_module() -> std::shared_ptr<native_module_t> override {
        auto module = std::make_shared<native_module_t>();
        
        module->export_function("get_state", zephyr::api::make_native_function(
            [this](const std::string& key) -> int {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto it = m_state.find(key);
                return it != m_state.end() ? it->second : 0;
            }
        ));
        
        return module;
    }
};
```

### 4. Resource Management

```cpp
class resource_managed_plugin_t : public zephyr::api::plugin_interface_t {
    std::unique_ptr<external_library_t> m_library;
    
public:
    auto initialize(engine_t* engine) -> plugin_result_t override {
        try {
            m_library = std::make_unique<external_library_t>();
            m_library->initialize();
            return plugin_result_t::success();
        } catch (const std::exception& e) {
            return plugin_result_t::error("Failed to initialize library: " + std::string(e.what()));
        }
    }
    
    auto finalize() -> void override {
        if (m_library) {
            m_library->cleanup();
            m_library.reset();
        }
    }
};
```

### 5. Documentation

```cpp
// ✅ Good: Well-documented functions
module->export_function("calculate_distance", zephyr::api::make_native_function(
    [](const std::vector<double>& point1, const std::vector<double>& point2) -> double {
        // Implementation...
    }
));

module->set_function_doc("calculate_distance", R"(
    Calculate Euclidean distance between two points.
    
    Args:
        point1 (list): First point coordinates
        point2 (list): Second point coordinates (must have same length as point1)
    
    Returns:
        float: Euclidean distance between the points
    
    Raises:
        ValueError: If points have different dimensions
        TypeError: If coordinates are not numeric
    
    Example:
        distance = calculate_distance([0, 0], [