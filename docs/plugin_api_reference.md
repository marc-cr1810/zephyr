# Zephyr Plugin API Documentation

A comprehensive C++ API for embedding Zephyr and creating dynamic plugins.

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Quick Start](#quick-start)
- [API Components](#api-components)
- [Creating Plugins](#creating-plugins)
- [Building Plugins](#building-plugins)
- [Usage Examples](#usage-examples)
- [Best Practices](#best-practices)
- [Performance Considerations](#performance-considerations)
- [Troubleshooting](#troubleshooting)
- [API Reference](#api-reference)

## Overview

The Zephyr Plugin API enables:

1. **Embedding Zephyr** in C++ applications
2. **Dynamic plugin loading** from shared libraries (.so, .dll, .dylib)
3. **Seamless interop** between C++ and Zephyr data types
4. **Runtime extension** of Zephyr with native C++ functionality

## Key Features

### 🚀 **Embedding Engine**
- Execute Zephyr scripts from C++
- Register C++ functions and classes
- Bidirectional data conversion
- Error handling and debugging

### 🔌 **Dynamic Plugin System**
- Load C++ shared libraries at runtime
- Import plugins directly in Zephyr code
- Version compatibility checking
- Dependency management

### 🔄 **Type Conversion**
- Automatic conversion between C++ and Zephyr types
- Support for containers (vectors, maps)
- Optional and variant type support
- Custom type registration

### ⚡ **Performance**
- Zero-copy operations where possible
- Thread-safe plugin loading
- Efficient native function calls
- Minimal runtime overhead

## Quick Start

### 1. Basic Embedding

```cpp
#include "api/zephyr_api.hpp"

int main() {
    // Create engine
    auto engine = zephyr::api::create_engine();

    // Register C++ function
    engine->register_function("add", [](double a, double b) {
        return a + b;
    });

    // Execute Zephyr code
    auto result = engine->execute_string(R"(
        result = add(5.0, 3.0)
        print("5 + 3 =", result)
        return result
    )");

    if (result) {
        std::cout << "Success!" << std::endl;
    } else {
        std::cout << "Error: " << result.error() << std::endl;
    }

    return 0;
}
```

### 2. Creating a Simple Plugin

```cpp
#include "api/plugin_interface.hpp"
#include "api/native_module.hpp"

class my_plugin_t : public zephyr::api::plugin_interface_t {
public:
    auto get_plugin_info() const -> zephyr::api::plugin_info_t override {
        return {
            .name = "my_plugin",
            .description = "Example plugin",
            .author = "Your Name",
            .version = {1, 0, 0},
            .min_zephyr_version = {1, 0, 0}
        };
    }

    auto initialize(zephyr::api::engine_t* engine) -> zephyr::api::plugin_result_t override {
        return zephyr::api::plugin_result_t::success();
    }

    auto finalize() -> void override {}

    auto create_module() -> std::shared_ptr<zephyr::api::native_module_t> override {
        auto module = std::make_shared<zephyr::api::native_module_t>();

        // Add functions to module
        ZEPHYR_FUNCTION("hello", []() -> std::string {
            return "Hello from C++!";
        });

        return module;
    }
};

// Export plugin
ZEPHYR_PLUGIN(my_plugin_t)
```

### 3. Using Plugins in Zephyr

```zephyr
# Import entire plugin
import my_plugin from "my_plugin.so"
message = my_plugin.hello()
print(message)

# Named imports
import hello from "my_plugin.so"
print(hello())

# Namespace import
import * as mp from "my_plugin.so"
print(mp.hello())
```

## API Components

### Core Classes

| Class | Purpose |
|-------|---------|
| `engine_t` | Main embedding engine |
| `plugin_interface_t` | Base class for plugins |
| `native_module_t` | Module implementation |
| `native_function_t` | Function wrapper |
| `plugin_loader_t` | Dynamic library loader |

### Type System

| C++ Type | Zephyr Type | Conversion |
|----------|-------------|------------|
| `int`, `long` | `int` | Automatic |
| `float`, `double` | `float` | Automatic |
| `std::string` | `string` | Automatic |
| `bool` | `boolean` | Automatic |
| `std::vector<T>` | `list` | Recursive |
| `std::map<std::string, T>` | `dictionary` | Recursive |
| `std::optional<T>` | `T \| none` | Automatic |

### Result Types

```cpp
// Success/Error result wrapper
template<typename T>
class result_t {
public:
    static auto success(T value) -> result_t<T>;
    static auto error(const std::string& message) -> result_t<T>;

    auto is_success() const -> bool;
    auto value() const -> const T&;
    auto error() const -> std::string;
};

// Common aliases
using void_result_t = result_t<void>;
using value_result_t = result_t<value_t>;
```

## Creating Plugins

### Plugin Structure

Every plugin must implement the `plugin_interface_t`:

```cpp
class my_plugin_t : public plugin_interface_t {
    // Required methods
    auto get_plugin_info() const -> plugin_info_t override;
    auto initialize(engine_t* engine) -> plugin_result_t override;
    auto finalize() -> void override;
    auto create_module() -> std::shared_ptr<native_module_t> override;

    // Optional methods
    auto get_symbol(const std::string& name) -> std::optional<value_t> override;
    auto get_all_symbols() -> std::optional<std::map<std::string, value_t>> override;
    auto health_check() -> plugin_result_t override;
};
```

### Function Registration

```cpp
class math_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        // Simple function
        ZEPHYR_FUNCTION("add", [](double a, double b) -> double {
            return a + b;
        });

        // Function with error handling
        export_function("divide", make_native_function([](double a, double b) -> double {
            if (b == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            return a / b;
        }));

        // Variadic function
        export_function("sum", make_native_function([](const std::vector<double>& numbers) -> double {
            double total = 0.0;
            for (double n : numbers) total += n;
            return total;
        }));

        // Constants
        export_constant("PI", to_zephyr(3.14159265359));

        return void_result_t::success();
    }
};
```

### Class Registration

```cpp
class vector3d_t {
public:
    vector3d_t(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

    auto magnitude() const -> double {
        return std::sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
    }

    auto get_x() const -> double { return m_x; }
    auto set_x(double x) -> void { m_x = x; }

private:
    double m_x, m_y, m_z;
};

class vector3d_class_t : public native_class_t {
public:
    vector3d_class_t() {
        // Constructor
        set_constructor(make_native_function([](double x, double y, double z) {
            return std::make_shared<vector3d_t>(x, y, z);
        }));

        // Methods
        ZEPHYR_METHOD("magnitude", &vector3d_t::magnitude, instance);

        // Properties
        ZEPHYR_PROPERTY("x", &vector3d_t::get_x, &vector3d_t::set_x, instance);
    }

    auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
        // Handle constructor arguments
        if (args.size() != 3) {
            return value_result_t::error("Vector3D requires 3 arguments");
        }

        auto x = from_zephyr<double>(args[0]);
        auto y = from_zephyr<double>(args[1]);
        auto z = from_zephyr<double>(args[2]);

        if (!x || !y || !z) {
            return value_result_t::error("All arguments must be numbers");
        }

        auto vector = std::make_shared<vector3d_t>(x.value(), y.value(), z.value());
        return value_result_t::success(to_zephyr(vector));
    }
};
```

## Building Plugins

### CMakeLists.txt Example

```cmake
cmake_minimum_required(VERSION 3.10)
project(my_plugin)

set(CMAKE_CXX_STANDARD 17)

# Find Zephyr headers
find_path(ZEPHYR_INCLUDE_DIR
    NAMES api/plugin_interface.hpp
    PATHS /usr/include/zephyr /usr/local/include/zephyr
)

# Create shared library
add_library(my_plugin SHARED
    my_plugin.cpp
)

target_include_directories(my_plugin PRIVATE
    ${ZEPHYR_INCLUDE_DIR}
)

# Platform-specific settings
if(WIN32)
    set_target_properties(my_plugin PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
    )
elseif(APPLE)
    set_target_properties(my_plugin PROPERTIES
        PREFIX ""
        SUFFIX ".dylib"
    )
else()
    set_target_properties(my_plugin PROPERTIES
        PREFIX ""
        SUFFIX ".so"
    )
endif()

# Hide symbols by default
set_target_properties(my_plugin PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Link math library on Unix
if(UNIX)
    target_link_libraries(my_plugin m)
endif()
```

### Manual Build Commands

```bash
# Linux/macOS
g++ -std=c++17 -fPIC -shared -o my_plugin.so my_plugin.cpp -I/path/to/zephyr/include

# Windows (MinGW)
g++ -std=c++17 -shared -o my_plugin.dll my_plugin.cpp -I/path/to/zephyr/include

# Windows (MSVC)
cl /std:c++17 /LD my_plugin.cpp /I"C:\path\to\zephyr\include"
```

## Usage Examples

### Advanced Plugin Usage

```zephyr
# Load complex plugin
import advanced_math from "math_plugin.so"

# Mathematical operations
result = advanced_math.sin(advanced_math.PI / 4)
print("sin(π/4) =", result)

# Statistical functions
data = [1.2, 2.3, 1.8, 3.1, 2.7]
mean = advanced_math.mean(data)
median = advanced_math.median(data)
print("Mean:", mean, "Median:", median)

# Vector operations
v1 = advanced_math.Vector3D(1.0, 0.0, 0.0)
v2 = advanced_math.Vector3D(0.0, 1.0, 0.0)
cross = v1.cross(v2)
print("Cross product:", cross.to_string())

# Error handling
try {
    result = advanced_math.sqrt(-1.0)
} catch (error) {
    print("Caught domain error:", error)
}
```

### C++ Integration

```cpp
#include "api/zephyr_api.hpp"

class application_t {
public:
    void setup_zephyr() {
        m_engine = zephyr::api::create_engine();

        // Add library search paths
        m_engine->add_library_search_path("./plugins");
        m_engine->add_library_search_path("/usr/local/lib/zephyr/plugins");

        // Register application functions
        m_engine->register_function("log_message", [this](const std::string& msg) {
            this->log(msg);
        });

        // Load plugins
        auto math_result = m_engine->load_plugin("math_plugin.so");
        if (!math_result) {
            std::cerr << "Failed to load math plugin: " << math_result.error() << std::endl;
        }

        // Set error handler
        m_engine->set_error_handler([this](const zephyr::api::error_info_t& error) {
            this->handle_script_error(error);
        });
    }

    void run_script(const std::string& script_content) {
        auto result = m_engine->execute_string(script_content);
        if (!result) {
            std::cerr << "Script execution failed: " << result.error() << std::endl;
        }
    }

private:
    std::unique_ptr<zephyr::api::engine_t> m_engine;

    void log(const std::string& message) {
        std::cout << "[APP] " << message << std::endl;
    }

    void handle_script_error(const zephyr::api::error_info_t& error) {
        std::cerr << "[SCRIPT ERROR] " << error.to_string() << std::endl;
    }
};
```

## Best Practices

### Plugin Development

1. **Version Compatibility**
   ```cpp
   auto get_plugin_info() const -> plugin_info_t override {
       return {
           .min_zephyr_version = {1, 0, 0},  // Specify minimum version
           .version = {2, 1, 0}              // Use semantic versioning
       };
   }
   ```

2. **Error Handling**
   ```cpp
   export_function("safe_divide", make_native_function([](double a, double b) -> double {
       if (b == 0.0) {
           throw std::domain_error("Division by zero");
       }
       return a / b;
   }));
   ```

3. **Resource Management**
   ```cpp
   class resource_plugin_t : public plugin_interface_t {
       auto initialize(engine_t* engine) -> plugin_result_t override {
           m_resource = acquire_resource();
           return plugin_result_t::success();
       }

       auto finalize() -> void override {
           release_resource(m_resource);
       }
   };
   ```

4. **Thread Safety**
   ```cpp
   class thread_safe_plugin_t : public plugin_interface_t {
       mutable std::mutex m_mutex;

       auto thread_safe_operation() -> result_t<int> {
           std::lock_guard<std::mutex> lock(m_mutex);
           // Thread-safe operations
           return result_t<int>::success(42);
       }
   };
   ```

### Performance Optimization

1. **Minimize Conversions**
   ```cpp
   // Efficient: Direct value_t manipulation
   export_function("process_list", make_native_function([](const value_t& list) -> value_t {
       // Work directly with Zephyr objects
       return process_zephyr_list(list);
   }));

   // Less efficient: Multiple conversions
   export_function("process_list", make_native_function([](const std::vector<double>& nums) -> std::vector<double> {
       // Converts vector → value_t → vector → value_t
       return process_cpp_vector(nums);
   }));
   ```

2. **Batch Operations**
   ```cpp
   // Good: Process arrays in bulk
   export_function("process_batch", make_native_function([](const std::vector<double>& data) -> std::vector<double> {
       std::vector<double> result;
       result.reserve(data.size());

       for (double val : data) {
           result.push_back(expensive_operation(val));
       }

       return result;
   }));
   ```

3. **Avoid Exceptions in Hot Paths**
   ```cpp
   // Use result types instead of exceptions for expected errors
   export_function("safe_operation", make_native_function([](double input) -> result_t<double> {
       if (input < 0) {
           return result_t<double>::error("Negative input not allowed");
       }
       return result_t<double>::success(std::sqrt(input));
   }));
   ```

## Performance Considerations

### Optimization Tips

1. **Use `value_t` directly for complex operations**
2. **Batch process arrays and containers**
3. **Cache frequently used conversions**
4. **Avoid string operations in tight loops**
5. **Use move semantics where possible**

## Troubleshooting

### Common Issues

1. **Plugin Not Loading**
   ```
   Error: Failed to load plugin: undefined symbol
   ```
   **Solution:** Ensure all symbols are exported and dependencies are linked.

2. **Type Conversion Errors**
   ```
   Error: Expected number, got string
   ```
   **Solution:** Add type validation in your functions.

3. **Version Incompatibility**
   ```
   Error: Plugin requires Zephyr version 1.2.0, but 1.0.0 is loaded
   ```
   **Solution:** Update Zephyr or adjust plugin requirements.

### Debug Mode

```cpp
auto engine = zephyr::api::create_engine();
engine->set_debug_mode(true);

// Enable verbose logging
engine->set_error_handler([](const auto& error) {
    std::cout << "DEBUG: " << error.to_string() << std::endl;
    if (!error.stack_trace.empty()) {
        std::cout << "Stack trace:\n" << error.stack_trace << std::endl;
    }
});
```

### Plugin Validation

```cpp
// Check plugin before use
auto loader = engine->get_plugin_loader();
auto validation_result = loader->validate_plugin_library("my_plugin.so");

if (!validation_result) {
    std::cerr << "Plugin validation failed: " << validation_result.error() << std::endl;
}
```

## API Reference

### Core Functions

```cpp
namespace zephyr::api {
    // Engine creation
    auto create_engine() -> std::unique_ptr<engine_t>;
    auto create_engine(const engine_config_t& config) -> std::unique_ptr<engine_t>;

    // Type conversions
    template<typename T> auto to_zephyr(const T& value) -> value_t;
    template<typename T> auto from_zephyr(const value_t& value) -> result_t<T>;

    // Function wrapping
    template<typename F> auto make_native_function(F&& func) -> native_function_t;

    // Global convenience
    auto default_engine() -> engine_t&;
    auto execute(const std::string& code) -> value_result_t;
}
```

### Macros

```cpp
// Plugin declaration
#define ZEPHYR_PLUGIN(class_name)

// Function registration
#define ZEPHYR_FUNCTION(name, func)

// Method registration
#define ZEPHYR_METHOD(name, method, instance)

// Property registration
#define ZEPHYR_PROPERTY(name, getter, setter, instance)
```

---

## Contributing

We welcome contributions to the Zephyr Plugin API! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

## License

The Zephyr Plugin API is released under the [MIT License](LICENSE).

---

**Need Help?**
- 📖 [Full Documentation](https://zephyr-lang.org/docs)
- 💬 [Community Forum](https://zephyr-lang.org/forum)
- 🐛 [Issue Tracker](https://github.com/zephyr-lang/zephyr/issues)
- 📧 [Email Support](mailto:support@zephyr-lang.org)
