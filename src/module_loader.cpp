#include "zephyr/module_loader.hpp"
#include "zephyr/interpreter.hpp"
#include "zephyr/parser.hpp"
#include "zephyr/lexer.hpp"
#include "zephyr/errors.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/objects/builtin_function_object.hpp"
#include "zephyr/objects/module_object.hpp"
#include "zephyr/runtime_error.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace zephyr
{

module_t::module_t(const std::string& name, const std::string& file_path)
    : m_name(name), m_file_path(file_path), m_is_executed(false)
{
}

// =============================================================================
// Plugin Module Implementation
// =============================================================================

plugin_module_t::plugin_module_t(const std::string& name, const std::string& file_path,
                                std::shared_ptr<zephyr::api::plugin_interface_t> plugin)
    : m_name(name), m_file_path(file_path), m_plugin(plugin), m_is_executed(false)
{
}

auto plugin_module_t::get_name() const -> const std::string&
{
    return m_name;
}

auto plugin_module_t::get_file_path() const -> const std::string&
{
    return m_file_path;
}

auto plugin_module_t::is_executed() const -> bool
{
    return m_is_executed;
}

auto plugin_module_t::set_executed(bool executed) -> void
{
    m_is_executed = executed;
}

auto plugin_module_t::execute() -> void
{
    if (m_is_executed) {
        return;
    }
    
    try {
        // Initialize the plugin
        auto init_result = m_plugin->initialize(nullptr); // TODO: Pass engine
        if (!init_result) {
            throw import_error_t("Plugin initialization failed: " + init_result.error());
        }
        
        // Create the native module
        m_native_module = m_plugin->create_module();
        if (!m_native_module) {
            throw import_error_t("Plugin failed to create module");
        }
        
        // Initialize the native module
        auto module_init_result = m_native_module->initialize();
        if (!module_init_result) {
            throw import_error_t("Native module initialization failed: " + module_init_result.error());
        }
        
        // Convert native module exports to Zephyr objects
        auto exported_symbols = m_native_module->get_exported_symbols();
        
        for (const auto& symbol_name : exported_symbols) {
            value_t zephyr_value = nullptr;
            
            // Try to get as function
            if (m_native_module->has_function(symbol_name)) {
                auto native_func = m_native_module->get_function(symbol_name);
                if (native_func) {
                    // Create a native function adapter (similar to zephyr_api.cpp)
                    class plugin_function_adapter_t : public builtin_function_object_t {
                    private:
                        zephyr::api::native_function_t m_native_func;
                        
                    public:
                        plugin_function_adapter_t(const zephyr::api::native_function_t& func, const std::string& name) 
                            : builtin_function_object_t(nullptr, name), m_native_func(func) {}
                        
                        auto call(const std::vector<value_t>& args) -> value_t override {
                            auto result = m_native_func(args);
                            if (result) {
                                return result.value();
                            } else {
                                throw import_error_t(result.error());
                            }
                        }
                    };
                    
                    auto adapter = std::make_shared<plugin_function_adapter_t>(*native_func, symbol_name);
                    zephyr_value = adapter;
                }
            }
            // Try to get as constant
            else if (m_native_module->has_constant(symbol_name)) {
                auto constant_value = m_native_module->get_constant(symbol_name);
                if (constant_value) {
                    zephyr_value = *constant_value;
                }
            }
            // Try to get as variable
            else if (m_native_module->has_variable(symbol_name)) {
                auto variable_value = m_native_module->get_variable(symbol_name);
                if (variable_value) {
                    zephyr_value = *variable_value;
                }
            }
            
            if (zephyr_value) {
                m_exports[symbol_name] = zephyr_value;
            }
        }
        
        m_is_executed = true;
        
    } catch (const std::exception& e) {
        throw import_error_t("Plugin execution failed: " + std::string(e.what()));
    }
}

auto plugin_module_t::get_export(const std::string& symbol_name) const -> value_t
{
    auto it = m_exports.find(symbol_name);
    if (it != m_exports.end()) {
        return it->second;
    }
    return nullptr;
}

auto plugin_module_t::get_all_exports() const -> const std::map<std::string, value_t>&
{
    return m_exports;
}

auto plugin_module_t::get_plugin() const -> std::shared_ptr<zephyr::api::plugin_interface_t>
{
    return m_plugin;
}

auto module_t::get_name() const -> const std::string&
{
    return m_name;
}

auto module_t::get_file_path() const -> const std::string&
{
    return m_file_path;
}

auto module_t::get_source_code() const -> const std::string&
{
    return m_source_code;
}

auto module_t::set_source_code(const std::string& source) -> void
{
    m_source_code = source;
}

auto module_t::get_ast() const -> const program_t*
{
    return m_ast.get();
}

auto module_t::set_ast(std::unique_ptr<program_t> ast) -> void
{
    m_ast = std::move(ast);
}

auto module_t::is_executed() const -> bool
{
    return m_is_executed;
}

auto module_t::set_executed(bool executed) -> void
{
    m_is_executed = executed;
}

auto module_t::execute(module_loader_t& loader) -> void
{
    if (m_is_executed)
    {
        return;
    }
    
    if (!m_ast)
    {
        throw import_error_t("Module AST not set");
    }
    
    // Create interpreter for this module
    m_module_interpreter = std::make_shared<interpreter_t>(m_file_path, m_source_code);
    m_module_interpreter->set_module_loader(std::shared_ptr<module_loader_t>(&loader, [](module_loader_t*){}));
    m_module_interpreter->set_current_module(std::shared_ptr<module_t>(this, [](module_t*){}));
    
    // Execute the module
    m_module_interpreter->interpret(*m_ast);
    
    m_is_executed = true;
}

auto module_t::get_export(const std::string& symbol_name) const -> value_t
{
    auto it = m_exports.find(symbol_name);
    if (it != m_exports.end())
    {
        return it->second;
    }
    return nullptr;
}

auto module_t::add_export(const std::string& symbol_name, value_t value) -> void
{
    m_exports[symbol_name] = value;
}

auto module_t::get_all_exports() const -> const std::map<std::string, value_t>&
{
    return m_exports;
}

auto module_t::get_global_scope() const -> const std::map<std::string, value_t>&
{
    if (m_module_interpreter && m_is_executed)
    {
        return m_module_interpreter->get_global_scope();
    }
    static const std::map<std::string, value_t> empty_scope;
    return empty_scope;
}

module_loader_t::module_loader_t()
{
    initialize_search_paths();
    // Plugin loader will be set later via set_plugin_loader()
    m_plugin_loader = nullptr;
}

auto module_loader_t::initialize_search_paths() -> void
{
    // Add current working directory
    m_search_paths.push_back(".");
    
    // Parse ZEPHYRPATH environment variable
    const char* zephyr_path = std::getenv("ZEPHYRPATH");
    if (zephyr_path)
    {
        std::string path_string(zephyr_path);
        // Split by platform-specific delimiter
#ifdef _WIN32
        const char delimiter = ';';
#else
        const char delimiter = ':';
#endif
        
        size_t start = 0;
        size_t end = path_string.find(delimiter);
        
        while (end != std::string::npos)
        {
            std::string path = path_string.substr(start, end - start);
            if (!path.empty())
            {
                m_search_paths.push_back(path);
            }
            start = end + 1;
            end = path_string.find(delimiter, start);
        }
        
        // Add the last path
        if (start < path_string.length())
        {
            std::string path = path_string.substr(start);
            if (!path.empty())
            {
                m_search_paths.push_back(path);
            }
        }
    }
}

auto module_loader_t::load_module(const std::string& specifier, 
                                 bool is_path_based, 
                                 const std::string& requesting_file_path) -> std::shared_ptr<module_t>
{
    // Resolve the module path
    std::string resolved_path = resolve_module_path(specifier, is_path_based, requesting_file_path);
    
    // Check if this is a plugin file
    if (is_plugin_file(resolved_path)) {
        // Load as plugin and convert to regular module interface
        auto plugin_module = load_plugin_module(specifier, requesting_file_path);
        if (!plugin_module) {
            throw import_error_t("Failed to load plugin: " + specifier);
        }
        
        // Create a regular module wrapper that delegates to the plugin module
        auto module = std::make_shared<module_t>(plugin_module->get_name(), plugin_module->get_file_path());
        
        // Execute plugin module to populate exports
        plugin_module->execute();
        
        // Copy exports from plugin module to regular module
        for (const auto& [name, value] : plugin_module->get_all_exports()) {
            module->add_export(name, value);
        }
        
        // Cache and return
        m_module_cache[resolved_path] = module;
        return module;
    }
    
    // Check if module is already cached
    auto it = m_module_cache.find(resolved_path);
    if (it != m_module_cache.end())
    {
        return it->second;
    }
    
    // Check for circular dependencies
    detect_circular_dependency(resolved_path);
    
    // Add to loading stack
    m_loading_stack.push_back(resolved_path);
    
    try
    {
        // Read the module file
        std::string source_code = read_file_content(resolved_path);
        
        // Determine module name
        std::string module_name;
        if (is_path_based)
        {
            // For path-based imports, use the filename without extension
            std::filesystem::path path(resolved_path);
            module_name = path.stem().string();
        }
        else
        {
            // For name-based imports, use the specifier as the module name
            // For dotted names like "math.advanced", use the full dotted name
            module_name = specifier;
        }
        
        // Create module
        auto module = std::make_shared<module_t>(module_name, resolved_path);
        module->set_source_code(source_code);
        
        // Parse the module
        lexer_t lexer(source_code);
        parser_t parser(lexer);
        auto ast = parser.parse();
        module->set_ast(std::move(ast));
        
        // Cache the module before execution to handle circular dependencies
        m_module_cache[resolved_path] = module;
        
        // Execute the module
        module->execute(*this);
        
        // Remove from loading stack
        m_loading_stack.pop_back();
        
        return module;
    }
    catch (const std::exception& e)
    {
        // Remove from loading stack on error
        m_loading_stack.pop_back();
        throw import_error_t("Failed to load module '" + specifier + "': " + e.what());
    }
}

auto module_loader_t::resolve_module_path(const std::string& specifier, 
                                         bool is_path_based,
                                         const std::string& requesting_file_path) const -> std::string
{
    if (is_path_based)
    {
        // Path-based resolution
        std::filesystem::path requesting_dir = std::filesystem::path(requesting_file_path).parent_path();
        std::filesystem::path resolved_path = requesting_dir / specifier;
        
        // Convert to absolute path
        resolved_path = std::filesystem::absolute(resolved_path);
        
        if (!file_exists(resolved_path.string()))
        {
            throw import_error_t("Module file not found: " + resolved_path.string());
        }
        
        return resolved_path.string();
    }
    else
    {
        // Name-based resolution with support for dotted module names
        // For "math.advanced", search for:
        // 1. math/advanced.zephyr (subdirectory)
        // 2. math.advanced.zephyr (filename with dots)
        
        std::vector<std::string> search_patterns;
        
        // Convert dots to path separators for subdirectory search
        std::string subdir_path = specifier;
        std::replace(subdir_path.begin(), subdir_path.end(), '.', '/');
        search_patterns.push_back(subdir_path + ".zephyr");
        
        // Also try as direct filename with dots
        search_patterns.push_back(specifier + ".zephyr");
        
        // Search in all search paths with all patterns
        for (const auto& search_path : m_search_paths)
        {
            for (const auto& pattern : search_patterns)
            {
                std::filesystem::path candidate_path = std::filesystem::path(search_path) / pattern;
                candidate_path = std::filesystem::absolute(candidate_path);
                
                if (file_exists(candidate_path.string()))
                {
                    return candidate_path.string();
                }
            }
        }
        
        // Module not found
        throw import_error_t("Module '" + specifier + "' not found in search paths");
    }
}

auto module_loader_t::detect_circular_dependency(const std::string& module_path) -> void
{
    auto it = std::find(m_loading_stack.begin(), m_loading_stack.end(), module_path);
    if (it != m_loading_stack.end())
    {
        // Build circular dependency chain
        std::string chain = module_path;
        for (auto stack_it = it; stack_it != m_loading_stack.end(); ++stack_it)
        {
            chain += " -> " + *stack_it;
        }
        chain += " -> " + module_path;
        
        throw import_error_t("Circular dependency detected: " + chain);
    }
}

auto module_loader_t::file_exists(const std::string& path) const -> bool
{
    std::filesystem::path file_path(path);
    return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
}

auto module_loader_t::make_absolute_path(const std::string& path) const -> std::string
{
    return std::filesystem::absolute(std::filesystem::path(path)).string();
}

auto module_loader_t::read_file_content(const std::string& path) const -> std::string
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw import_error_t("Cannot open file: " + path);
    }
    
    // Read entire file content
    std::string content;
    std::string line;
    while (std::getline(file, line))
    {
        content += line + '\n';
    }
    
    return content;
}

