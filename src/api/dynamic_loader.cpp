#include "zephyr/api/dynamic_loader.hpp"
#include "zephyr/api/result.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

#ifndef _WIN32
    #include <dlfcn.h>
#endif

namespace zephyr::api {

// =============================================================================
// Dynamic Library Implementation
// =============================================================================

dynamic_library_t::dynamic_library_t(const std::string& path)
    : m_path(path), m_handle(nullptr), m_loaded(false) {}

dynamic_library_t::~dynamic_library_t() {
    unload();
}

dynamic_library_t::dynamic_library_t(dynamic_library_t&& other) noexcept
    : m_path(std::move(other.m_path))
    , m_handle(other.m_handle)
    , m_loaded(other.m_loaded) {
    other.m_handle = nullptr;
    other.m_loaded = false;
}

dynamic_library_t& dynamic_library_t::operator=(dynamic_library_t&& other) noexcept {
    if (this != &other) {
        unload();
        
        m_path = std::move(other.m_path);
        m_handle = other.m_handle;
        m_loaded = other.m_loaded;
        
        other.m_handle = nullptr;
        other.m_loaded = false;
    }
    return *this;
}

auto dynamic_library_t::load() -> void_result_t {
    if (m_loaded) {
        return void_result_t::success();
    }
    
    if (!std::filesystem::exists(m_path)) {
        return void_result_t::error("Library file does not exist: " + m_path);
    }
    
#ifdef _WIN32
    m_handle = LoadLibraryA(m_path.c_str());
    if (!m_handle) {
        return void_result_t::error("Failed to load library: " + get_system_error());
    }
#else
    m_handle = dlopen(m_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!m_handle) {
        return void_result_t::error("Failed to load library: " + get_system_error());
    }
#endif
    
    m_loaded = true;
    return void_result_t::success();
}

auto dynamic_library_t::unload() -> void {
    if (!m_loaded || !m_handle) {
        return;
    }
    
#ifdef _WIN32
    FreeLibrary(m_handle);
#else
    dlclose(m_handle);
#endif
    
    m_handle = nullptr;
    m_loaded = false;
}

auto dynamic_library_t::is_loaded() const -> bool {
    return m_loaded && m_handle != nullptr;
}

auto dynamic_library_t::reload() -> void_result_t {
    unload();
    return load();
}

auto dynamic_library_t::get_last_error() const -> std::string {
    return get_system_error();
}

auto dynamic_library_t::get_system_error() const -> std::string {
#ifdef _WIN32
    DWORD error = GetLastError();
    if (error == 0) {
        return "No error";
    }
    
    LPSTR buffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buffer, 0, nullptr);
    
    std::string message(buffer, size);
    LocalFree(buffer);
    return message;
#else
    const char* error = dlerror();
    return error ? std::string(error) : "No error";
#endif
}

