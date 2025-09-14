# Zephyr C++ API Reference

Complete reference documentation for the Zephyr C++ API. This document provides detailed information about all classes, functions, types, and constants available in the API.

## Table of Contents

- [Headers and Namespaces](#headers-and-namespaces)
- [Core Classes](#core-classes)
- [Type System](#type-system)
- [Error Handling](#error-handling)
- [Plugin System](#plugin-system)
- [Utility Functions](#utility-functions)
- [Macros and Constants](#macros-and-constants)
- [Examples](#examples)

## Headers and Namespaces

### Primary Headers

```cpp
#include "zephyr/zephyr.hpp"              // All-in-one convenience header
#include "zephyr/api/zephyr_api.hpp"      // Main engine API
#include "zephyr/api/plugin_interface.hpp" // Plugin development
#include "zephyr/api/type_converter.hpp"  // Type conversion utilities
#include "zephyr/api/result.hpp"          // Error handling types
```

### Namespaces

- `zephyr::api` - Main API components
- `zephyr::quick` - Convenience functions for rapid prototyping

## Core Classes

### engine_t

The main interface for interacting with the Zephyr runtime.

```cpp
namespace zephyr::api {
    class engine_t {
    public:
        // Constructor/Destructor
        engine_t();
        ~engine_t();
        
        // Non-copyable but movable
        engine_t(const engine_t&) = delete;
        engine_t& operator=(const engine_t&) = delete;
        engine_t(engine_t&&) = default;
        engine_t& operator=(engine_t&&) = default;
    };
}
```

#### Script Execution

```cpp
// Execute Zephyr code from string
auto execute_string(const std::string& code, 
                   const std::string& context = "<embedded>") -> value_result_t;

// Execute Zephyr code from file
auto execute_file(const std::string& path) -> value_result_t;

// Start interactive REPL
auto start_repl() -> void;

// Evaluate expression and return result
auto evaluate_expression(const std::string& expression) -> value_result_t;

// Call Zephyr function by name
auto call_function(const std::string& function_name, 
                  const std::vector<value_t>& args) -> value_result_t;

// Template version for convenience
template<typename... Args>
auto call_function(const std::string& function_name, Args&&... args) -> value_result_t;
```

#### Global Variable Management

```cpp
// Set global variable (template version)
template<typename T>
auto set_global(const std::string& name, const T& value) -> void;

// Set global variable (value_t version)
auto set_global_value(const std::string& name, const value_t& value) -> void;

// Get global variable (template version)
template<typename T>
auto get_global(const std::string& name) -> std::optional<T>;

// Get global variable (value_t version)
auto get_global_value(const std::string& name) -> std::optional<value_t>;

// Check if global exists
auto has_global(const std::string& name) -> bool;

// Remove global variable
auto remove_global(const std::string& name) -> bool;

// List all globals
auto list_globals() -> std::vector<std::string>;
```

#### Function Registration

```cpp
// Register native function
auto register_function(const std::string& name, native_function_t func) -> void;

// Register function with automatic type conversion (template)
template<typename F>
auto register_function(const std::string& name, F&& func) -> void;

// Unregister function
auto unregister_function(const std::string& name) -> bool;

// Check if function is registered
auto has_function(const std::string& name) -> bool;

// List registered functions
auto list_functions() -> std::vector<std::string>;
```

#### Class Registration

```cpp
// Register native class
auto register_class(const std::string& name, std::shared_ptr<native_class_t> cls) -> void;

// Register C++ class with automatic wrapping (template)
template<typename T>
auto register_class(const std::string& name) -> std::shared_ptr<native_class_t>;

// Unregister class
auto unregister_class(const std::string& name) -> bool;

// Check if class is registered
auto has_class(const std::string& name) -> bool;

// Get registered class
auto get_class(const std::string& name) -> std::shared_ptr<native_class_t>;

// List registered classes
auto list_classes() -> std::vector<std::string>;
```

#### Module Management

```cpp
// Register native module
auto register_module(const std::string& name, std::shared_ptr<native_module_t> module) -> void;

// Create and register empty module
auto create_module(const std::string& name) -> std::shared_ptr<native_module_t>;

// Unregister module
auto unregister_module(const std::string& name) -> bool;

// Check if module is registered
auto has_module(const std::string& name) -> bool;

// Get registered module
auto get_module(const std::string& name) -> std::shared_ptr<native_module_t>;

// List registered modules
auto list_modules() -> std::vector<std::string>;
```

#### Plugin Management

```cpp
// Load plugin from shared library
auto load_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>>;

// Unload plugin
auto unload_plugin(const std::string& library_path) -> void_result_t;

// Check if plugin is loaded
auto is_plugin_loaded(const std::string& library_path) -> bool;

// Get loaded plugin
auto get_plugin(const std::string& library_path) -> std::shared_ptr<plugin_interface_t>;

// List loaded plugins
auto list_loaded_plugins() -> std::vector<plugin_info_t>;

// Add library search path
auto add_library_search_path(const std::string& path) -> void;

// Get library search paths
auto get_library_search_paths() -> std::vector<std::string>;

// Auto-load plugins from directory
auto scan_and_load_plugins(const std::string& directory, bool recursive = false) 
    -> std::vector<result_t<std::shared_ptr<plugin_interface_t>>>;
```

#### Configuration

```cpp
// Enable/disable debug mode
auto set_debug_mode(bool enabled) -> void;
auto is_debug_mode() const -> bool;

// Enable/disable async execution
auto set_async_enabled(bool enabled) -> void;
auto is_async_enabled() const -> bool;

// Add import path for Zephyr modules
auto add_import_path(const std::string& path) -> void;
auto get_import_paths() -> std::vector<std::string>;

// Set output redirection
auto set_stdout_handler(std::function<void(const std::string&)> handler) -> void;
auto set_stderr_handler(std::function<void(const std::string&)> handler) -> void;

// Engine metadata
auto set_name(const std::string& name) -> void;
auto get_name() const -> std::string;
auto set_version(const std::string& version) -> void;
auto get_version() const -> std::string;
```

#### Error Handling

```cpp
// Set global error handler
auto set_error_handler(error_handler_t handler) -> void;

// Get last error
auto get_last_error() -> std::optional<error_info_t>;

// Clear last error
auto clear_last_error() -> void;
```

#### Statistics and Monitoring

```cpp
struct engine_stats_t {
    size_t functions_registered = 0;
    size_t classes_registered = 0;
    size_t modules_registered = 0;
    size_t plugins_loaded = 0;
    size_t scripts_executed = 0;
    size_t expressions_evaluated = 0;
    std::chrono::milliseconds total_execution_time{0};
};

auto get_stats() const -> engine_stats_t;
auto reset_stats() -> void;
```

#### Advanced Features

```cpp
// Direct access to internal components (advanced users only)
auto get_interpreter() -> interpreter_t*;
auto get_runtime() -> runtime_t*;
auto get_plugin_loader() -> plugin_loader_t*;
```

## Type System

### Core Types

```cpp
namespace zephyr::api {
    using value_t = std::shared_ptr<object_t>;
    using native_function_t = std::function<value_result_t(const std::vector<value_t>&)>;
    using property_getter_t = std::function<value_result_t()>;
    using property_setter_t = std::function<void_result_t(const value_t&)>;
}
```

### Type Conversion Functions

#### Primary Conversion Functions

```cpp
// Convert C++ type to Zephyr value
template<typename T>
auto to_zephyr(const T& value) -> value_t;

// Convert Zephyr value to C++ type
template<typename T>
auto from_zephyr(const value_t& value) -> result_t<T>;

// Safe conversion (returns optional instead of throwing)
template<typename T>
auto try_from_zephyr(const value_t& value) -> std::optional<T>;

// Type checking
template<typename T>
auto is_zephyr_type(const value_t& value) -> bool;
```

#### Supported Type Conversions

| C++ Type | Zephyr Type | Conversion Notes |
|----------|-------------|------------------|
| `int`, `long`, `size_t` | `int` | Automatic range validation |
| `float`, `double` | `float` | Full precision preserved |
| `std::string`, `const char*` | `string` | UTF-8 encoding |
| `bool` | `bool` | Direct mapping |
| `std::vector<T>` | `list` | Recursive element conversion |
| `std::map<std::string, T>` | `dict` | String keys, converted values |
| `std::optional<T>` | `T` or `none` | Null safety |

#### Utility Functions

```cpp
// Get type name of Zephyr value
auto get_zephyr_type_name(const value_t& value) -> std::string;

// Get C++ type name
auto get_cpp_type_name(const std::type_info& type) -> std::string;

template<typename T>
auto get_cpp_type_name() -> std::string;

// Check if value is none/null
auto is_none(const value_t& value) -> bool;

// Create none value
auto make_none() -> value_t;
```

#### Batch Conversion Functions

```cpp
// Convert vector of values
auto to_zephyr_list(const std::vector<value_t>& values) -> value_t;
auto from_zephyr_list(const value_t& list) -> result_t<std::vector<value_t>>;

template<typename T>
auto to_zephyr_list(const std::vector<T>& values) -> value_t;

template<typename T>
auto from_zephyr_list(const value_t& list) -> result_t<std::vector<T>>;

// Convert map of values
auto to_zephyr_dict(const std::map<std::string, value_t>& values) -> value_t;
auto from_zephyr_dict(const value_t& dict) -> result_t<std::map<std::string, value_t>>;

template<typename T>
auto to_zephyr_dict(const std::map<std::string, T>& values) -> value_t;

template<typename T>
auto from_zephyr_dict(const value_t& dict) -> result_t<std::map<std::string, T>>;
```

### Function Wrapper System

#### Native Function Creation

```cpp
// Create native function from std::function
template<typename R, typename... Args>
auto make_native_function(std::function<R(Args...)> func, const std::string& name = "") -> native_function_t;

// Create native function from function pointer
template<typename R, typename... Args>
auto make_native_function(R(*func)(Args...), const std::string& name = "") -> native_function_t;

// Create native function from lambda/functor
template<typename F>
auto make_native_function(F&& func, const std::string& name = "") -> native_function_t;
```

#### Method Wrappers

```cpp
// Create method wrapper for instance methods
template<typename R, typename C, typename... Args>
auto make_native_method(R(C::*method)(Args...), std::shared_ptr<C> instance, const std::string& name = "") -> native_function_t;

// Create method wrapper for const methods
template<typename R, typename C, typename... Args>
auto make_native_method(R(C::*method)(Args...) const, std::shared_ptr<C> instance, const std::string& name = "") -> native_function_t;
```

#### Property Wrappers

```cpp
// Create property getter
template<typename T, typename C>
auto make_property_getter(T(C::*getter)() const, std::shared_ptr<C> instance, const std::string& name = "") -> property_getter_t;

template<typename T, typename C>
auto make_property_getter(T C::*field, std::shared_ptr<C> instance, const std::string& name = "") -> property_getter_t;

// Create property setter
template<typename T, typename C>
auto make_property_setter(void(C::*setter)(const T&), std::shared_ptr<C> instance, const std::string& name = "") -> property_setter_t;

template<typename T, typename C>
auto make_property_setter(T C::*field, std::shared_ptr<C> instance, const std::string& name = "") -> property_setter_t;
```

## Error Handling

### Result Types

```cpp
template<typename T>
class result_t {
public:
    // Constructors
    result_t(const T& value);                 // Success constructor
    result_t(const std::string& error);       // Error constructor
    result_t(const error_info_t& error);      // Detailed error constructor
    
    // Static factory methods
    static auto success(const T& value) -> result_t<T>;
    static auto error(const std::string& message) -> result_t<T>;
    static auto error(const error_info_t& info) -> result_t<T>;
    
    // State checking
    auto is_success() const -> bool;
    auto is_error() const -> bool;
    operator bool() const;                    // Implicit conversion to bool
    
    // Value access
    auto value() const -> const T&;           // Get value (throws if error)
    auto value_or(const T& default_value) const -> T;
    
    // Error access
    auto error() const -> std::string;
    auto error_info() const -> std::optional<error_info_t>;
    
    // Functional operations
    template<typename F>
    auto map(F&& func) const -> result_t<decltype(func(std::declval<T>()))>;
    
    template<typename F>
    auto and_then(F&& func) const -> decltype(func(std::declval<T>()));
};

// Specialization for void results
using void_result_t = result_t<void>;
using value_result_t = result_t<value_t>;
```

### Error Information

```cpp
enum class error_severity_t {
    info,
    warning,
    error,
    fatal
};

struct error_info_t {
    std::string message;
    std::string type = "RuntimeError";
    std::string plugin_name;
    int line = -1;
    int column = -1;
    std::string filename;
    std::string source_location;
    error_severity_t severity = error_severity_t::error;
    
    auto to_string() const -> std::string;
    auto severity_string() const -> std::string;
};

// Utility functions
auto create_error_info(const std::string& message, 
                      const std::string& type = "RuntimeError",
                      const std::string& plugin_name = "",
                      int line = -1, int column = -1,
                      const std::string& filename = "",
                      error_severity_t severity = error_severity_t::error) -> error_info_t;

auto create_plugin_error(const std::string& message,
                        const std::string& plugin_name = "",
                        const std::string& type = "PluginError") -> error_info_t;
```

### Exception Types

```cpp
class plugin_exception_t : public std::runtime_error {
public:
    plugin_exception_t(const error_info_t& info);
    plugin_exception_t(const std::string& message, const std::string& plugin_name = "");
    
    auto info() const -> const error_info_t&;
    
private:
    error_info_t m_info;
};
```

### Error Handler Types

```cpp
using error_handler_t = std::function<void(const error_info_t&)>;
```

## Plugin System

### Plugin Interface

```cpp
class plugin_interface_t {
public:
    virtual ~plugin_interface_t() = default;
    
    // Required methods
    virtual auto get_plugin_info() const -> plugin_info_t = 0;
    virtual auto initialize(engine_t* engine) -> plugin_result_t = 0;
    virtual auto finalize() -> void = 0;
    virtual auto create_module() -> std::shared_ptr<native_module_t> = 0;
    
    // Optional methods
    virtual auto get_symbol(const std::string& symbol_name) -> std::optional<value_t>;
    virtual auto get_all_symbols() -> std::optional<std::map<std::string, value_t>>;
    virtual auto handle_custom_import(const std::string& import_spec) -> std::optional<value_t>;
    virtual auto configure(const std::map<std::string, std::string>& config) -> plugin_result_t;
    virtual auto health_check() -> plugin_result_t;
};
```

### Plugin Information

```cpp
struct plugin_version_t {
    int major = 1;
    int minor = 0;
    int patch = 0;
    
    auto to_string() const -> std::string;
    auto is_compatible_with(const plugin_version_t& other) const -> bool;
    auto operator==(const plugin_version_t& other) const -> bool;
    auto operator<(const plugin_version_t& other) const -> bool;
};

struct plugin_info_t {
    std::string name;
    std::string description;
    std::string author;
    plugin_version_t version;
    plugin_version_t min_zephyr_version;
    std::vector<std::string> dependencies;
    std::string license;
    std::string website;
    
    auto to_string() const -> std::string;
};
```

### Plugin Result Type

```cpp
class plugin_result_t {
public:
    plugin_result_t();                        // Success constructor
    plugin_result_t(value_t value);          // Success with value
    plugin_result_t(const std::string& error); // Error constructor
    
    static auto success() -> plugin_result_t;
    static auto success(value_t value) -> plugin_result_t;
    static auto error(const std::string& message) -> plugin_result_t;
    
    auto is_success() const -> bool;
    auto is_error() const -> bool;
    auto value() const -> value_t;
    auto error() const -> std::string;
    
    operator bool() const;
};
```

### Native Module

```cpp
class native_module_t : public std::enable_shared_from_this<native_module_t> {
public:
    virtual ~native_module_t() = default;
    
    // Export functions
    auto export_function(const std::string& name, native_function_t func) -> void;
    auto export_internal_function(const std::string& name, native_function_t func) -> void;
    
    // Export classes
    auto export_class(const std::string& name, std::shared_ptr<native_class_t> cls) -> void;
    
    // Export constants/variables
    auto export_constant(const std::string& name, value_t value) -> void;
    auto export_variable(const std::string& name, value_t value) -> void;
    auto export_internal_constant(const std::string& name, value_t value) -> void;
    
    // Access methods
    auto has_function(const std::string& name) const -> bool;
    auto has_class(const std::string& name) const -> bool;
    auto has_constant(const std::string& name) const -> bool;
    auto has_variable(const std::string& name) const -> bool;
    auto has_symbol(const std::string& name) const -> bool;
    auto is_exported(const std::string& name) const -> bool;
    
    // Get symbols
    auto get_function(const std::string& name) const -> std::optional<native_function_t>;
    auto get_class(const std::string& name) const -> std::optional<std::shared_ptr<native_class_t>>;
    auto get_constant(const std::string& name) const -> std::optional<value_t>;
    auto get_variable(const std::string& name) const -> std::optional<value_t>;
    auto get_symbol(const std::string& name) const -> std::optional<value_t>;
    
    // List symbols
    auto get_exported_symbols() const -> std::vector<std::string>;
    auto get_all_symbols() const -> std::vector<std::string>;
    auto get_all_symbol_values() const -> std::map<std::string, value_t>;
    auto get_exported_symbol_values() const -> std::map<std::string, value_t>;
    
    // Module lifecycle
    virtual auto initialize() -> void_result_t;
    virtual auto finalize() -> void;
    
    // Create Zephyr module object
    auto create_module_object() -> value_result_t;
    
    // Metadata
    auto set_name(const std::string& name) -> void;
    auto get_name() const -> std::string;
    auto set_version(const std::string& version) -> void;
    auto get_version() const -> std::string;
    auto set_description(const std::string& description) -> void;
    auto get_description() const -> std::string;
    auto set_author(const std::string& author) -> void;
    auto get_author() const -> std::string;
    
    // Documentation
    auto set_function_doc(const std::string& func_name, const std::string& doc) -> void;
    auto get_function_doc(const std::string& func_name) const -> std::string;
};
```

### Native Class

```cpp
class native_class_t {
public:
    virtual ~native_class_t() = default;
    
    // Constructor
    auto set_constructor(native_function_t constructor) -> void;
    
    // Method registration
    auto add_method(const std::string& name, native_function_t method) -> void;
    auto add_static_method(const std::string& name, native_function_t method) -> void;
    
    // Property registration
    auto add_property_getter(const std::string& name, property_getter_t getter) -> void;
    auto add_property_setter(const std::string& name, property_setter_t setter) -> void;
    auto add_property(const std::string& name, property_getter_t getter, property_setter_t setter = nullptr) -> void;
    
    // Access methods
    auto has_method(const std::string& name) const -> bool;
    auto has_static_method(const std::string& name) const -> bool;
    auto has_property(const std::string& name) const -> bool;
    
    auto get_method(const std::string& name) const -> std::optional<native_function_t>;
    auto get_static_method(const std::string& name) const -> std::optional<native_function_t>;
    auto get_property_getter(const std::string& name) const -> std::optional<property_getter_t>;
    auto get_property_setter(const std::string& name) const -> std::optional<property_setter_t>;
    
    // Instance creation
    virtual auto create_instance(const std::vector<value_t>& args) -> value_result_t = 0;
    
    // Create Zephyr class object
    auto create_zephyr_class(const std::string& name) -> value_result_t;
    
    // Metadata
    auto set_name(const std::string& name) -> void;
    auto get_name() const -> std::string;
    auto set_description(const std::string& description) -> void;
    auto get_description() const -> std::string;
};
```

### C++ Class Wrapper Template

```cpp
template<typename T>
class cpp_class_wrapper_t : public native_class_t {
public:
    auto create_instance(const std::vector<value_t>& args) -> value_result_t override;
    
private:
    auto wrap_cpp_object(std::shared_ptr<T> obj) -> value_t;
};

// Factory function
template<typename T>
auto make_native_class(const std::string& name = "") -> std::shared_ptr<native_class_t>;
```

## Utility Functions

### Factory Functions

```cpp
// Create engine
auto create_engine() -> std::unique_ptr<engine_t>;

// Create engine with configuration
struct engine_config_t {
    std::string name = "Zephyr Engine";
    std::string version = "1.0.0";
    bool debug_mode = false;
    bool async_enabled = true;
    std::vector<std::string> import_paths;
    std::vector<std::string> library_search_paths;
    error_handler_t error_handler;
    std::function<void(const std::string&)> stdout_handler;
    std::function<void(const std::string&)> stderr_handler;
};

auto create_engine(const engine_config_t& config) -> std::unique_ptr<engine_t>;

// Create native module
auto make_native_module(const std::string& name = "") -> std::shared_ptr<native_module_t>;
```

### Global API Functions

```cpp
// Get default global engine instance
auto default_engine() -> engine_t&;

// Execute code using default engine
auto execute(const std::string& code, const std::string& context = "<global>") -> value_result_t;

// Register function in default engine
template<typename F>
auto register_global_function(const std::string& name, F&& func) -> void;

// Register class in default engine
template<typename T>
auto register_global_class(const std::string& name) -> std::shared_ptr<native_class_t>;

// Load plugin in default engine
auto load_global_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>>;
```

### Convenience Functions (zephyr::quick namespace)

```cpp
namespace zephyr::quick {
    using namespace api;
    
    // Ultra-quick engine creation and script execution
    inline auto run(const std::string& code) -> value_result_t;
}
```

## Macros and Constants

### Plugin Export Macros

```cpp
// Export plugin class
#define ZEPHYR_PLUGIN(plugin_class) \
    ZEPHYR_PLUGIN_API_VERSION() \
    ZEPHYR_PLUGIN_CREATOR(plugin_class)

// Internal macros (used by ZEPHYR_PLUGIN)
#define ZEPHYR_PLUGIN_API_VERSION()
#define ZEPHYR_PLUGIN_CREATOR(plugin_class)
```

### Function Registration Macros

```cpp
// Register function with automatic naming
#define ZEPHYR_FUNCTION(name, func) \
    export_function(name, zephyr::api::make_native_function(func, name))

// Register method
#define ZEPHYR_METHOD(name, method, instance) \
    add_method(name, zephyr::api::make_native_method(method, instance, name))

// Register property
#define ZEPHYR_PROPERTY(name, getter, setter, instance) \
    add_property(name, \
        zephyr::api::make_property_getter(getter, instance, name "_getter"), \
        zephyr::api::make_property_setter(setter, instance, name "_setter"))

// Register read-only property
#define ZEPHYR_READONLY_PROPERTY(name, getter, instance) \
    add_property_getter(name, zephyr::api::make_property_getter(getter, instance, name "_getter"))
```

### Utility Macros

```cpp
// Register a function with the default engine
#define ZEPHYR_REGISTER_FUNCTION(name, func) \
    zephyr::api::register_global_function(name, func)

// Register a class with the default engine
#define ZEPHYR_REGISTER_CLASS(name, type) \
    zephyr::api::register_global_class<type>(name)

// Execute Zephyr code and check for errors
#define ZEPHYR_EXECUTE_OR_THROW(code) \
    do { \
        auto result = zephyr::api::execute(code); \
        if (!result) { \
            throw zephyr::api::plugin_exception_t(result.error_info().value_or( \
                zephyr::api::create_error_info(result.error()))); \
        } \
    } while(0)

// Safely execute Zephyr code
#define ZEPHYR_EXECUTE_SAFE(code, error_var) \
    [&]() -> bool { \
        auto result = zephyr::api::execute(code); \
        if (!result) { \
            error_var = result.error(); \
            return false; \
        } \
        return true; \
    }()
```

### Version Constants

```cpp
#define ZEPHYR_VERSION_MAJOR 1
#define ZEPHYR_VERSION_MINOR 0
#define ZEPHYR_VERSION_PATCH 0
#define ZEPHYR_VERSION_STRING "1.0.0"

// Plugin API version
constexpr plugin_version_t CURRENT_PLUGIN_API_VERSION{1, 0, 0};
```

### Platform-Specific Macros

```cpp
#ifdef _WIN32
    #define ZEPHYR_EXPORT __declspec(dllexport)
#else
    #define ZEPHYR_EXPORT __attribute__((visibility("default")))
#endif
```

## Examples

### Basic Engine Usage

```cpp
#include "zephyr/zephyr.hpp"

int main() {
    // Create engine
    auto engine = zephyr::api::create_engine();
    
    // Register function
    engine->register_function("add", [](double a, double b) -> double {
        return a + b;
    });
    
    // Execute script
    auto result = engine->execute_string("return add(5.0, 3.0)");
    if (result) {
        std::cout << "Result: " << result.value()->to_string() << std::endl;
    }
    
    return 0;
}
```

### Plugin Development

```cpp
#include "zephyr/zephyr.hpp"

class math_plugin_t : public zephyr::api::plugin_interface_t {
public:
    auto get_plugin_info() const -> zephyr::api::plugin_info_t override {
        zephyr::api::plugin_info_t info;
        info.name = "math_utils";
        info.version = {1, 0, 0};
        info.description = "Mathematical utility functions";
        return info;
    }
    
    auto initialize(zephyr::api::engine_t* engine) -> zephyr