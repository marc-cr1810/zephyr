#pragma once

// Zephyr C++ API - All-in-One Convenience Header
// This header provides everything needed to use the Zephyr C++ API

// Core API Components
#include "zephyr/api/zephyr_api.hpp"
#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/api/native_function.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/api/result.hpp"
#include "zephyr/api/dynamic_loader.hpp"

// Essential object types (for advanced use)
#include "zephyr/objects/object.hpp"

/**
 * @file zephyr.hpp
 * @brief Main convenience header for the Zephyr C++ API
 * 
 * This header provides all the essential components needed to:
 * - Embed Zephyr in C++ applications
 * - Create plugins for Zephyr
 * - Convert between C++ and Zephyr types
 * - Handle errors and results
 * 
 * @section quick_start Quick Start
 * 
 * For basic embedding:
 * @code
 * #include "zephyr/zephyr.hpp"
 * 
 * int main() {
 *     auto engine = zephyr::api::create_engine();
 *     
 *     // Register C++ function
 *     engine->register_function("add", [](double a, double b) -> double {
 *         return a + b;
 *     });
 *     
 *     // Execute Zephyr code
 *     auto result = engine->execute_string(R"(
 *         result = add(5.0, 3.0)
 *         return result
 *     )");
 *     
 *     if (result) {
 *         std::cout << "Result: " << result.value()->to_string() << std::endl;
 *     }
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * For plugin development:
 * @code
 * #include "zephyr/zephyr.hpp"
 * 
 * class my_plugin_t : public zephyr::api::plugin_interface_t {
 *     // Implement plugin interface...
 * };
 * 
 * ZEPHYR_PLUGIN(my_plugin_t)
 * @endcode
 * 
 * @section components Main Components
 * 
 * - zephyr::api::engine_t - Main engine for executing Zephyr code
 * - zephyr::api::plugin_interface_t - Base class for plugins
 * - zephyr::api::native_module_t - Module creation for plugins
 * - zephyr::api::make_native_function() - Function registration helpers
 * - zephyr::api::to_zephyr() / from_zephyr() - Type conversion
 * - zephyr::api::result_t - Error handling
 * 
 * @section namespace Namespace
 * All Zephyr API components are in the zephyr::api namespace.
 * 
 * @section compatibility Compatibility
 * This API requires C++17 or later and is compatible with:
 * - Windows (MSVC, MinGW)
 * - Linux (GCC, Clang)  
 * - macOS (Clang)
 * 
 * @version 1.0.0
 * @author Zephyr Team
 * @copyright MIT License
 */

namespace zephyr {

/**
 * @brief Main API namespace
 * 
 * Contains all the public API components for embedding Zephyr
 * and creating plugins.
 */
namespace api {
    // All API components are already included above
    
    /**
     * @brief Quick convenience aliases for common types
     */
    using Engine = engine_t;
    using Plugin = plugin_interface_t;
    using Module = native_module_t;
    using Result = result_t<value_t>;
    using VoidResult = result_t<void>;
    
    /**
     * @brief Quick access to commonly used functions
     */
    
    // Create engine (convenience alias)
    inline auto make_engine() -> std::unique_ptr<engine_t> {
        return create_engine();
    }
    
    // Quick execute (using default engine)
    inline auto execute(const std::string& code) -> value_result_t {
        return default_engine().execute_string(code);
    }
    
    // Quick function registration (using default engine)
    template<typename F>
    inline auto register_function(const std::string& name, F&& func) -> void {
        default_engine().register_function(name, std::forward<F>(func));
    }
    
} // namespace api

/**
 * @brief Global convenience namespace
 * 
 * Provides quick access to the most commonly used functionality
 * without needing to specify the full zephyr::api namespace.
 */
namespace quick {
    using namespace api;
    
    /**
     * @brief Ultra-quick engine creation and script execution
     * 
     * @code
     * auto result = zephyr::quick::run("print('Hello World')");
     * @endcode
     */
    inline auto run(const std::string& code) -> value_result_t {
        static auto engine = create_engine();
        return engine->execute_string(code);
    }
    
} // namespace quick

} // namespace zephyr

/**
 * @def ZEPHYR_VERSION_MAJOR
 * @brief Major version number
 */
#ifndef ZEPHYR_VERSION_MAJOR
#define ZEPHYR_VERSION_MAJOR 2025
#endif

/**
 * @def ZEPHYR_VERSION_MINOR  
 * @brief Minor version number
 */
#ifndef ZEPHYR_VERSION_MINOR
#define ZEPHYR_VERSION_MINOR 9
#endif

/**
 * @def ZEPHYR_VERSION_PATCH
 * @brief Patch version number
 */
#ifndef ZEPHYR_VERSION_PATCH
#define ZEPHYR_VERSION_PATCH 14
#endif

/**
 * @def ZEPHYR_VERSION_STRING
 * @brief Full version string
 */
#ifndef ZEPHYR_VERSION_STRING
#define ZEPHYR_VERSION_STRING "v2025-09-14"
#endif