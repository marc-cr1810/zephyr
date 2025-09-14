# Getting Started with Zephyr C++ API

Welcome to the Zephyr C++ API! This comprehensive guide will walk you through everything you need to know to start embedding Zephyr in your C++ applications and creating powerful plugins.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Your First Zephyr Program](#your-first-zephyr-program)
- [Basic Concepts](#basic-concepts)
- [Step-by-Step Tutorial](#step-by-step-tutorial)
- [Common Patterns](#common-patterns)
- [Error Handling](#error-handling)
- [Building Your Project](#building-your-project)
- [Next Steps](#next-steps)

## Prerequisites

Before you begin, ensure you have:

- **C++17 or later** compatible compiler
- **CMake 3.12+** for building
- **Basic C++ knowledge** (smart pointers, lambdas, templates)
- **Zephyr library** installed on your system

### Supported Compilers

| Platform | Recommended Compiler | Minimum Version |
|----------|---------------------|-----------------|
| Windows  | MSVC | 2019 (16.0) |
| Windows  | MinGW-w64 | GCC 8.0 |
| Linux    | GCC | 8.0 |
| Linux    | Clang | 8.0 |
| macOS    | Clang | 10.0 |

## Installation

### Option 1: Using CMake (Recommended)

If Zephyr is installed system-wide:

```cmake
find_package(Zephyr REQUIRED)
target_link_libraries(your_target zephyr::zephyr)
```

### Option 2: Manual Setup

1. Ensure Zephyr headers are in your include path
2. Link against the Zephyr library
3. Add the include directory to your compiler flags

```bash
g++ -std=c++17 -I/path/to/zephyr/include your_app.cpp -lzephyr
```

### Verify Installation

Create a simple test file to verify everything works:

```cpp
#include "zephyr/zephyr.hpp"
#include <iostream>

int main() {
    std::cout << "Zephyr version: " << ZEPHYR_VERSION_STRING << std::endl;
    auto engine = zephyr::api::create_engine();
    std::cout << "Engine created successfully!" << std::endl;
    return 0;
}
```

## Your First Zephyr Program

Let's start with the classic "Hello, World!" example:

```cpp
#include "zephyr/zephyr.hpp"
#include <iostream>

int main() {
    // Create a Zephyr engine
    auto engine = zephyr::api::create_engine();
    
    // Execute Zephyr code
    auto result = engine->execute_string(R"(
        print("Hello, World from Zephyr!")
        return "Success"
    )");
    
    // Check the result
    if (result) {
        std::cout << "Zephyr says: " << result.value()->to_string() << std::endl;
    } else {
        std::cout << "Error: " << result.error() << std::endl;
    }
    
    return 0;
}
```

**Expected Output:**
```
Hello, World from Zephyr!
Zephyr says: Success
```

## Basic Concepts

### 1. Engine (`zephyr::api::engine_t`)

The engine is your main interface to Zephyr. It:
- Executes Zephyr scripts
- Manages the Zephyr runtime
- Handles function registration
- Manages global variables
- Loads and manages plugins

```cpp
auto engine = zephyr::api::create_engine();
```

### 2. Results (`result_t<T>`)

All potentially failing operations return a `result_t<T>`:

```cpp
auto result = engine->execute_string("return 42");
if (result) {
    // Success - use result.value()
    auto value = result.value();
} else {
    // Error - use result.error()
    std::cout << "Error: " << result.error() << std::endl;
}
```

### 3. Type Conversion

Automatic conversion between C++ and Zephyr types:

```cpp
// C++ -> Zephyr
auto zephyr_int = zephyr::api::to_zephyr(42);
auto zephyr_string = zephyr::api::to_zephyr(std::string("Hello"));

// Zephyr -> C++
auto cpp_int = zephyr::api::from_zephyr<int>(zephyr_value);
auto cpp_string = zephyr::api::from_zephyr<std::string>(zephyr_value);
```

### 4. Function Registration

Register C++ functions for Zephyr to call:

```cpp
engine->register_function("add", [](double a, double b) -> double {
    return a + b;
});
```

## Step-by-Step Tutorial

### Step 1: Create and Configure Engine

```cpp
#include "zephyr/zephyr.hpp"
#include <iostream>

int main() {
    // Create engine with custom configuration
    auto config = zephyr::api::engine_config_t{};
    config.name = "My Application";
    config.debug_mode = true;
    config.import_paths = {"./scripts", "./plugins"};
    
    auto engine = zephyr::api::create_engine(config);
    
    std::cout << "Engine: " << engine->get_name() << std::endl;
    std::cout << "Debug mode: " << (engine->is_debug_mode() ? "on" : "off") << std::endl;
```

### Step 2: Register Functions

```cpp
    // Register a simple function
    engine->register_function("multiply", [](double a, double b) -> double {
        return a * b;
    });
    
    // Register a function with different types
    engine->register_function("greet", [](const std::string& name) -> std::string {
        return "Hello, " + name + "!";
    });
    
    // Register a function with complex logic
    engine->register_function("fibonacci", [](int n) -> int {
        if (n <= 1) return n;
        int a = 0, b = 1;
        for (int i = 2; i <= n; ++i) {
            int temp = a + b;
            a = b;
            b = temp;
        }
        return b;
    });
    
    std::cout << "Registered " << engine->list_functions().size() << " functions" << std::endl;
```

### Step 3: Set Global Variables

```cpp
    // Set various types of global variables
    engine->set_global("app_version", std::string("1.0.0"));
    engine->set_global("max_items", 100);
    engine->set_global("pi", 3.14159);
    engine->set_global("debug", true);
    
    // Set complex types
    std::vector<std::string> features = {"scripting", "plugins", "type-safety"};
    engine->set_global("features", features);
    
    std::cout << "Set " << engine->list_globals().size() << " global variables" << std::endl;
```

### Step 4: Execute Scripts

```cpp
    // Execute a simple script
    auto result1 = engine->execute_string(R"(
        print("App version: " + app_version)
        print("Max items: " + str(max_items))
        
        # Use registered functions
        product = multiply(6, 7)
        greeting = greet("Developer")
        
        print(greeting)
        print("6 * 7 = " + str(product))
        
        return product
    )");
    
    if (result1) {
        std::cout << "Script result: " << result1.value()->to_string() << std::endl;
    } else {
        std::cout << "Script error: " << result1.error() << std::endl;
    }
```

### Step 5: Execute from File

```cpp
    // Create a test script file
    std::ofstream script_file("test_script.zephyr");
    script_file << R"(
        print("Running from file!")
        
        # Calculate fibonacci numbers
        numbers = []
        for i in range(10):
            fib = fibonacci(i)
            numbers.append(fib)
        
        print("First 10 Fibonacci numbers: " + str(numbers))
        
        # Use global variables
        print("Available features:")
        for feature in features:
            print("  - " + feature)
        
        return "File execution complete"
    )";
    script_file.close();
    
    // Execute the file
    auto result2 = engine->execute_file("test_script.zephyr");
    
    if (result2) {
        std::cout << "File result: " << result2.value()->to_string() << std::endl;
    } else {
        std::cout << "File error: " << result2.error() << std::endl;
    }
```

### Step 6: Interact with Results

```cpp
    // Get modified global variables
    auto modified_features = engine->get_global<std::vector<std::string>>("features");
    if (modified_features) {
        std::cout << "Features from Zephyr:" << std::endl;
        for (const auto& feature : modified_features.value()) {
            std::cout << "  - " << feature << std::endl;
        }
    }
    
    // Call Zephyr functions from C++
    auto call_result = engine->call_function("multiply", 8.0, 9.0);
    if (call_result) {
        std::cout << "8 * 9 = " << call_result.value()->to_string() << std::endl;
    }
    
    return 0;
}
```

## Common Patterns

### Pattern 1: Configuration System

```cpp
class ConfigManager {
    std::unique_ptr<zephyr::api::engine_t> m_engine;
    
public:
    ConfigManager() : m_engine(zephyr::api::create_engine()) {
        // Register configuration functions
        m_engine->register_function("set_window_size", [this](int w, int h) {
            set_window_size(w, h);
        });
        
        m_engine->register_function("set_theme", [this](const std::string& theme) {
            set_theme(theme);
        });
    }
    
    void load_config(const std::string& config_file) {
        auto result = m_engine->execute_file(config_file);
        if (!result) {
            throw std::runtime_error("Failed to load config: " + result.error());
        }
    }
    
private:
    void set_window_size(int width, int height) { /* implementation */ }
    void set_theme(const std::string& theme) { /* implementation */ }
};
```

### Pattern 2: Plugin Architecture

```cpp
class Application {
    std::unique_ptr<zephyr::api::engine_t> m_engine;
    
public:
    Application() : m_engine(zephyr::api::create_engine()) {
        // Set up plugin search paths
        m_engine->add_library_search_path("./plugins");
        m_engine->add_library_search_path("/usr/local/lib/myapp/plugins");
    }
    
    void load_plugins() {
        auto results = m_engine->scan_and_load_plugins("./plugins", true);
        
        for (const auto& result : results) {
            if (result) {
                auto plugin = result.value();
                auto info = plugin->get_plugin_info();
                std::cout << "Loaded plugin: " << info.name 
                         << " v" << info.version.to_string() << std::endl;
            } else {
                std::cout << "Failed to load plugin: " << result.error() << std::endl;
            }
        }
    }
    
    void run_script(const std::string& script) {
        auto result = m_engine->execute_string(script);
        if (!result) {
            handle_script_error(result.error());
        }
    }
    
private:
    void handle_script_error(const std::string& error) {
        std::cerr << "Script error: " << error << std::endl;
    }
};
```

### Pattern 3: Data Processing Pipeline

```cpp
class DataProcessor {
    std::unique_ptr<zephyr::api::engine_t> m_engine;
    
public:
    DataProcessor() : m_engine(zephyr::api::create_engine()) {
        // Register data processing functions
        m_engine->register_function("load_data", [this](const std::string& path) -> std::vector<double> {
            return load_data_file(path);
        });
        
        m_engine->register_function("save_results", [this](const std::vector<double>& results, const std::string& path) {
            save_results_file(results, path);
        });
    }
    
    void process(const std::string& script_path, const std::string& input_path, const std::string& output_path) {
        // Set up input/output paths
        m_engine->set_global("input_path", input_path);
        m_engine->set_global("output_path", output_path);
        
        // Execute processing script
        auto result = m_engine->execute_file(script_path);
        if (!result) {
            throw std::runtime_error("Processing failed: " + result.error());
        }
    }
    
private:
    std::vector<double> load_data_file(const std::string& path) { /* implementation */ }
    void save_results_file(const std::vector<double>& results, const std::string& path) { /* implementation */ }
};
```

### Pattern 4: Interactive REPL

```cpp
class InteractiveShell {
    std::unique_ptr<zephyr::api::engine_t> m_engine;
    
public:
    InteractiveShell() : m_engine(zephyr::api::create_engine()) {
        setup_commands();
    }
    
    void run() {
        std::string line;
        std::cout << "Zephyr Interactive Shell\nType 'exit' to quit.\n\n";
        
        while (true) {
            std::cout << ">>> ";
            std::getline(std::cin, line);
            
            if (line == "exit" || line == "quit") {
                break;
            }
            
            if (line.empty()) {
                continue;
            }
            
            auto result = m_engine->execute_string(line);
            if (result) {
                auto value = result.value();
                if (value->to_string() != "none") {
                    std::cout << value->to_string() << std::endl;
                }
            } else {
                std::cout << "Error: " << result.error() << std::endl;
            }
        }
    }
    
private:
    void setup_commands() {
        m_engine->register_function("help", []() -> std::string {
            return "Available commands: help, exit, quit";
        });
        
        m_engine->register_function("version", []() -> std::string {
            return ZEPHYR_VERSION_STRING;
        });
    }
};
```

## Error Handling

### Basic Error Handling

```cpp
auto result = engine->execute_string("some_code");
if (!result) {
    std::cout << "Error: " << result.error() << std::endl;
    return 1;
}
```

### Detailed Error Information

```cpp
auto result = engine->execute_string("invalid syntax here");
if (!result) {
    auto last_error = engine->get_last_error();
    if (last_error) {
        std::cout << "Error details: " << last_error->to_string() << std::endl;
        std::cout << "Type: " << last_error->type << std::endl;
        std::cout << "Line: " << last_error->line << std::endl;
        std::cout << "Column: " << last_error->column << std::endl;
    }
}
```

### Custom Error Handling

```cpp
engine->set_error_handler([](const zephyr::api::error_info_t& error) {
    std::cerr << "[ERROR] " << error.message << std::endl;
    if (error.line >= 0) {
        std::cerr << "  at line " << error.line;
        if (error.column >= 0) {
            std::cerr << ", column " << error.column;
        }
        std::cerr << std::endl;
    }
});
```

### Exception Safety

```cpp
try {
    auto engine = zephyr::api::create_engine();
    
    engine->register_function("may_throw", []() -> int {
        throw std::runtime_error("Something went wrong");
        return 42;
    });
    
    auto result = engine->execute_string("return may_throw()");
    if (!result) {
        std::cout << "Caught exception: " << result.error() << std::endl;
    }
} catch (const std::exception& e) {
    std::cout << "C++ exception: " << e.what() << std::endl;
}
```

## Building Your Project

### CMakeLists.txt Example

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyZephyrApp VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Zephyr
find_package(Zephyr REQUIRED)

# Create your executable
add_executable(myapp
    main.cpp
    config_manager.cpp
    data_processor.cpp
)

# Link with Zephyr
target_link_libraries(myapp
    zephyr::zephyr
)

# Optional: Set include directories if needed
target_include_directories(myapp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

### Manual Compilation

```bash
# Linux/macOS with GCC
g++ -std=c++17 -I/usr/local/include main.cpp -L/usr/local/lib -lzephyr -pthread -o myapp

# Linux/macOS with Clang
clang++ -std=c++17 -I/usr/local/include main.cpp -L/usr/local/lib -lzephyr -pthread -o myapp

# Windows with MSVC
cl /EHsc /std:c++17 /I"C:\zephyr\include" main.cpp /link "C:\zephyr\lib\zephyr.lib"
```

### pkg-config Support

If Zephyr provides pkg-config files:

```bash
g++ -std=c++17 $(pkg-config --cflags zephyr) main.cpp $(pkg-config --libs zephyr) -o myapp
```

## Next Steps

### 1. Explore Advanced Features

- **Plugin Development**: Learn to create dynamic plugins
- **Custom Type Conversion**: Add support for your own types  
- **Performance Optimization**: Fine-tune for your use case
- **Async Programming**: Use Zephyr's async capabilities

### 2. Read More Documentation

- [C++ API User Guide](cpp_api_guide.md) - Comprehensive API documentation
- [Plugin Development Guide](plugin_development.md) - Create dynamic plugins
- [API Reference](cpp_api_reference.md) - Complete API reference
- [Best Practices](cpp_best_practices.md) - Tips and recommendations

### 3. Study Examples

Check out the `examples/` directory for:
- `quick_start.cpp` - Professional namespace demonstration
- `api_demo.cpp` - Comprehensive feature showcase
- `embedding_example.cpp` - Real-world embedding scenarios
- `simple_plugin.cpp` - Plugin development example

### 4. Join the Community

- **GitHub**: Contribute, report issues, request features
- **Documentation**: Help improve documentation and examples
- **Testing**: Help test on different platforms and use cases

### 5. Performance Considerations

- **Engine Reuse**: Reuse engines instead of creating new ones
- **Function Caching**: Register functions once, use many times
- **Memory Management**: Let smart pointers handle cleanup
- **Error Handling**: Use result types instead of exceptions where possible

---

**Congratulations!** You're now ready to start building amazing applications with the Zephyr C++ API. The combination of C++ performance and Zephyr flexibility opens up endless possibilities for your projects.

**Having issues?** Check the [FAQ](faq.md) or [file an issue](https://github.com/zephyr/zephyr/issues) on GitHub.