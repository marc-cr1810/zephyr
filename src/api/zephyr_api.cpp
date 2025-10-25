#include "zephyr/api/zephyr_api.hpp"
#include "zephyr/api/dynamic_loader.hpp"
#include "zephyr/runtime.hpp"
#include "zephyr/interpreter.hpp"
#include "zephyr/module_loader.hpp"
#include "zephyr/parser.hpp"
#include "zephyr/lexer.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/objects/builtin_function_object.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <set>

namespace zephyr::api {

// =============================================================================
// Function Adapter for Builtin Integration
// =============================================================================

// Adapter class to bridge native_function_t with builtin_function_object_t
class native_function_adapter_t : public builtin_function_object_t {
private:
    native_function_t m_native_func;
    
public:
    native_function_adapter_t(const native_function_t& func, const std::string& name) 
        : builtin_function_object_t(nullptr, name), m_native_func(func) {}
    
    // Override the call method to use our native function
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override {
        auto result = m_native_func(args);
        if (result) {
            return result.value();
        } else {
            throw std::runtime_error(result.error());
        }
    }
};

// =============================================================================
// Global Engine Instance
// =============================================================================

static std::unique_ptr<engine_t> g_default_engine;
static std::mutex g_default_engine_mutex;

// =============================================================================
// Engine Implementation
// =============================================================================

engine_t::engine_t()
    : m_debug_mode(false)
    , m_async_enabled(true) {
    initialize_engine();
}

engine_t::~engine_t() {
    // Cleanup is handled by destructors
}

auto engine_t::initialize_engine() -> void {
    // Create core components
    m_runtime = std::make_unique<runtime_t>();
    m_interpreter = std::make_unique<interpreter_t>();
    m_plugin_loader = std::make_unique<plugin_loader_t>(this);
    
    // Connect interpreter to runtime's module loader
    if (m_runtime && m_runtime->get_module_loader()) {
        m_interpreter->set_module_loader(m_runtime->get_module_loader());
        // Set engine reference in module loader for native module support
        m_runtime->get_module_loader()->set_engine(this);
    }
    
    // Setup builtin modules
    setup_builtin_modules();
    
    // Reset statistics
    reset_stats();
}

auto engine_t::setup_builtin_modules() -> void {
    // This would setup built-in modules
    // For now, we'll just initialize the basics
}

auto engine_t::execute_string(const std::string& code, const std::string& context) -> value_result_t {
    try {
        update_stats_execution();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Parse the code
        lexer_t lexer(code);
        parser_t parser(lexer);
        auto program = parser.parse();
        
        if (!program) {
            return value_result_t::error("Failed to parse code: syntax error");
        }
        
        // Execute the program
        m_interpreter->interpret(*program);
        auto result = m_interpreter->current_result();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::lock_guard<std::mutex> lock(m_stats_mutex);
        m_stats.total_execution_time += duration;
        
        return value_result_t::success(result ? result : std::make_shared<none_object_t>());
        
    } catch (const std::exception& e) {
        auto error = create_error_info(e.what(), "ExecutionError", "", -1, -1, context);
        handle_error(error);
        return value_result_t::error(error);
    }
}

auto engine_t::execute_file(const std::string& path) -> value_result_t {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return value_result_t::error("Failed to open file: " + path);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        return execute_string(content, path);
        
    } catch (const std::exception& e) {
        auto error = create_error_info(e.what(), "FileError", "", -1, -1, path);
        handle_error(error);
        return value_result_t::error(error);
    }
}

auto engine_t::start_repl() -> void {
    if (m_runtime) {
        m_runtime->start_repl();
    }
}

auto engine_t::set_global_value(const std::string& name, const value_t& value) -> void {
    if (m_interpreter) {
        // Use the new interpreter API to set global variables directly
        m_interpreter->set_global_variable(name, value);
        
        // Also remove from local registry to avoid duplication
        m_global_variables.erase(name);
    } else {
        // Fallback to local registry if interpreter is not available
        m_global_variables[name] = value;
    }
}