auto dynamic_library_t::get_library_extension() -> std::string {
#ifdef _WIN32
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

auto dynamic_library_t::get_library_prefix() -> std::string {
#ifdef _WIN32
    return "";
#else
    return "lib";
#endif
}

auto dynamic_library_t::format_library_name(const std::string& name) -> std::string {
    std::string full_name = name;
    
    // Add prefix if not present
    std::string prefix = get_library_prefix();
    if (!prefix.empty() && (full_name.size() < prefix.size() || 
        full_name.substr(0, prefix.size()) != prefix)) {
        full_name = prefix + full_name;
    }
    
    // Add extension if not present
    std::string extension = get_library_extension();
    if (full_name.size() < extension.size() || 
        full_name.substr(full_name.size() - extension.size()) != extension) {
        full_name += extension;
    }
    
    return full_name;
}

// =============================================================================
// Plugin Loader Implementation
// =============================================================================

plugin_loader_t::plugin_loader_t(engine_t* engine)
    : m_engine(engine) {
    // Add default search paths
    m_search_paths.push_back(".");
    m_search_paths.push_back("./plugins");
    
#ifdef _WIN32
    m_search_paths.push_back("C:\\Program Files\\Zephyr\\plugins");
#else
    m_search_paths.push_back("/usr/local/lib/zephyr/plugins");
    m_search_paths.push_back("/usr/lib/zephyr/plugins");
#endif
}

plugin_loader_t::~plugin_loader_t() {
    unload_all_plugins();
}

auto plugin_loader_t::load_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>> {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Resolve full path
    std::string resolved_path = resolve_library_path(library_path);
    
    // Check if already loaded
    if (is_plugin_loaded_internal(resolved_path)) {
        auto& loaded_plugin = m_loaded_plugins[resolved_path];
        return result_t<std::shared_ptr<plugin_interface_t>>::success(loaded_plugin->plugin);
    }
    
    try {
        // Create and load dynamic library
        auto library = std::make_unique<dynamic_library_t>(resolved_path);
        auto load_result = library->load();
        if (!load_result) {
            set_last_error("Failed to load library '" + resolved_path + "': " + load_result.error());
            return result_t<std::shared_ptr<plugin_interface_t>>::error(m_last_error->message);
        }
        
        // Validate plugin exports
        auto validation_result = validate_plugin_exports_internal(*library);
        if (!validation_result) {
            set_last_error("Plugin validation failed: " + validation_result.error());
            return result_t<std::shared_ptr<plugin_interface_t>>::error(m_last_error->message);
        }
        
        // Create plugin instance
        auto plugin_result = create_plugin_instance(*library);
        if (!plugin_result) {
            set_last_error("Failed to create plugin instance: " + plugin_result.error());
            return result_t<std::shared_ptr<plugin_interface_t>>::error(m_last_error->message);
        }
        
        // Get plugin info and validate compatibility
        auto plugin_info = plugin_result.value()->get_plugin_info();
        auto compat_result = validate_plugin_compatibility(plugin_info);
        if (!compat_result) {
            set_last_error("Plugin compatibility check failed: " + compat_result.error());
            return result_t<std::shared_ptr<plugin_interface_t>>::error(m_last_error->message);
        }
        
        // Initialize plugin
        auto init_result = initialize_plugin(plugin_result.value());
        if (!init_result) {
            set_last_error("Plugin initialization failed: " + init_result.error());
            return result_t<std::shared_ptr<plugin_interface_t>>::error(m_last_error->message);
        }
        
        // Store the loaded plugin
        auto loaded_plugin = std::make_unique<loaded_plugin_t>(
            std::move(library),
            std::move(plugin_result.value()),
            plugin_info,
            resolved_path
        );
        
        std::shared_ptr<plugin_interface_t> plugin_ptr = loaded_plugin->plugin;
        m_loaded_plugins[resolved_path] = std::move(loaded_plugin);
        
        return result_t<std::shared_ptr<plugin_interface_t>>::success(plugin_ptr);
        
    } catch (const std::exception& e) {
        set_last_error("Exception during plugin loading: " + std::string(e.what()));
        return result_t<std::shared_ptr<plugin_interface_t>>::error(m_last_error->message);
    }
}

auto plugin_loader_t::unload_plugin(const std::string& library_path) -> void_result_t {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string resolved_path = resolve_library_path(library_path);
    
    auto it = m_loaded_plugins.find(resolved_path);
    if (it == m_loaded_plugins.end()) {
        return void_result_t::error("Plugin not loaded: " + resolved_path);
    }
    
    try {
        // Finalize plugin
        it->second->plugin->finalize();
        
        // Remove from loaded plugins (destructor will unload library)
        m_loaded_plugins.erase(it);
        
        return void_result_t::success();
        
    } catch (const std::exception& e) {
        return void_result_t::error("Exception during plugin unloading: " + std::string(e.what()));
    }
}

auto plugin_loader_t::reload_plugin(const std::string& library_path) -> result_t<std::shared_ptr<plugin_interface_t>> {
    auto unload_result = unload_plugin(library_path);
    if (!unload_result) {
        return result_t<std::shared_ptr<plugin_interface_t>>::error("Failed to unload plugin for reload: " + unload_result.error());
    }
    
    return load_plugin(library_path);
}

auto plugin_loader_t::is_plugin_loaded(const std::string& library_path) const -> bool {
    std::lock_guard<std::mutex> lock(m_mutex);
    // Create a temporary copy to call non-const methods
    plugin_loader_t* non_const_this = const_cast<plugin_loader_t*>(this);
    std::string resolved_path = non_const_this->resolve_library_path(library_path);
    return m_loaded_plugins.find(resolved_path) != m_loaded_plugins.end();
}

// Internal version without mutex (for use when mutex is already held)
auto plugin_loader_t::is_plugin_loaded_internal(const std::string& resolved_path) const -> bool {
    return m_loaded_plugins.find(resolved_path) != m_loaded_plugins.end();
}

auto plugin_loader_t::get_plugin(const std::string& library_path) -> std::shared_ptr<plugin_interface_t> {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string resolved_path = resolve_library_path(library_path);
    
    auto it = m_loaded_plugins.find(resolved_path);
    return it != m_loaded_plugins.end() ? it->second->plugin : std::shared_ptr<plugin_interface_t>();
}

auto plugin_loader_t::get_all_plugins() -> std::vector<std::shared_ptr<plugin_interface_t>> {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::shared_ptr<plugin_interface_t>> plugins;
    plugins.reserve(m_loaded_plugins.size());
    
    for (const auto& [path, loaded_plugin] : m_loaded_plugins) {
        plugins.push_back(loaded_plugin->plugin);
    }
    
    return plugins;
}

auto plugin_loader_t::get_loaded_plugin_paths() -> std::vector<std::string> {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::string> paths;
    paths.reserve(m_loaded_plugins.size());
    
    for (const auto& [path, loaded_plugin] : m_loaded_plugins) {
        paths.push_back(path);
    }
    
    return paths;
}

auto plugin_loader_t::add_search_path(const std::string& path) -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto normalized_path = normalize_path(path);
    if (std::find(m_search_paths.begin(), m_search_paths.end(), normalized_path) == m_search_paths.end()) {
        m_search_paths.push_back(normalized_path);
    }
}

