# Zephyr C++ API Usage Guide

## 🚀 **Complete Integration Guide for the Zephyr C++ Plugin API**

The Zephyr C++ API is now **fully integrated** and **ready for production use**! This guide shows you exactly how to use all the features.

## 📋 **Quick Start**

### 1. **Basic Embedding in C++**

```cpp
#include "zephyr/zephyr.hpp"
#include <iostream>

int main() {
    // Create a Zephyr engine
    auto engine = zephyr::api::create_engine();
    
    // Register a C++ function
    engine->register_function("add", [](double a, double b) -> double {
        return a + b;
    });
    
    // Execute Zephyr code
    auto result = engine->execute_string(R"(
        result = add(5.0, 3.0)
        print("5 + 3 = " + str(result))
        return result
    )");
    
    if (result) {
        std::cout << "Result: " << result.value()->to_string() << std::endl;
    } else {
        std::cout << "Error: " << result.error() << std::endl;
    }
    
    return 0;
}
```

### 2. **Creating a Plugin**

```cpp
// my_plugin.cpp
#include "zephyr/zephyr.hpp"

class my_plugin_t : public zephyr::api::plugin_interface_t {
public:
    auto get_plugin_info() const -> zephyr::api::plugin_info_t override {
        zephyr::api::plugin_info_t info;
        info.name = "my_plugin";
        info.description = "My awesome plugin";
        info.author = "Your Name";
        info.version = {1, 0, 0};
        info.min_zephyr_version = {1, 0, 0};
        return info;
    }
    
    auto initialize(zephyr::api::engine_t* engine) -> zephyr::api::plugin_result_t override {
        return zephyr::api::plugin_result_t::success();
    }
    
    auto finalize() -> void override {}
    
    auto create_module() -> std::shared_ptr<zephyr::api::native_module_t> override {
        auto module = std::make_shared<zephyr::api::native_module_t>();
        module->set_name("my_plugin");
        
        // Export functions
        module->export_function("hello", zephyr::api::make_native_function(
            []() -> std::string {
                return "Hello from C++!";
            }, "hello"
        ));
        
        module->export_function("multiply", zephyr::api::make_native_function(
            [](double a, double b) -> double {
                return a * b;
            }, "multiply"
        ));
        
        // Export constants
        module->export_constant("PI", zephyr::api::to_zephyr(3.14159));
        
        return module;
    }
};

// Plugin exports
ZEPHYR_PLUGIN(my_plugin_t)
```

### 3. **Using the Plugin in Zephyr**

```zephyr
# Load the plugin
import my_plugin from "my_plugin.so"

# Use plugin functions
greeting = my_plugin.hello()
print(greeting)  # Prints: "Hello from C++!"

result = my_plugin.multiply(6, 7)
print("6 * 7 = " + str(result))  # Prints: "6 * 7 = 42"

# Use plugin constants
print("PI = " + str(my_plugin.PI))
```

## 🎯 **Core Features**

### **Engine Management**

```cpp
#include "zephyr/zephyr.hpp"

// Create engine
auto engine = zephyr::api::create_engine();

// Configure engine
engine->set_debug_mode(true);
engine->set_name("My App Engine");
engine->add_import_path("./plugins");

// Engine info
std::cout << "Engine: " << engine->get_name() << std::endl;
std::cout << "Version: " << engine->get_version() << std::endl;
```

### **Function Registration**

```cpp
// Simple function
engine->register_function("add", [](double a, double b) -> double {
    return a + b;
});

// Function with error handling
engine->register_function("safe_divide", [](double a, double b) -> double {
    if (b == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
});

// String function
engine->register_function("greet", [](const std::string& name) -> std::string {
    return "Hello, " + name + "!";
});

// Integer function
engine->register_function("factorial", [](int n) -> int {
    if (n <= 1) return 1;
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
});
```

### **Global Variables**

```cpp
// Set global variables
engine->set_global("app_version", std::string("1.2.3"));
engine->set_global("debug", true);
engine->set_global("max_users", 1000);

// Get global variables
auto version = engine->get_global<std::string>("app_version");
auto debug = engine->get_global<bool>("debug");
auto max_users = engine->get_global<int>("max_users");

if (version) {
    std::cout << "Version: " << version.value() << std::endl;
}

// List all globals
for (const auto& name : engine->list_globals()) {
    std::cout << "Global: " << name << std::endl;
}
```

### **Executing Zephyr Code**

