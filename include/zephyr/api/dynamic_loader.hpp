#pragma once

#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/result.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <mutex>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
    using library_handle_t = HMODULE;
#else
    #include <dlfcn.h>
    using library_handle_t = void*;
#endif

namespace zephyr::api {

// Forward declarations
class engine_t;

// =============================================================================
// Dynamic Library Wrapper
// =============================================================================

class dynamic_library_t {
public:
    explicit dynamic_library_t(const std::string& path);
    ~dynamic_library_t();
    
    // Non-copyable but movable
    dynamic_library_t(const dynamic_library_t&) = delete;
    dynamic_library_t& operator=(const dynamic_library_t&) = delete;
    dynamic_library_t(dynamic_library_t&& other) noexcept;
    dynamic_library_t& operator=(dynamic_library_t&& other) noexcept;
    
    // Library management
    auto load() -> void_result_t;
    auto unload() -> void;
    auto is_loaded() const -> bool;
    auto reload() -> void_result_t;
    
    // Function pointer retrieval
    template<typename T>
    auto get_function(const std::string& name) -> T* {
        if (!m_handle) return nullptr;
        
#ifdef _WIN32
        return reinterpret_cast<T*>(GetProcAddress(m_handle, name.c_str()));
#else
        return reinterpret_cast<T*>(dlsym(m_handle, name.c_str()));
#endif
    }
    
    // Get library information
    auto get_path() const -> const std::string& { return m_path; }
    auto get_filename() const -> std::string { return std::filesystem::path(m_path).filename(); }
    auto get_handle() const -> library_handle_t { return m_handle; }
    auto get_last_error() const -> std::string;
    
    // Platform-specific helpers
    static auto get_library_extension() -> std::string;
    static auto get_library_prefix() -> std::string;
    static auto format_library_name(const std::string& name) -> std::string;

private:
    std::string m_path;
    library_handle_t m_handle = nullptr;
    bool m_loaded = false;
    
    auto get_system_error() const -> std::string;
};

// =============================================================================
// Plugin Loader
// =============================================================================

class plugin_loader_t {
public:
    explicit plugin_loader_t(engine_t* engine = nullptr);
    ~plugin_loader_t();
    
    // Engine management
    auto set_engine(engine_t* engine) -> void { m_engine = engine; }
    auto get_engine() const -> engine_t* { return m_engine; }
    
    // Plugin loading/unloading
    auto load_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>>;
    auto unload_plugin(const std::string& library_path) -> void_result_t;
    auto reload_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>>;
    auto is_plugin_loaded(const std::string& library_path) const -> bool;
    
    // Plugin access
    auto get_plugin(const std::string& library_path) -> std::shared_ptr<plugin_interface_t>;
    auto get_all_plugins() -> std::vector<std::shared_ptr<plugin_interface_t>>;
    auto get_loaded_plugin_paths() -> std::vector<std::string>;
    
    // Search path management
    auto add_search_path(const std::string& path) -> void;
    auto remove_search_path(const std::string& path) -> void;
    auto clear_search_paths() -> void;
    auto get_search_paths() const -> const std::vector<std::string>&;
    
    // Library resolution
    auto find_library(const std::string& name) -> std::optional<std::string>;
    auto resolve_library_path(const std::string& name_or_path) -> std::string;
    
    // Plugin information
    auto get_plugin_info(const std::string& library_path) -> std::optional<plugin_info_t>;
    auto list_loaded_plugins() -> std::vector<plugin_info_t>;
    
    // Batch operations
    auto load_plugins_from_directory(const std::string& directory, 
                                   bool recursive = false) -> std::vector<result_t<std::shared_ptr<plugin_interface_t>>>;
    auto unload_all_plugins() -> void;
    
    // Plugin validation
    auto validate_plugin_library(const std::string& library_path) -> void_result_t;
    auto get_plugin_api_version(const std::string& library_path) -> result_t<plugin_version_t>;
    
    // Error handling
    auto set_error_handler(std::function<void(const error_info_t&)> handler) -> void;
    auto get_last_error() const -> std::optional<error_info_t>;

private:
    struct loaded_plugin_t {
        std::unique_ptr<dynamic_library_t> library;
        std::shared_ptr<plugin_interface_t> plugin;
        plugin_info_t info;
        std::string resolved_path;
        
        loaded_plugin_t(std::unique_ptr<dynamic_library_t> lib,
                       std::shared_ptr<plugin_interface_t> plug,
                       plugin_info_t inf,
                       std::string path)
            : library(std::move(lib))
            , plugin(std::move(plug))
            , info(std::move(inf))
            , resolved_path(std::move(path)) {}
    };
    
    engine_t* m_engine;
    std::map<std::string, std::unique_ptr<loaded_plugin_t>> m_loaded_plugins;
    std::vector<std::string> m_search_paths;
    std::function<void(const error_info_t&)> m_error_handler;
    std::optional<error_info_t> m_last_error;
    mutable std::mutex m_mutex; // Thread safety
    
