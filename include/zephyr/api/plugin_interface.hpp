#pragma once

#include "zephyr/objects/object.hpp"
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <map>

// Export macro for cross-platform compatibility
#ifdef _WIN32
    #define ZEPHYR_EXPORT __declspec(dllexport)
#else
    #define ZEPHYR_EXPORT __attribute__((visibility("default")))
#endif

namespace zephyr::api {

// Forward declarations
class engine_t;
class native_module_t;
using value_t = std::shared_ptr<object_t>;

// Version info for compatibility checking
struct plugin_version_t {
    int major = 1;
    int minor = 0;
    int patch = 0;
    
    auto to_string() const -> std::string {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch);
    }
    
    auto is_compatible_with(const plugin_version_t& other) const -> bool {
        // Semantic versioning: same major version, minor >= required
        return major == other.major && minor >= other.minor;
    }
    
    auto operator==(const plugin_version_t& other) const -> bool {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    auto operator<(const plugin_version_t& other) const -> bool {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

// Plugin metadata
struct plugin_info_t {
    std::string name;
    std::string description;
    std::string author;
    plugin_version_t version;
    plugin_version_t min_zephyr_version;
    std::vector<std::string> dependencies;
    std::string license;
    std::string website;
    
    auto to_string() const -> std::string {
        std::string result = name + " v" + version.to_string();
        if (!author.empty()) {
            result += " by " + author;
        }
        if (!description.empty()) {
            result += "\n" + description;
        }
        return result;
    }
};

// Result wrapper for plugin operations
class plugin_result_t {
public:
    plugin_result_t() : m_success(true) {}
    plugin_result_t(value_t value) : m_value(value), m_success(true) {}
    plugin_result_t(const std::string& error) : m_error(error), m_success(false) {}
    
    static auto success() -> plugin_result_t {
        return plugin_result_t();
    }
    
    static auto success(value_t value) -> plugin_result_t {
        return plugin_result_t(value);
    }
    
    static auto error(const std::string& message) -> plugin_result_t {
        return plugin_result_t(message);
    }
    
    auto is_success() const -> bool { return m_success; }
    auto is_error() const -> bool { return !m_success; }
    auto value() const -> value_t { return m_value; }
    auto error() const -> std::string { return m_error; }
    
    operator bool() const { return m_success; }

private:
    value_t m_value;
    std::string m_error;
    bool m_success;
};

// Plugin interface that shared libraries must implement
class plugin_interface_t {
public:
    virtual ~plugin_interface_t() = default;
    
    // Plugin metadata - must be implemented
    virtual auto get_plugin_info() const -> plugin_info_t = 0;
    
    // Plugin lifecycle methods
    
    // Called when library is loaded, before any imports
    virtual auto initialize(engine_t* engine) -> plugin_result_t = 0;
    
    // Called when library is unloaded
    virtual auto finalize() -> void = 0;
    
    // Module creation - called for each import
    virtual auto create_module() -> std::shared_ptr<native_module_t> = 0;
    
    // Optional: Handle specific symbol imports (for named imports)
    // e.g., import sin, cos from "math_plugin.so"
    virtual auto get_symbol(const std::string& symbol_name) -> std::optional<value_t> {
        return std::nullopt;
    }
    
    // Optional: Handle namespace imports with alias
    // e.g., import * as math from "math_plugin.so"
    virtual auto get_all_symbols() -> std::optional<std::map<std::string, value_t>> {
        return std::nullopt;
    }
    
    // Optional: Custom import handling
    virtual auto handle_custom_import(const std::string& import_spec) -> std::optional<value_t> {
        return std::nullopt;
    }
    
    // Optional: Plugin-specific configuration
    virtual auto configure(const std::map<std::string, std::string>& config) -> plugin_result_t {
        return plugin_result_t::success();
    }
    
    // Optional: Health check - useful for debugging
    virtual auto health_check() -> plugin_result_t {
        return plugin_result_t::success();
    }
};

// Standard function signatures that shared libraries must export
using create_plugin_func_t = plugin_interface_t*(*)();
using destroy_plugin_func_t = void(*)(plugin_interface_t*);
using get_plugin_api_version_func_t = int(*)();

// Standard export names - these must be exported by every plugin
constexpr const char* CREATE_PLUGIN_FUNC_NAME = "zephyr_create_plugin";
constexpr const char* DESTROY_PLUGIN_FUNC_NAME = "zephyr_destroy_plugin"; 
constexpr const char* GET_API_VERSION_FUNC_NAME = "zephyr_get_api_version";

// Plugin API version - increment when breaking changes occur
constexpr plugin_version_t CURRENT_PLUGIN_API_VERSION{1, 0, 0};

// Helper macros for plugin exports
#define ZEPHYR_PLUGIN_API_VERSION() \
    extern "C" ZEPHYR_EXPORT int zephyr_get_api_version() { \
        return 1; \
    }

#define ZEPHYR_PLUGIN_CREATOR(plugin_class) \
    extern "C" ZEPHYR_EXPORT zephyr::api::plugin_interface_t* zephyr_create_plugin() { \
        return new plugin_class(); \
    } \
    extern "C" ZEPHYR_EXPORT void zephyr_destroy_plugin(zephyr::api::plugin_interface_t* plugin) { \
        delete plugin; \
    }

// Complete plugin declaration macro
#define ZEPHYR_PLUGIN(plugin_class) \
    ZEPHYR_PLUGIN_API_VERSION() \
    ZEPHYR_PLUGIN_CREATOR(plugin_class)

} // namespace zephyr::api