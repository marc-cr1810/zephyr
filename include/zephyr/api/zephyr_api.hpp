#pragma once

#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/api/native_function.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/api/dynamic_loader.hpp"
#include "zephyr/api/result.hpp"
#include "zephyr/interpreter.hpp"
#include "zephyr/runtime.hpp"
#include "zephyr/module_loader.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>

namespace zephyr::api {

// =============================================================================
// Main Engine API
// =============================================================================

class engine_t {
public:
    engine_t();
    ~engine_t();
    
    // Non-copyable but movable
    engine_t(const engine_t&) = delete;
    engine_t& operator=(const engine_t&) = delete;
    engine_t(engine_t&&) = default;
    engine_t& operator=(engine_t&&) = default;
    
    // =============================================================================
    // Script Execution
    // =============================================================================
    
    // Execute Zephyr code from string
    auto execute_string(const std::string& code, 
                       const std::string& context = "<embedded>") -> value_result_t;
    
    // Execute Zephyr code from file
    auto execute_file(const std::string& path) -> value_result_t;
    
    // Start interactive REPL
    auto start_repl() -> void;
    
    // =============================================================================
    // Global Variable Management
    // =============================================================================
    
    // Set global variable
    template<typename T>
    auto set_global(const std::string& name, const T& value) -> void {
        set_global_value(name, to_zephyr(value));
    }
    
    auto set_global_value(const std::string& name, const value_t& value) -> void;
    
    // Get global variable
    template<typename T>
    auto get_global(const std::string& name) -> std::optional<T> {
        auto value = get_global_value(name);
        if (!value) return std::nullopt;
        
        auto result = try_from_zephyr<T>(value.value());
        return result;
    }
    
    auto get_global_value(const std::string& name) -> std::optional<value_t>;
    
    // Check if global exists
    auto has_global(const std::string& name) -> bool;
    
    // Remove global variable
    auto remove_global(const std::string& name) -> bool;
    
    // List all globals
    auto list_globals() -> std::vector<std::string>;
    
    // =============================================================================
    // Function Registration
    // =============================================================================
    
    // Register native function
    auto register_function(const std::string& name, native_function_t func) -> void;
    
    // Register function with automatic type conversion
    template<typename F>
    auto register_function(const std::string& name, F&& func) -> void {
        register_function(name, make_native_function(std::forward<F>(func), name));
    }
    
    // Unregister function
    auto unregister_function(const std::string& name) -> bool;
    
    // Check if function is registered
    auto has_function(const std::string& name) -> bool;
    
    // List registered functions
    auto list_functions() -> std::vector<std::string>;
    
    // =============================================================================
    // Class Registration
    // =============================================================================
    
    // Register native class
    auto register_class(const std::string& name, std::shared_ptr<native_class_t> cls) -> void;
    
    // Register C++ class with automatic wrapping
    template<typename T>
    auto register_class(const std::string& name) -> std::shared_ptr<native_class_t> {
        auto cls = make_native_class<T>(name);
        register_class(name, cls);
        return cls;
    }
    
    // Unregister class
    auto unregister_class(const std::string& name) -> bool;
    
    // Check if class is registered
    auto has_class(const std::string& name) -> bool;
    
    // Get registered class
    auto get_class(const std::string& name) -> std::shared_ptr<native_class_t>;
    
    // List registered classes
    auto list_classes() -> std::vector<std::string>;
    
    // =============================================================================
    // Module Registration
    // =============================================================================
    
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
    
    // =============================================================================
    // Dynamic Plugin Loading
    // =============================================================================
    
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
    auto scan_and_load_plugins(const std::string& directory, bool recursive = false) -> std::vector<result_t<std::shared_ptr<plugin_interface_t>>>;
    
    // =============================================================================
    // Error Handling
    // =============================================================================
    
    // Set global error handler
    auto set_error_handler(error_handler_t handler) -> void;
    
    // Get last error
    auto get_last_error() -> std::optional<error_info_t>;
    
    // Clear last error
    auto clear_last_error() -> void;
    
    // =============================================================================
    // Configuration
    // =============================================================================
    
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
    
    // =============================================================================
    // Advanced Features
    // =============================================================================
    
    // Direct access to internal components (advanced users only)
    auto get_interpreter() -> interpreter_t*;
    auto get_runtime() -> runtime_t*;
    auto get_plugin_loader() -> plugin_loader_t*;
    
