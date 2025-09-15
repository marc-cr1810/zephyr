#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/zephyr.hpp"

namespace zephyr::api {

/**
 * Simple working plugin that demonstrates basic plugin functionality
 * without complex features. This serves as a minimal example for
 * plugin development.
 */
class working_plugin_t : public plugin_interface_t {
public:
    working_plugin_t() = default;
    ~working_plugin_t() = default;

    auto get_plugin_info() const -> plugin_info_t override {
        plugin_info_t info;
        info.name = "working_plugin";
        info.description = "A minimal working plugin for Zephyr";
        info.author = "Zephyr Team";
        info.version = {ZEPHYR_VERSION_MAJOR, ZEPHYR_VERSION_MINOR, ZEPHYR_VERSION_PATCH};
        info.min_zephyr_version = {ZEPHYR_VERSION_MAJOR, ZEPHYR_VERSION_MINOR, ZEPHYR_VERSION_PATCH};
        info.license = "MIT";
        return info;
    }
    
    auto initialize(engine_t* engine) -> plugin_result_t override {
        // Allow null engine for runtime scenarios without full engine
        m_engine = engine;
        return plugin_result_t::success();
    }
    
    auto finalize() -> void override {
        m_engine = nullptr;
    }
    
    auto create_module() -> std::shared_ptr<native_module_t> override {
        try {
            auto module = std::make_shared<native_module_t>();
            module->set_name("working_plugin");
            module->set_description("Basic working plugin module");
            module->set_version(ZEPHYR_VERSION_STRING);
            
            // Export constants that can be imported
            module->export_constant("version", std::make_shared<string_object_t>(ZEPHYR_VERSION_STRING));
            module->export_constant("status", std::make_shared<string_object_t>("working"));
            module->export_constant("test_value", std::make_shared<int_object_t>(42));
            
            return module;
        } catch (const std::exception&) {
            return nullptr;
        }
    }
    
private:
    engine_t* m_engine = nullptr;
};

} // namespace zephyr::api

// Plugin export functions
ZEPHYR_PLUGIN(zephyr::api::working_plugin_t)