auto module_loader_t::load_plugin_module(const std::string& specifier,
                                        const std::string& requesting_file_path) -> std::shared_ptr<plugin_module_t>
{
    // Resolve plugin path
    std::string resolved_path = resolve_plugin_path(specifier, requesting_file_path);
    
    // Check if already cached
    auto it = m_plugin_cache.find(resolved_path);
    if (it != m_plugin_cache.end()) {
        return it->second;
    }
    
    try {
        // Check if plugin loader is available
        if (!m_plugin_loader) {
            std::cerr << "Plugin loader not available for loading: " << resolved_path << std::endl;
            return nullptr;
        }
        
        // Load the plugin
        auto plugin_result = m_plugin_loader->load_plugin(resolved_path);
        if (!plugin_result) {

            return nullptr;
        }
        
        auto plugin = plugin_result.value();
        
        // Determine module name
        std::filesystem::path path(resolved_path);
        std::string module_name = path.stem().string();
        
        // Create plugin module
        auto plugin_module = std::make_shared<plugin_module_t>(module_name, resolved_path, plugin);
        
        // Cache and return
        m_plugin_cache[resolved_path] = plugin_module;
        return plugin_module;
        
    } catch (const std::exception& e) {

        return nullptr;
    }
}

auto module_loader_t::is_plugin_file(const std::string& file_path) const -> bool
{
    std::filesystem::path path(file_path);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension == ".so" || extension == ".dll" || extension == ".dylib";
}