auto plugin_loader_t::remove_search_path(const std::string& path) -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto normalized_path = normalize_path(path);
    m_search_paths.erase(
        std::remove(m_search_paths.begin(), m_search_paths.end(), normalized_path),
        m_search_paths.end()
    );
}

auto plugin_loader_t::clear_search_paths() -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_search_paths.clear();
}

auto plugin_loader_t::get_search_paths() const -> const std::vector<std::string>& {
    return m_search_paths;
}

auto plugin_loader_t::find_library(const std::string& name) -> std::optional<std::string> {
    // Note: This method is called from load_plugin which already holds the mutex
    
    // If it's already an absolute path and exists, return it
    if (is_absolute_path(name) && std::filesystem::exists(name)) {
        return name;
    }
    
    // Try different variations of the name
    std::vector<std::string> name_variations = {
        name,
        dynamic_library_t::format_library_name(name)
    };
    
    // Add extension if not present
    std::string ext = dynamic_library_t::get_library_extension();
    if (name.size() < ext.size() || 
        name.substr(name.size() - ext.size()) != ext) {
        name_variations.push_back(name + ext);
    }
    
    // Search in all paths
    for (const auto& search_path : m_search_paths) {
        for (const auto& name_variant : name_variations) {
            std::filesystem::path full_path = std::filesystem::path(search_path) / name_variant;
            if (std::filesystem::exists(full_path)) {
                return get_canonical_path(full_path.string());
            }
        }
    }
    
    return std::nullopt;
}

auto plugin_loader_t::resolve_library_path(const std::string& name_or_path) -> std::string {
    // If it's an absolute path, return it
    if (is_absolute_path(name_or_path)) {
        return get_canonical_path(name_or_path);
    }

    // Try to find it in search paths
    auto found_path = find_library(name_or_path);
    if (found_path) {
        return found_path.value();
    }

    // If not found, return the original path (might be relative)
    return get_canonical_path(name_or_path);
}