    // Evaluate expression and return result
    auto evaluate_expression(const std::string& expression) -> value_result_t;
    
    // Call Zephyr function by name
    auto call_function(const std::string& function_name, 
                      const std::vector<value_t>& args) -> value_result_t;
    
    template<typename... Args>
    auto call_function(const std::string& function_name, Args&&... args) -> value_result_t {
        std::vector<value_t> zephyr_args = {to_zephyr(args)...};
        return call_function(function_name, zephyr_args);
    }
    
    // Get/set engine metadata
    auto set_name(const std::string& name) -> void;
    auto get_name() const -> std::string;
    
    auto set_version(const std::string& version) -> void;
    auto get_version() const -> std::string;
    
    // Statistics and performance
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

private:
    // Internal implementation
    std::unique_ptr<runtime_t> m_runtime;
    std::unique_ptr<interpreter_t> m_interpreter;
    std::unique_ptr<plugin_loader_t> m_plugin_loader;
    
    // Configuration
    std::string m_name = "Zephyr Engine";
    std::string m_version = "1.0.0";
    bool m_debug_mode = false;
    bool m_async_enabled = true;
    
    // Error handling
    error_handler_t m_error_handler;
    std::optional<error_info_t> m_last_error;
    
    // Output redirection
    std::function<void(const std::string&)> m_stdout_handler;
    std::function<void(const std::string&)> m_stderr_handler;
    
    // Registry
    std::map<std::string, native_function_t> m_registered_functions;
    std::map<std::string, std::shared_ptr<native_class_t>> m_registered_classes;
    std::map<std::string, std::shared_ptr<native_module_t>> m_registered_modules;
    
    // Global variables storage
    std::map<std::string, value_t> m_global_variables;
    
    // Search paths
    std::vector<std::string> m_import_paths;
    
    // Statistics
    mutable engine_stats_t m_stats;
    mutable std::mutex m_stats_mutex;
    
    // Helper methods
    auto initialize_engine() -> void;
    auto setup_builtin_modules() -> void;
    auto handle_error(const error_info_t& error) -> void;
    auto update_stats_execution() -> void;
    auto update_stats_evaluation() -> void;
};

// =============================================================================
// Convenience Factory Functions
// =============================================================================

// Create a new engine instance
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

// =============================================================================
// Global API Functions (for simple use cases)
// =============================================================================

// Get default global engine instance
auto default_engine() -> engine_t&;

// Execute code using default engine
auto execute(const std::string& code, const std::string& context = "<global>") -> value_result_t;

// Register function in default engine
template<typename F>
auto register_global_function(const std::string& name, F&& func) -> void {
    default_engine().register_function(name, std::forward<F>(func));
}

// Register class in default engine
template<typename T>
auto register_global_class(const std::string& name) -> std::shared_ptr<native_class_t> {
    return default_engine().register_class<T>(name);
}

// Load plugin in default engine
auto load_global_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>>;

// =============================================================================
// Utility Macros for Easy Integration
// =============================================================================

// Macro to register a function with the default engine
#define ZEPHYR_REGISTER_FUNCTION(name, func) \
    zephyr::api::register_global_function(name, func)

// Macro to register a class with the default engine
#define ZEPHYR_REGISTER_CLASS(name, type) \
    zephyr::api::register_global_class<type>(name)

// Macro to execute Zephyr code and check for errors
#define ZEPHYR_EXECUTE_OR_THROW(code) \
    do { \
        auto result = zephyr::api::execute(code); \
        if (!result) { \
            throw zephyr::api::plugin_exception_t(result.error_info().value_or( \
                zephyr::api::create_error_info(result.error()))); \
        } \
    } while(0)

// Macro to safely execute Zephyr code
#define ZEPHYR_EXECUTE_SAFE(code, error_var) \
    [&]() -> bool { \
        auto result = zephyr::api::execute(code); \
        if (!result) { \
            error_var = result.error(); \
            return false; \
        } \
        return true; \
    }()

} // namespace zephyr::api

// =============================================================================
// Global Convenience Functions (in zephyr namespace)
// =============================================================================

namespace zephyr {

// Convenience aliases
using api_engine_t = api::engine_t;
using api_result_t = api::value_result_t;

// Quick access functions
inline auto create_api_engine() { return api::create_engine(); }
inline auto execute_code(const std::string& code) { return api::execute(code); }

} // namespace zephyr