    // Helper methods
    auto validate_plugin_compatibility(const plugin_info_t& info) -> void_result_t;
    auto resolve_dependencies(const plugin_info_t& info) -> void_result_t;
    auto create_plugin_instance(dynamic_library_t& library) -> result_t<std::shared_ptr<plugin_interface_t>>;
    auto initialize_plugin(std::shared_ptr<plugin_interface_t> plugin) -> void_result_t;
    auto validate_plugin_exports_internal(dynamic_library_t& library) -> void_result_t;
    auto is_plugin_loaded_internal(const std::string& resolved_path) const -> bool;
    auto set_last_error(const error_info_t& error) -> void;
    auto set_last_error(const std::string& message, const std::string& context = "") -> void;
    
    // Path utilities
    auto is_absolute_path(const std::string& path) const -> bool;
    auto normalize_path(const std::string& path) const -> std::string;
    auto get_canonical_path(const std::string& path) const -> std::string;
    
    // Library file detection
    auto is_library_file(const std::string& path) -> bool;
    auto get_library_files_in_directory(const std::string& directory, bool recursive) -> std::vector<std::string>;
};

// =============================================================================
// Plugin Registry (Global Plugin Management)
// =============================================================================

class plugin_registry_t {
public:
    static auto instance() -> plugin_registry_t&;
    
    // Global plugin management
    auto register_loader(const std::string& name, std::shared_ptr<plugin_loader_t> loader) -> void;
    auto unregister_loader(const std::string& name) -> void;
    auto get_loader(const std::string& name) -> std::shared_ptr<plugin_loader_t>;
    auto get_all_loaders() -> std::map<std::string, std::shared_ptr<plugin_loader_t>>;
    
    // Global search paths
    auto add_global_search_path(const std::string& path) -> void;
    auto remove_global_search_path(const std::string& path) -> void;
    auto get_global_search_paths() -> std::vector<std::string>;
    
    // Plugin discovery
    auto discover_plugins_in_path(const std::string& path) -> std::vector<std::string>;
    auto get_all_available_plugins() -> std::vector<std::string>;
    
    // Configuration
    auto set_default_search_paths() -> void;
    auto load_configuration_from_file(const std::string& config_file) -> void_result_t;
    auto save_configuration_to_file(const std::string& config_file) -> void_result_t;

private:
    plugin_registry_t() = default;
    
    std::map<std::string, std::shared_ptr<plugin_loader_t>> m_loaders;
    std::vector<std::string> m_global_search_paths;
    mutable std::mutex m_mutex;
};

// =============================================================================
// Utility Functions
// =============================================================================

// Library name resolution
auto resolve_library_name(const std::string& name) -> std::string;
auto get_standard_library_paths() -> std::vector<std::string>;
auto find_library_in_paths(const std::string& name, const std::vector<std::string>& paths) -> std::optional<std::string>;

// Plugin validation
auto validate_plugin_exports(const std::string& library_path) -> void_result_t;
auto check_plugin_dependencies(const plugin_info_t& info, const std::vector<plugin_info_t>& available_plugins) -> void_result_t;

// Error helpers
auto create_library_error(const std::string& message, const std::string& library_path) -> error_info_t;
auto create_plugin_loading_error(const std::string& message, const std::string& plugin_name = "") -> error_info_t;

// =============================================================================
// RAII Plugin Loader Helper
// =============================================================================

class scoped_plugin_loader_t {
public:
    scoped_plugin_loader_t(plugin_loader_t& loader, const std::string& library_path)
        : m_loader(loader), m_library_path(library_path) {
        m_result = m_loader.load_plugin(m_library_path);
    }
    
    ~scoped_plugin_loader_t() {
        if (m_result && m_auto_unload) {
            m_loader.unload_plugin(m_library_path);
        }
    }
    
    // Non-copyable, movable
    scoped_plugin_loader_t(const scoped_plugin_loader_t&) = delete;
    scoped_plugin_loader_t& operator=(const scoped_plugin_loader_t&) = delete;
    scoped_plugin_loader_t(scoped_plugin_loader_t&&) = default;
    scoped_plugin_loader_t& operator=(scoped_plugin_loader_t&&) = default;
    
    auto get() -> std::shared_ptr<plugin_interface_t> {
        return m_result ? m_result.value() : nullptr;
    }
    
    auto is_loaded() const -> bool {
        return m_result.is_success();
    }
    
    auto error() const -> std::string {
        return m_result.error();
    }
    
    auto release() -> std::shared_ptr<plugin_interface_t> {
        m_auto_unload = false;
        return get();
    }
    
    operator bool() const {
        return is_loaded();
    }

private:
    plugin_loader_t& m_loader;
    std::string m_library_path;
    result_t<std::shared_ptr<plugin_interface_t>> m_result;
    bool m_auto_unload = true;
};

// =============================================================================
// Configuration Structures
// =============================================================================

struct plugin_config_t {
    std::string name;
    std::string path;
    bool auto_load = false;
    std::map<std::string, std::string> settings;
    std::vector<std::string> dependencies;
};

struct loader_config_t {
    std::vector<std::string> search_paths;
    std::vector<plugin_config_t> plugins;
    bool auto_discover = true;
    bool strict_version_checking = true;
    std::string log_level = "info";
};

} // namespace zephyr::api