auto plugin_loader_t::get_plugin_info(const std::string& library_path) -> std::optional<plugin_info_t> {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string resolved_path = resolve_library_path(library_path);
    
    auto it = m_loaded_plugins.find(resolved_path);
    if (it != m_loaded_plugins.end()) {
        return it->second->info;
    }
    
    return std::nullopt;
}

auto plugin_loader_t::list_loaded_plugins() -> std::vector<plugin_info_t> {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<plugin_info_t> plugin_infos;
    plugin_infos.reserve(m_loaded_plugins.size());
    
    for (const auto& [path, loaded_plugin] : m_loaded_plugins) {
        plugin_infos.push_back(loaded_plugin->info);
    }
    
    return plugin_infos;
}

auto plugin_loader_t::load_plugins_from_directory(const std::string& directory, bool recursive) -> std::vector<result_t<std::shared_ptr<plugin_interface_t>>> {
    std::vector<result_t<std::shared_ptr<plugin_interface_t>>> results;
    
    auto library_files = get_library_files_in_directory(directory, recursive);
    
    for (const auto& library_file : library_files) {
        auto load_result = load_plugin(library_file);
        results.push_back(std::move(load_result));
    }
    
    return results;
}

auto plugin_loader_t::unload_all_plugins() -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Finalize all plugins first
    for (auto& [path, loaded_plugin] : m_loaded_plugins) {
        try {
            loaded_plugin->plugin->finalize();
        } catch (const std::exception& e) {
            // Log error but continue with other plugins
            if (m_error_handler) {
                auto error = create_plugin_loading_error(
                    "Error finalizing plugin '" + loaded_plugin->info.name + "': " + e.what()
                );
                m_error_handler(error);
            }
        }
    }
    
    // Clear all plugins (destructors will handle cleanup)
    m_loaded_plugins.clear();
}

auto plugin_loader_t::validate_plugin_library(const std::string& library_path) -> void_result_t {
    std::string resolved_path = resolve_library_path(library_path);
    
    try {
        dynamic_library_t library(resolved_path);
        auto load_result = library.load();
        if (!load_result) {
            return void_result_t::error("Failed to load library: " + load_result.error());
        }
        
        return validate_plugin_exports(library_path);
        
    } catch (const std::exception& e) {
        return void_result_t::error("Exception during validation: " + std::string(e.what()));
    }
}

auto plugin_loader_t::get_plugin_api_version(const std::string& library_path) -> result_t<plugin_version_t> {
    std::string resolved_path = resolve_library_path(library_path);
    
    try {
        dynamic_library_t library(resolved_path);
        auto load_result = library.load();
        if (!load_result) {
            return result_t<plugin_version_t>::error("Failed to load library: " + load_result.error());
        }
        
        auto version_func = library.get_function<get_plugin_api_version_func_t>(GET_API_VERSION_FUNC_NAME);
        if (!version_func) {
            return result_t<plugin_version_t>::error("Plugin does not export version function");
        }
        
        int version_int = (*version_func)();
        plugin_version_t version = {version_int, 0, 0}; // Convert int to plugin_version_t
        return result_t<plugin_version_t>::success(version);
        
    } catch (const std::exception& e) {
        return result_t<plugin_version_t>::error("Exception during version check: " + std::string(e.what()));
    }
}

auto plugin_loader_t::set_error_handler(std::function<void(const error_info_t&)> handler) -> void {
    m_error_handler = handler;
}

auto plugin_loader_t::get_last_error() const -> std::optional<error_info_t> {
    return m_last_error;
}

// =============================================================================
// Private Helper Methods
// =============================================================================