```cpp
// Execute from string
auto result = engine->execute_string(R"(
    x = 10
    y = 20
    return x + y
)");

// Execute from file
auto file_result = engine->execute_file("script.zephyr");

// Evaluate expression
auto expr_result = engine->evaluate_expression("factorial(5) + 10");

// Call Zephyr function from C++
auto call_result = engine->call_function("my_zephyr_function", 42, "hello");

// Handle results
if (result) {
    std::cout << "Success: " << result.value()->to_string() << std::endl;
} else {
    std::cout << "Error: " << result.error() << std::endl;
}
```

### **Plugin Loading**

```cpp
// Load plugin
auto plugin_result = engine->load_plugin("math_plugin.so");

if (plugin_result) {
    auto plugin = plugin_result.value();
    auto info = plugin->get_plugin_info();
    std::cout << "Loaded: " << info.name << " v" << info.version.to_string() << std::endl;
} else {
    std::cout << "Failed to load plugin: " << plugin_result.error() << std::endl;
}

// Add search paths
engine->add_library_search_path("./plugins");
engine->add_library_search_path("/usr/local/lib/zephyr");

// Scan directory for plugins
auto scan_results = engine->scan_and_load_plugins("./plugins", true);
for (const auto& result : scan_results) {
    if (result) {
        std::cout << "Loaded plugin: " << result.value()->get_plugin_info().name << std::endl;
    }
}
```

## 🔧 **Advanced Features**

### **Type Conversion**

```cpp
using namespace zephyr::api;

// Convert C++ types to Zephyr
auto zephyr_int = to_zephyr(42);
auto zephyr_string = to_zephyr(std::string("hello"));
auto zephyr_vector = to_zephyr(std::vector<double>{1.0, 2.0, 3.0});

// Convert Zephyr types to C++
auto cpp_result = from_zephyr<int>(zephyr_value);
if (cpp_result) {
    int value = cpp_result.value();
} else {
    std::cout << "Conversion failed: " << cpp_result.error() << std::endl;
}

// Safe conversion (returns optional)
auto safe_value = try_from_zephyr<double>(zephyr_value);
if (safe_value) {
    double val = safe_value.value();
}
```

### **Custom Classes** *(Basic Support)*

```cpp
// Simple C++ class
class Vector2D {
public:
    Vector2D(double x, double y) : m_x(x), m_y(y) {}
    double magnitude() const { return std::sqrt(m_x*m_x + m_y*m_y); }
    double get_x() const { return m_x; }
    double get_y() const { return m_y; }
private:
    double m_x, m_y;
};

// Register class methods as functions
auto vec = std::make_shared<Vector2D>(3.0, 4.0);

engine->register_function("vector_magnitude", [vec]() -> double {
    return vec->magnitude();
});

engine->register_function("vector_x", [vec]() -> double {
    return vec->get_x();
});
```

### **Error Handling**

```cpp
// Set global error handler
engine->set_error_handler([](const zephyr::api::error_info_t& error) {
    std::cerr << "Zephyr Error: " << error.to_string() << std::endl;
});

// Check for errors
auto result = engine->execute_string("invalid syntax here");
if (!result) {
    std::cout << "Error: " << result.error() << std::endl;
    
    auto last_error = engine->get_last_error();
    if (last_error) {
        std::cout << "Details: " << last_error->to_string() << std::endl;
    }
}
```

### **Statistics and Monitoring**

```cpp
// Get engine statistics
auto stats = engine->get_stats();
std::cout << "Functions registered: " << stats.functions_registered << std::endl;
std::cout << "Scripts executed: " << stats.scripts_executed << std::endl;
std::cout << "Total execution time: " << stats.total_execution_time.count() << "ms" << std::endl;

// List registered items
std::cout << "Registered functions:" << std::endl;
for (const auto& name : engine->list_functions()) {
    std::cout << "  - " << name << std::endl;
}
```

## 📦 **Building and Integration**

### **CMakeLists.txt Example**

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyZephyrApp)

# Find Zephyr
find_package(Zephyr REQUIRED)

# Your application
add_executable(myapp
    main.cpp
)

target_link_libraries(myapp
    zephyr::zephyr
)

# Build a plugin
add_library(my_plugin SHARED
    my_plugin.cpp
)

target_link_libraries(my_plugin
    zephyr::zephyr
)
```

### **Compiling**

```bash
# Build your application
mkdir build && cd build
cmake .. 
make

# Run with plugin
./myapp
```

## 🎯 **Best Practices**

### **1. Error Handling**
```cpp
// Always check results
auto result = engine->execute_string(code);
if (!result) {
    // Handle error appropriately
    log_error("Script execution failed: " + result.error());
    return;
}
```

### **2. Resource Management**
```cpp
// Use RAII - smart pointers handle cleanup automatically
auto engine = zephyr::api::create_engine();
// Engine will be cleaned up automatically
```

### **3. Thread Safety**
```cpp
// Create separate engines for different threads
thread_local auto engine = zephyr::api::create_engine();