auto module_loader_t::resolve_plugin_path(const std::string& specifier,
                                         const std::string& requesting_file_path) const -> std::string
{
    // For plugins, we use similar logic to regular modules but look for plugin extensions
    
    // If it's already a path with plugin extension, resolve it directly
    if (specifier.find('/') != std::string::npos || specifier.find('\\') != std::string::npos) {
        if (is_plugin_file(specifier)) {
            std::filesystem::path requesting_dir = std::filesystem::path(requesting_file_path).parent_path();
            std::filesystem::path plugin_path = requesting_dir / specifier;
            return make_absolute_path(plugin_path.string());
        }
    }
    
    // Try different plugin extensions
    std::vector<std::string> extensions;
#ifdef _WIN32
    extensions = {".dll"};
#elif defined(__APPLE__)
    extensions = {".dylib", ".so"};
#else
    extensions = {".so"};
#endif
    
    // Try in current directory first (relative to requesting file)
    if (!requesting_file_path.empty()) {
        std::filesystem::path requesting_dir = std::filesystem::path(requesting_file_path).parent_path();
        
        for (const auto& ext : extensions) {
            std::filesystem::path candidate = requesting_dir / (specifier + ext);
            if (file_exists(candidate.string())) {
                return make_absolute_path(candidate.string());
            }
        }
    }
    
    // Try search paths
    for (const auto& search_path : m_search_paths) {
        for (const auto& ext : extensions) {
            std::filesystem::path candidate = std::filesystem::path(search_path) / (specifier + ext);
            if (file_exists(candidate.string())) {
                return make_absolute_path(candidate.string());
            }
        }
    }
    
    // Not found
    throw import_error_t("Plugin not found: " + specifier);
}

auto module_loader_t::set_plugin_loader(std::shared_ptr<zephyr::api::plugin_loader_t> plugin_loader) -> void
{
    m_plugin_loader = std::move(plugin_loader);
}

} // namespace zephyr