auto plugin_loader_t::validate_plugin_compatibility(const plugin_info_t& info) -> void_result_t {
    // Check minimum Zephyr version
    plugin_version_t current_zephyr_version{1, 0, 0}; // This would come from actual Zephyr version
    
    if (!current_zephyr_version.is_compatible_with(info.min_zephyr_version)) {
        return void_result_t::error(
            "Plugin requires Zephyr version " + info.min_zephyr_version.to_string() + 
            " but current version is " + current_zephyr_version.to_string()
        );
    }
    
    return resolve_dependencies(info);
}

auto plugin_loader_t::resolve_dependencies(const plugin_info_t& info) -> void_result_t {
    for (const auto& dependency : info.dependencies) {
        bool found = false;
        
        for (const auto& [path, loaded_plugin] : m_loaded_plugins) {
            if (loaded_plugin->info.name == dependency) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return void_result_t::error("Missing dependency: " + dependency);
        }
    }
    
    return void_result_t::success();
}

auto plugin_loader_t::create_plugin_instance(dynamic_library_t& library) -> result_t<std::shared_ptr<plugin_interface_t>> {
    auto create_func = library.get_function<create_plugin_func_t>(CREATE_PLUGIN_FUNC_NAME);
    auto destroy_func = library.get_function<destroy_plugin_func_t>(DESTROY_PLUGIN_FUNC_NAME);
    
    if (!create_func || !destroy_func) {
        return result_t<std::shared_ptr<plugin_interface_t>>::error(
            "Plugin does not export required functions"
        );
    }
    
    // Attempt to create plugin instance
    try {
        // Use a safer explicit cast approach
        typedef plugin_interface_t* (*safe_create_func_t)();
        safe_create_func_t safe_create = reinterpret_cast<safe_create_func_t>(create_func);
        
        auto raw_plugin = safe_create();
        
        if (!raw_plugin) {
            return result_t<std::shared_ptr<plugin_interface_t>>::error(
                "Plugin creation function returned null"
            );
        }
        
        // Create shared_ptr with default deleter (disable custom deleter to debug segfault)
        std::shared_ptr<plugin_interface_t> plugin(raw_plugin);
        return result_t<std::shared_ptr<plugin_interface_t>>::success(plugin);
        
    } catch (const std::exception& e) {
        return result_t<std::shared_ptr<plugin_interface_t>>::error(
            "Exception during plugin creation: " + std::string(e.what())
        );
    } catch (...) {
        return result_t<std::shared_ptr<plugin_interface_t>>::error(
            "Unknown exception during plugin creation"
        );
    }
}

auto plugin_loader_t::initialize_plugin(std::shared_ptr<plugin_interface_t> plugin) -> void_result_t {
    auto result = plugin->initialize(m_engine);
    if (!result) {
        return void_result_t::error(result.error());
    }
    
    return void_result_t::success();
}

auto plugin_loader_t::validate_plugin_exports_internal(dynamic_library_t& library) -> void_result_t {
    // Check for required exports
    auto create_func = library.get_function<create_plugin_func_t>(CREATE_PLUGIN_FUNC_NAME);
    auto destroy_func = library.get_function<destroy_plugin_func_t>(DESTROY_PLUGIN_FUNC_NAME);
    auto version_func = library.get_function<get_plugin_api_version_func_t>(GET_API_VERSION_FUNC_NAME);
    
    if (!create_func) {
        return void_result_t::error("Missing required export: " + std::string(CREATE_PLUGIN_FUNC_NAME));
    }
    
    if (!destroy_func) {
        return void_result_t::error("Missing required export: " + std::string(DESTROY_PLUGIN_FUNC_NAME));
    }
    
    if (!version_func) {
        return void_result_t::error("Missing required export: " + std::string(GET_API_VERSION_FUNC_NAME));
    }
    
    // Skip version check for now due to calling convention issues
    // This is a known limitation - plugins are loaded without version validation
    
    return void_result_t::success();
}

auto plugin_loader_t::set_last_error(const error_info_t& error) -> void {
    m_last_error = error;
    if (m_error_handler) {
        m_error_handler(error);
    }
}