// Or use synchronization
std::mutex engine_mutex;
{
    std::lock_guard<std::mutex> lock(engine_mutex);
    auto result = engine->execute_string(code);
}
```

### **4. Performance**
```cpp
// Register functions once, use many times
engine->register_function("expensive_op", expensive_function);

// Reuse engines instead of creating new ones
static auto global_engine = zephyr::api::create_engine();
```

### **5. Plugin Development**
```cpp
// Always implement error checking
auto initialize(engine_t* engine) -> plugin_result_t override {
    if (!engine) {
        return plugin_result_t::error("Engine is null");
    }
    // ... setup code
    return plugin_result_t::success();
}

// Provide good metadata
auto get_plugin_info() const -> plugin_info_t override {
    plugin_info_t info;
    info.name = "my_plugin";
    info.description = "Detailed description of what this plugin does";
    info.author = "Your Name <your@email.com>";
    info.version = {1, 0, 0};
    info.license = "MIT";
    info.website = "https://github.com/yourname/plugin";
    return info;
}
```

## 🚀 **Real-World Examples**

### **Web Server Integration**

```cpp
#include "zephyr/zephyr.hpp"
#include <httplib.h>

class WebServer {
    zephyr::api::engine_t* m_engine;
    httplib::Server m_server;
    
public:
    WebServer() {
        m_engine = zephyr::api::create_engine().release();
        
        // Register web functions
        m_engine->register_function("http_get", [this](const std::string& path) {
            return handle_get(path);
        });
        
        // Setup routes
        m_server.Get("/api/(.+)", [this](const httplib::Request& req, httplib::Response& res) {
            std::string script = load_handler_script(req.matches[1]);
            auto result = m_engine->execute_string(script);
            if (result) {
                res.set_content(result.value()->to_string(), "application/json");
            } else {
                res.status = 500;
                res.set_content(result.error(), "text/plain");
            }
        });
    }
};
```

### **Game Scripting**

```cpp
class GameEngine {
    zephyr::api::engine_t* m_script_engine;
    
public:
    void init_scripting() {
        m_script_engine = zephyr::api::create_engine().release();
        
        // Register game functions
        m_script_engine->register_function("move_player", [this](double x, double y) {
            player.move(x, y);
        });
        
        m_script_engine->register_function("spawn_enemy", [this](const std::string& type) {
            spawn_enemy(type);
        });
        
        // Load game scripts
        m_script_engine->execute_file("game_logic.zephyr");
    }
    
    void update() {
        m_script_engine->call_function("on_update", get_delta_time());
    }
};
```

## ✅ **What's Working Right Now**

- ✅ **Engine Creation & Configuration**
- ✅ **Function Registration** (any C++ function signature)
- ✅ **Type Conversion** (int, double, string, bool, vectors, maps)
- ✅ **Global Variables** (get/set from C++)
- ✅ **Script Execution** (strings, files, expressions)
- ✅ **Plugin Loading** (dynamic shared libraries)
- ✅ **Error Handling** (comprehensive error info)
- ✅ **Memory Safety** (smart pointers throughout)
- ✅ **Performance Monitoring** (statistics and timing)

## 🎊 **Conclusion**

The Zephyr C++ API is **production-ready** and **fully integrated**! You can now:

1. **Embed Zephyr** in any C++ application
2. **Create plugins** in C++ for Zephyr scripts  
3. **Bridge C++ and Zephyr** seamlessly with automatic type conversion
4. **Load plugins dynamically** at runtime
5. **Handle errors** comprehensively
6. **Monitor performance** with built-in statistics

## 🎯 **Professional Include Structure**

Zephyr now uses a **professional namespace-based include structure**:

```cpp
#include "zephyr/zephyr.hpp"        // All-in-one convenience header
#include "zephyr/api/zephyr_api.hpp" // Main engine API
#include "zephyr/api/plugin_interface.hpp" // Plugin development
```

### **Benefits:**
- ✅ **Clear namespace** - All includes start with `zephyr/`
- ✅ **Professional structure** - Follows industry standards (like Qt, Boost)
- ✅ **IDE friendly** - Better IntelliSense and autocomplete
- ✅ **Convenient** - Use `zephyr/zephyr.hpp` for everything

The integration challenges have been solved, and the system is ready for real-world use! 🚀

**Start building with Zephyr today!**