auto engine_t::get_global_value(const std::string& name) -> std::optional<value_t> {
    if (m_interpreter) {
        try {
            // Check interpreter's global scope first (primary source of truth)
            const auto& global_scope = m_interpreter->get_global_scope();
            auto it = global_scope.find(name);
            if (it != global_scope.end()) {
                return it->second;
            }
            
            // Fallback to local registry for API-set values that haven't been propagated yet
            auto local_it = m_global_variables.find(name);
            if (local_it != m_global_variables.end()) {
                return local_it->second;
            }
            
            return std::nullopt;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

auto engine_t::has_global(const std::string& name) -> bool {
    if (m_interpreter && m_interpreter->has_global_variable(name)) {
        return true;
    }
    
    // Also check local registry
    return m_global_variables.find(name) != m_global_variables.end();
}

auto engine_t::remove_global(const std::string& name) -> bool {
    bool removed = false;
    
    if (m_interpreter) {
        // Use the new interpreter API to remove global variables directly
        removed = m_interpreter->remove_global_variable(name);
    }
    
    // Also remove from local registry if it exists
    auto it = m_global_variables.find(name);
    if (it != m_global_variables.end()) {
        m_global_variables.erase(it);
        removed = true;
    }
    
    return removed;
}

auto engine_t::list_globals() -> std::vector<std::string> {
    std::vector<std::string> globals;
    std::set<std::string> unique_names; // Prevent duplicates
    
    // Add variables from interpreter's global scope (primary source)
    if (m_interpreter) {
        try {
            const auto& global_scope = m_interpreter->get_global_scope();
            for (const auto& [name, _] : global_scope) {
                if (unique_names.insert(name).second) {
                    globals.push_back(name);
                }
            }
        } catch (const std::exception&) {
            // Continue with other sources if this fails
        }
    }
    
    // Add registered functions
    for (const auto& [name, _] : m_registered_functions) {
        if (unique_names.insert(name).second) {
            globals.push_back(name);
        }
    }
    
    // Add registered classes
    for (const auto& [name, _] : m_registered_classes) {
        if (unique_names.insert(name).second) {
            globals.push_back(name);
        }
    }
    
    // Add registered modules
    for (const auto& [name, _] : m_registered_modules) {
        if (unique_names.insert(name).second) {
            globals.push_back(name);
        }
    }
    
    // Add any remaining local registry items (fallback)
    for (const auto& [name, _] : m_global_variables) {
        if (unique_names.insert(name).second) {
            globals.push_back(name);
        }
    }
    
    return globals;
}

auto engine_t::register_function(const std::string& name, native_function_t func) -> void {
    m_registered_functions[name] = func;
    
    // Create our adapter that bridges native_function_t with builtin_function_object_t
    auto adapter = std::make_shared<native_function_adapter_t>(func, name);
    
    set_global_value(name, adapter);
    
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    m_stats.functions_registered++;
}

auto engine_t::unregister_function(const std::string& name) -> bool {
    auto it = m_registered_functions.find(name);
    if (it != m_registered_functions.end()) {
        m_registered_functions.erase(it);
        return true;
    }
    return false;
}

auto engine_t::has_function(const std::string& name) -> bool {
    return m_registered_functions.find(name) != m_registered_functions.end();
}

auto engine_t::list_functions() -> std::vector<std::string> {
    std::vector<std::string> functions;
    functions.reserve(m_registered_functions.size());
    
    for (const auto& [name, _] : m_registered_functions) {
        functions.push_back(name);
    }
    
    return functions;
}

auto engine_t::register_class(const std::string& name, std::shared_ptr<native_class_t> cls) -> void {
    m_registered_classes[name] = cls;
    
    // Create Zephyr class object and register it
    auto class_result = cls->create_zephyr_class(name);
    if (class_result) {
        set_global_value(name, class_result.value());
    }
    
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    m_stats.classes_registered++;
}

auto engine_t::unregister_class(const std::string& name) -> bool {
    auto it = m_registered_classes.find(name);
    if (it != m_registered_classes.end()) {
        m_registered_classes.erase(it);
        return true;
    }
    return false;
}

auto engine_t::has_class(const std::string& name) -> bool {
    return m_registered_classes.find(name) != m_registered_classes.end();
}

auto engine_t::get_class(const std::string& name) -> std::shared_ptr<native_class_t> {
    auto it = m_registered_classes.find(name);
    return it != m_registered_classes.end() ? it->second : nullptr;
}

auto engine_t::list_classes() -> std::vector<std::string> {
    std::vector<std::string> classes;
    classes.reserve(m_registered_classes.size());
    
    for (const auto& [name, _] : m_registered_classes) {
        classes.push_back(name);
    }
    
    return classes;
}

auto engine_t::register_module(const std::string& name, std::shared_ptr<native_module_t> module) -> void {
    m_registered_modules[name] = module;
    
    // Initialize the module
    auto init_result = module->initialize();
    if (!init_result) {
        auto error = create_error_info("Failed to initialize module '" + name + "': " + init_result.error());
        handle_error(error);
        return;
    }
    
    // Create module object and register it
    auto module_result = module->create_module_object();
    if (module_result) {
        set_global_value(name, module_result.value());
    }
    
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    m_stats.modules_registered++;
}

auto engine_t::create_module(const std::string& name) -> std::shared_ptr<native_module_t> {
    auto module = make_native_module(name);
    register_module(name, module);
    return module;
}

auto engine_t::unregister_module(const std::string& name) -> bool {
    auto it = m_registered_modules.find(name);
    if (it != m_registered_modules.end()) {
        // Finalize the module
        it->second->finalize();
        m_registered_modules.erase(it);
        return true;
    }
    return false;
}

auto engine_t::has_module(const std::string& name) -> bool {
    return m_registered_modules.find(name) != m_registered_modules.end();
}

auto engine_t::get_module(const std::string& name) -> std::shared_ptr<native_module_t> {
    auto it = m_registered_modules.find(name);
    return it != m_registered_modules.end() ? it->second : nullptr;
}

auto engine_t::list_modules() -> std::vector<std::string> {
    std::vector<std::string> modules;
    modules.reserve(m_registered_modules.size());
    
    for (const auto& [name, _] : m_registered_modules) {
        modules.push_back(name);
    }
    
    return modules;
}

auto engine_t::load_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>> {
    if (!m_plugin_loader) {
        return result_t<std::shared_ptr<plugin_interface_t>>::error("Plugin loader not initialized");
    }
    
    auto result = m_plugin_loader->load_plugin(library_path);
    if (result) {
        std::lock_guard<std::mutex> lock(m_stats_mutex);
        m_stats.plugins_loaded++;
    }
    
    return result;
}

auto engine_t::unload_plugin(const std::string& library_path) -> void_result_t {
    if (!m_plugin_loader) {
        return void_result_t::error("Plugin loader not initialized");
    }
    
    return m_plugin_loader->unload_plugin(library_path);
}

auto engine_t::is_plugin_loaded(const std::string& library_path) -> bool {
    return m_plugin_loader && m_plugin_loader->is_plugin_loaded(library_path);
}

auto engine_t::get_plugin(const std::string& library_path) -> std::shared_ptr<plugin_interface_t> {
    return m_plugin_loader ? m_plugin_loader->get_plugin(library_path) : nullptr;
}

auto engine_t::list_loaded_plugins() -> std::vector<plugin_info_t> {
    return m_plugin_loader ? m_plugin_loader->list_loaded_plugins() : std::vector<plugin_info_t>();
}

auto engine_t::add_library_search_path(const std::string& path) -> void {
    if (m_plugin_loader) {
        m_plugin_loader->add_search_path(path);
    }
}

auto engine_t::get_library_search_paths() -> std::vector<std::string> {
    return m_plugin_loader ? m_plugin_loader->get_search_paths() : std::vector<std::string>();
}

auto engine_t::scan_and_load_plugins(const std::string& directory, bool recursive) -> std::vector<result_t<std::shared_ptr<plugin_interface_t>>> {
    if (!m_plugin_loader) {
        return {};
    }
    
    return m_plugin_loader->load_plugins_from_directory(directory, recursive);
}

auto engine_t::set_error_handler(error_handler_t handler) -> void {
    m_error_handler = handler;
    
    if (m_plugin_loader) {
        m_plugin_loader->set_error_handler(handler);
    }
}

auto engine_t::get_last_error() -> std::optional<error_info_t> {
    return m_last_error;
}

auto engine_t::clear_last_error() -> void {
    m_last_error.reset();
}

auto engine_t::set_debug_mode(bool enabled) -> void {
    m_debug_mode = enabled;
    if (m_interpreter) {
        m_interpreter->debug_mode = enabled;
    }
}

auto engine_t::is_debug_mode() const -> bool {
    return m_debug_mode;
}

auto engine_t::set_async_enabled(bool enabled) -> void {
    m_async_enabled = enabled;
}

auto engine_t::is_async_enabled() const -> bool {
    return m_async_enabled;
}

auto engine_t::add_import_path(const std::string& path) -> void {
    m_import_paths.push_back(path);
    
    // Integrate with runtime's module loader
    if (m_runtime && m_runtime->get_module_loader()) {
        m_runtime->get_module_loader()->add_import_path(path);
    }
}

auto engine_t::get_import_paths() -> std::vector<std::string> {
    std::vector<std::string> all_paths = m_import_paths;
    
    // Include paths from module loader if available
    if (m_runtime && m_runtime->get_module_loader()) {
        auto module_paths = m_runtime->get_module_loader()->get_import_paths();
        all_paths.insert(all_paths.end(), module_paths.begin(), module_paths.end());
    }
    
    return all_paths;
}

auto engine_t::set_stdout_handler(std::function<void(const std::string&)> handler) -> void {
    m_stdout_handler = handler;
}

auto engine_t::set_stderr_handler(std::function<void(const std::string&)> handler) -> void {
    m_stderr_handler = handler;
}

auto engine_t::get_interpreter() -> interpreter_t* {
    return m_interpreter.get();
}

auto engine_t::get_runtime() -> runtime_t* {
    return m_runtime.get();
}

auto engine_t::get_plugin_loader() -> plugin_loader_t* {
    return m_plugin_loader.get();
}

auto engine_t::evaluate_expression(const std::string& expression) -> value_result_t {
    update_stats_evaluation();
    
    // Wrap expression in a return statement to get the result
    std::string wrapped_code = "return " + expression;
    return execute_string(wrapped_code, "<expression>");
}

auto engine_t::call_function(const std::string& function_name, const std::vector<value_t>& args) -> value_result_t {
    try {
        // Check if it's a registered function
        auto func_it = m_registered_functions.find(function_name);
        if (func_it != m_registered_functions.end()) {
            return func_it->second(args);
        }
        
        // Try to resolve from global scope
        auto func_value = get_global_value(function_name);
        if (!func_value) {
            return value_result_t::error("Function not found: " + function_name);
        }
        
        // Call the function
        auto result = func_value.value()->call(args);
        return value_result_t::success(result);
        
    } catch (const std::exception& e) {
        return value_result_t::error("Function call failed: " + std::string(e.what()));
    }
}

auto engine_t::set_name(const std::string& name) -> void {
    m_name = name;
}

auto engine_t::get_name() const -> std::string {
    return m_name;
}

auto engine_t::set_version(const std::string& version) -> void {
    m_version = version;
}

auto engine_t::get_version() const -> std::string {
    return m_version;
}

auto engine_t::get_stats() const -> engine_stats_t {
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    return m_stats;
}

auto engine_t::reset_stats() -> void {
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    m_stats = engine_stats_t{};
}

auto engine_t::handle_error(const error_info_t& error) -> void {
    m_last_error = error;
    
    if (m_error_handler) {
        m_error_handler(error);
    } else {
        // Default error handling - print to stderr
        std::cerr << "Zephyr Error: " << error.to_string() << std::endl;
    }
}

auto engine_t::update_stats_execution() -> void {
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    m_stats.scripts_executed++;
}

auto engine_t::update_stats_evaluation() -> void {
    std::lock_guard<std::mutex> lock(m_stats_mutex);
    m_stats.expressions_evaluated++;
}

// =============================================================================
// Factory Functions
// =============================================================================

auto create_engine() -> std::unique_ptr<engine_t> {
    return std::make_unique<engine_t>();
}

auto create_engine(const engine_config_t& config) -> std::unique_ptr<engine_t> {
    auto engine = std::make_unique<engine_t>();
    
    engine->set_name(config.name);
    engine->set_version(config.version);
    engine->set_debug_mode(config.debug_mode);
    engine->set_async_enabled(config.async_enabled);
    
    for (const auto& path : config.import_paths) {
        engine->add_import_path(path);
    }
    
    for (const auto& path : config.library_search_paths) {
        engine->add_library_search_path(path);
    }
    
    if (config.error_handler) {
        engine->set_error_handler(config.error_handler);
    }
    
    if (config.stdout_handler) {
        engine->set_stdout_handler(config.stdout_handler);
    }
    
    if (config.stderr_handler) {
        engine->set_stderr_handler(config.stderr_handler);
    }
    
    return engine;
}

// =============================================================================
// Global API Functions
// =============================================================================

auto default_engine() -> engine_t& {
    std::lock_guard<std::mutex> lock(g_default_engine_mutex);
    
    if (!g_default_engine) {
        g_default_engine = create_engine();
    }
    
    return *g_default_engine;
}

auto execute(const std::string& code, const std::string& context) -> value_result_t {
    return default_engine().execute_string(code, context);
}

auto load_global_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>> {
    return default_engine().load_plugin(library_path);
}

} // namespace zephyr::api

// =============================================================================
// Global Convenience Functions (zephyr namespace)
// =============================================================================

// Note: create_api_engine() and execute_code() are already defined inline in the header