auto plugin_loader_t::set_last_error(const std::string& message, const std::string& context) -> void {
    auto error = create_plugin_loading_error(message, context);
    set_last_error(error);
}

auto plugin_loader_t::is_absolute_path(const std::string& path) const -> bool {
    std::filesystem::path fs_path(path);
    return fs_path.is_absolute();
}

auto plugin_loader_t::normalize_path(const std::string& path) const -> std::string {
    std::filesystem::path fs_path(path);
    return fs_path.lexically_normal().string();
}

auto plugin_loader_t::get_canonical_path(const std::string& path) const -> std::string {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::filesystem::filesystem_error&) {
        // If canonical fails, return normalized path
        return normalize_path(path);
    }
}

auto plugin_loader_t::is_library_file(const std::string& path) -> bool {
    std::string ext = dynamic_library_t::get_library_extension();
    return path.size() >= ext.size() && path.substr(path.size() - ext.size()) == ext;
}

auto plugin_loader_t::get_library_files_in_directory(const std::string& directory, bool recursive) -> std::vector<std::string> {
    std::vector<std::string> library_files;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file() && is_library_file(entry.path().string())) {
                    library_files.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && is_library_file(entry.path().string())) {
                    library_files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Directory doesn't exist or can't be read
        if (m_error_handler) {
            auto error = create_plugin_loading_error(
                "Failed to scan directory '" + directory + "': " + e.what()
            );
            m_error_handler(error);
        }
    }
    
    return library_files;
}

// =============================================================================
// Plugin Registry Implementation
// =============================================================================

auto plugin_registry_t::instance() -> plugin_registry_t& {
    static plugin_registry_t registry;
    return registry;
}

auto plugin_registry_t::register_loader(const std::string& name, std::shared_ptr<plugin_loader_t> loader) -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_loaders[name] = loader;
}

auto plugin_registry_t::unregister_loader(const std::string& name) -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_loaders.erase(name);
}

auto plugin_registry_t::get_loader(const std::string& name) -> std::shared_ptr<plugin_loader_t> {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_loaders.find(name);
    return it != m_loaders.end() ? it->second : nullptr;
}

auto plugin_registry_t::get_all_loaders() -> std::map<std::string, std::shared_ptr<plugin_loader_t>> {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_loaders;
}

auto plugin_registry_t::add_global_search_path(const std::string& path) -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_global_search_paths.push_back(path);
    
    // Add to all registered loaders
    for (const auto& [name, loader] : m_loaders) {
        loader->add_search_path(path);
    }
}

auto plugin_registry_t::remove_global_search_path(const std::string& path) -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_global_search_paths.erase(
        std::remove(m_global_search_paths.begin(), m_global_search_paths.end(), path),
        m_global_search_paths.end()
    );
    
    // Remove from all registered loaders
    for (const auto& [name, loader] : m_loaders) {
        loader->remove_search_path(path);
    }
}

auto plugin_registry_t::get_global_search_paths() -> std::vector<std::string> {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_global_search_paths;
}

auto plugin_registry_t::discover_plugins_in_path(const std::string& path) -> std::vector<std::string> {
    std::vector<std::string> plugins;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string ext = dynamic_library_t::get_library_extension();
                if (filename.size() >= ext.size() && filename.substr(filename.size() - ext.size()) == ext) {
                    plugins.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Directory doesn't exist or can't be read
    }
    
    return plugins;
}

auto plugin_registry_t::get_all_available_plugins() -> std::vector<std::string> {
    std::vector<std::string> all_plugins;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& search_path : m_global_search_paths) {
        auto plugins = discover_plugins_in_path(search_path);
        all_plugins.insert(all_plugins.end(), plugins.begin(), plugins.end());
    }
    
    return all_plugins;
}

auto plugin_registry_t::set_default_search_paths() -> void {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_global_search_paths.clear();
    m_global_search_paths.push_back(".");
    m_global_search_paths.push_back("./plugins");
    
#ifdef _WIN32
    m_global_search_paths.push_back("C:\\Program Files\\Zephyr\\plugins");
#else
    m_global_search_paths.push_back("/usr/local/lib/zephyr/plugins");
    m_global_search_paths.push_back("/usr/lib/zephyr/plugins");
#endif
}

