#pragma once

#include "zephyr/ast.hpp"
#include "objects/object.hpp"
#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/dynamic_loader.hpp"
#include <map>
#include <string>
#include <memory>
#include <vector>

namespace zephyr
{

// Forward declarations
class interpreter_t;
using value_t = std::shared_ptr<object_t>;

class module_t
{
public:
    // Functions
    module_t(const std::string& name, const std::string& file_path);
    
    auto get_name() const -> const std::string&;
    auto get_file_path() const -> const std::string&;
    auto get_source_code() const -> const std::string&;
    auto set_source_code(const std::string& source) -> void;
    auto get_ast() const -> const program_t*;
    auto set_ast(std::unique_ptr<program_t> ast) -> void;
    auto is_executed() const -> bool;
    auto set_executed(bool executed) -> void;
    
    auto execute(class module_loader_t& loader) -> void;
    auto get_export(const std::string& symbol_name) const -> value_t;
    auto add_export(const std::string& symbol_name, value_t value) -> void;
    auto get_all_exports() const -> const std::map<std::string, value_t>&;
    auto get_global_scope() const -> const std::map<std::string, value_t>&;

private:
    // Variables
    std::string m_name;                           // Module name (e.g., "math", "__main__")
    std::string m_file_path;                      // Absolute file path
    std::string m_source_code;                    // Module source content
    std::unique_ptr<program_t> m_ast;             // Parsed content
    std::map<std::string, value_t> m_exports;     // Export map: symbol_name -> value_t
    bool m_is_executed;                           // Execution state
    std::shared_ptr<interpreter_t> m_module_interpreter;
};

// Plugin-based module for .so/.dll/.dylib imports
class plugin_module_t
{
public:
    plugin_module_t(const std::string& name, const std::string& file_path,
                   std::shared_ptr<zephyr::api::plugin_interface_t> plugin);
    
    auto get_name() const -> const std::string&;
    auto get_file_path() const -> const std::string&;
    auto is_executed() const -> bool;
    auto set_executed(bool executed) -> void;
    
    auto execute() -> void;
    auto get_export(const std::string& symbol_name) const -> value_t;
    auto get_all_exports() const -> const std::map<std::string, value_t>&;
    auto get_plugin() const -> std::shared_ptr<zephyr::api::plugin_interface_t>;

private:
    std::string m_name;
    std::string m_file_path;
    std::shared_ptr<zephyr::api::plugin_interface_t> m_plugin;
    std::shared_ptr<zephyr::api::native_module_t> m_native_module;
    std::map<std::string, value_t> m_exports;
    bool m_is_executed;
};

class module_loader_t
{
public:
    // Functions
    module_loader_t();
    
    auto load_module(const std::string& specifier, 
                    bool is_path_based, 
                    const std::string& requesting_file_path) -> std::shared_ptr<module_t>;
    
    auto resolve_module_path(const std::string& specifier, 
                           bool is_path_based,
                           const std::string& requesting_file_path) const -> std::string;
    
    auto detect_circular_dependency(const std::string& module_path) -> void;
    auto initialize_search_paths() -> void;
    
    // Plugin loading support
    auto load_plugin_module(const std::string& specifier,
                           const std::string& requesting_file_path) -> std::shared_ptr<plugin_module_t>;
    auto is_plugin_file(const std::string& file_path) const -> bool;
    auto set_plugin_loader(std::shared_ptr<zephyr::api::plugin_loader_t> plugin_loader) -> void;

private:
    // Functions
    auto file_exists(const std::string& path) const -> bool;
    auto make_absolute_path(const std::string& path) const -> std::string;
    auto read_file_content(const std::string& path) const -> std::string;
    
    // Plugin-related helpers
    auto resolve_plugin_path(const std::string& specifier,
                            const std::string& requesting_file_path) const -> std::string;

    // Variables
    std::map<std::string, std::shared_ptr<module_t>> m_module_cache;  // absolute_path -> module_t
    std::map<std::string, std::shared_ptr<plugin_module_t>> m_plugin_cache; // absolute_path -> plugin_module_t
    std::vector<std::string> m_search_paths;                          // Search paths from ZEPHYRPATH
    std::vector<std::string> m_loading_stack;                         // Current loading stack for circular dependency detection
    std::shared_ptr<zephyr::api::plugin_loader_t> m_plugin_loader;   // Plugin dynamic loader
};

} // namespace zephyr