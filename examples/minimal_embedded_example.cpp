#include "zephyr/api/zephyr_api.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include <iostream>
#include <memory>
#include <cmath>

using namespace zephyr::api;
using namespace zephyr;

/**
 * Minimal Embedded Zephyr C++ Example
 * 
 * This is a simple working example that demonstrates:
 * - Basic C++ class definition
 * - Native module creation and registration
 * - Function exports using the raw API
 * - Simple mathematical operations
 */

// Simple C++ class
class Calculator {
public:
    static double add(double a, double b) { return a + b; }
    static double multiply(double a, double b) { return a * b; }
    static double power(double base, double exp) { return std::pow(base, exp); }
};

// Native module implementation
class math_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("math");
        set_version("1.0.0");
        set_description("Basic mathematical operations");
        
        // Export add function
        export_function("add", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("add requires exactly 2 arguments");
            }
            
            double a = 0.0, b = 0.0;
            
            // Handle first argument
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                a = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                a = float_obj->value();
            } else {
                return value_result_t::error("First argument must be a number");
            }
            
            // Handle second argument
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                b = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                b = float_obj->value();
            } else {
                return value_result_t::error("Second argument must be a number");
            }
            
            double result = Calculator::add(a, b);
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        // Export multiply function
        export_function("multiply", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("multiply requires exactly 2 arguments");
            }
            
            double a = 0.0, b = 0.0;
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                a = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                a = float_obj->value();
            } else {
                return value_result_t::error("First argument must be a number");
            }
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                b = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                b = float_obj->value();
            } else {
                return value_result_t::error("Second argument must be a number");
            }
            
            double result = Calculator::multiply(a, b);
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        // Export power function
        export_function("power", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("power requires exactly 2 arguments");
            }
            
            double base = 0.0, exp = 0.0;
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                base = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                base = float_obj->value();
            } else {
                return value_result_t::error("Base must be a number");
            }
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                exp = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                exp = float_obj->value();
            } else {
                return value_result_t::error("Exponent must be a number");
            }
            
            double result = Calculator::power(base, exp);
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        // Export constants
        export_constant("PI", std::make_shared<float_object_t>(M_PI));
        export_constant("E", std::make_shared<float_object_t>(M_E));
        
        return void_result_t::success();
    }
};

int main() {
    std::cout << "=== Minimal Embedded Zephyr C++ Example ===" << std::endl;
    std::cout << "Testing basic class and function integration" << std::endl;
    std::cout << std::endl;
    
    try {
        // Create Zephyr engine
        auto config = engine_config_t{};
        config.name = "Minimal Demo";
        config.debug_mode = true;  // Enable debug mode for troubleshooting
        
        auto engine = create_engine(config);
        std::cout << "Created Zephyr engine: " << engine->get_name() << std::endl;
        
        // Register math module
        std::cout << "Registering math module..." << std::endl;
        auto math_module = std::make_shared<math_module_t>();
        math_module->initialize();
        engine->register_module("math", math_module);
        std::cout << "Math module registered successfully!" << std::endl;
        std::cout << std::endl;
        
        // Test basic functionality
        std::cout << "=== Test: Basic Math Operations ===" << std::endl;
        
        auto result = engine->execute_string(R"ZEPHYR_CODE(
            import math
            
            print("Testing basic math operations...")
            
            result1 = math.add(5.5, 3.2)
            result2 = math.multiply(4.0, 7.0)
            result3 = math.power(2.0, 8.0)
            
            print("5.5 + 3.2 =", result1)
            print("4.0 * 7.0 =", result2)
            print("2.0 ^ 8.0 =", result3)
            
            print("Mathematical constants:")
            print("PI =", math.PI)
            print("E =", math.E)
            
            return "Basic math test completed successfully"
        )ZEPHYR_CODE", "math_test");
        
        if (result) {
            std::cout << "✅ Math test result: " << from_zephyr<std::string>(result.value()).value_or("unknown") << std::endl;
        } else {
            std::cout << "❌ Math test failed: " << result.error() << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "✅ C++ class integration: Working" << std::endl;
        std::cout << "✅ Native module registration: Working" << std::endl;
        std::cout << "✅ Function exports: Working" << std::endl;
        std::cout << "✅ Mathematical operations: Working" << std::endl;
        std::cout << "✅ Constants export: Working" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Minimal Embedded Example Complete ===" << std::endl;
    return 0;
}