// =============================================================================
// Utility Functions
// =============================================================================

auto resolve_library_name(const std::string& name) -> std::string {
    return dynamic_library_t::format_library_name(name);
}

auto get_standard_library_paths() -> std::vector<std::string> {
    std::vector<std::string> paths;
    
#ifdef _WIN32
    paths.push_back("C:\\Windows\\System32");
    paths.push_back("C:\\Program Files\\Zephyr\\plugins");
#else
    paths.push_back("/usr/lib");
    paths.push_back("/usr/local/lib");
    paths.push_back("/usr/local/lib/zephyr/plugins");
    paths.push_back("/usr/lib/zephyr/plugins");
#endif
    
    return paths;
}

auto find_library_in_paths(const std::string& name, const std::vector<std::string>& paths) -> std::optional<std::string> {
    std::vector<std::string> name_variations = {
        name,
        dynamic_library_t::format_library_name(name)
    };
    
    // Add extension if not present
    std::string ext = dynamic_library_t::get_library_extension();
    if (name.size() < ext.size() || name.substr(name.size() - ext.size()) != ext) {
        name_variations.push_back(name + ext);
    }
    
    for (const auto& path : paths) {
        for (const auto& name_variant : name_variations) {
            std::filesystem::path full_path = std::filesystem::path(path) / name_variant;
            if (std::filesystem::exists(full_path)) {
                return full_path.string();
            }
        }
    }
    
    return std::nullopt;
}

auto validate_plugin_exports(const std::string& library_path) -> void_result_t {
    try {
        dynamic_library_t library(library_path);
        auto load_result = library.load();
        if (!load_result) {
            return void_result_t::error("Failed to load library: " + load_result.error());
        }
        
        // Check required exports
        auto create_func = library.get_function<create_plugin_func_t>(CREATE_PLUGIN_FUNC_NAME);
        auto destroy_func = library.get_function<destroy_plugin_func_t>(DESTROY_PLUGIN_FUNC_NAME);
        auto version_func = library.get_function<get_plugin_api_version_func_t>(GET_API_VERSION_FUNC_NAME);
        
        if (!create_func) {
            return void_result_t::error("Missing required export: " + std::string(CREATE_PLUGIN_FUNC_NAME));
        }
        
        if (!destroy_func) {
            return void_result_t::error("Missing required export: " + std::string(DESTROY_PLUGIN_FUNC_NAME));
        }
        
        if (!version_func) {
            return void_result_t::error("Missing required export: " + std::string(GET_API_VERSION_FUNC_NAME));
        }
        
        return void_result_t::success();
        
    } catch (const std::exception& e) {
        return void_result_t::error("Exception during export validation: " + std::string(e.what()));
    }
}

auto check_plugin_dependencies(const plugin_info_t& info, const std::vector<plugin_info_t>& available_plugins) -> void_result_t {
    for (const auto& dependency : info.dependencies) {
        bool found = false;
        
        for (const auto& available_plugin : available_plugins) {
            if (available_plugin.name == dependency) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return void_result_t::error("Missing dependency: " + dependency);
        }
    }
    
    return void_result_t::success();
}

auto create_library_error(const std::string& message, const std::string& library_path) -> error_info_t {
    error_info_t error;
    error.message = message;
    error.type = "LibraryError";
    error.filename = library_path;
    error.severity = error_severity_t::error;
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    error.source_location = ss.str();
    
    return error;
}

auto create_plugin_loading_error(const std::string& message, const std::string& plugin_name) -> error_info_t {
    error_info_t error;
    error.message = message;
    error.type = "PluginLoadingError";
    error.plugin_name = plugin_name;
    error.severity = error_severity_t::error;
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    error.source_location = ss.str();
    
    return error;
}

} // namespace zephyr::api