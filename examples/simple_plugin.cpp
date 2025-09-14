#include "zephyr/zephyr.hpp"
#include <cmath>
#include <iostream>

namespace zephyr::api {

// Simple math plugin implementation
class simple_math_plugin_t : public plugin_interface_t {
public:
    simple_math_plugin_t() {
        std::cout << "[DEBUG] simple_math_plugin_t constructor called" << std::endl;
    }
    
    ~simple_math_plugin_t() {
        std::cout << "[DEBUG] simple_math_plugin_t destructor called" << std::endl;
    }

    auto get_plugin_info() const -> plugin_info_t override {
        plugin_info_t info;
        info.name = "simple_math";
        info.description = "Basic mathematical functions for Zephyr";
        info.author = "Zephyr Team";
        info.version = {1, 0, 0};
        info.min_zephyr_version = {1, 0, 0};
        info.license = "MIT";
        return info;
    }
    
    auto initialize(engine_t* engine) -> plugin_result_t override {
        // Allow null engine for runtime scenarios without full engine
        m_engine = engine;
        return plugin_result_t::success();
    }
    
    auto finalize() -> void override {
        // Nothing to clean up in this simple example
    }
    
    auto create_module() -> std::shared_ptr<native_module_t> override {
        std::cout << "[DEBUG] create_module() called" << std::endl;
        auto module = std::make_shared<native_module_t>();
        module->set_name("simple_math");
        module->set_description("Basic mathematical operations");
        module->set_version("1.0.0");
        
        // Export simple mathematical functions
        
        // Addition function
        module->export_function("add", make_native_function(
            [](double a, double b) -> double {
                return a + b;
            }, "add"
        ));
        
        // Subtraction function
        module->export_function("subtract", make_native_function(
            [](double a, double b) -> double {
                return a - b;
            }, "subtract"
        ));
        
        // Multiplication function
        module->export_function("multiply", make_native_function(
            [](double a, double b) -> double {
                return a * b;
            }, "multiply"
        ));
        
        // Division function with error checking
        module->export_function("divide", make_native_function(
            [](double a, double b) -> double {
                if (b == 0.0) {
                    throw std::runtime_error("Division by zero");
                }
                return a / b;
            }, "divide"
        ));
        
        // Power function
        module->export_function("power", make_native_function(
            [](double base, double exponent) -> double {
                return std::pow(base, exponent);
            }, "power"
        ));
        
        // Square root function
        module->export_function("sqrt", make_native_function(
            [](double x) -> double {
                if (x < 0) {
                    throw std::runtime_error("Cannot take square root of negative number");
                }
                return std::sqrt(x);
            }, "sqrt"
        ));
        
        // Absolute value function
        module->export_function("abs", make_native_function(
            [](double x) -> double {
                return std::abs(x);
            }, "abs"
        ));
        
        // Maximum of two numbers
        module->export_function("max", make_native_function(
            [](double a, double b) -> double {
                return std::max(a, b);
            }, "max"
        ));
        
        // Minimum of two numbers
        module->export_function("min", make_native_function(
            [](double a, double b) -> double {
                return std::min(a, b);
            }, "min"
        ));
        
        // Factorial function (integer input)
        module->export_function("factorial", make_native_function(
            [](int n) -> int {
                if (n < 0) {
                    throw std::runtime_error("Factorial is not defined for negative numbers");
                }
                if (n == 0 || n == 1) {
                    return 1;
                }
                int result = 1;
                for (int i = 2; i <= n; ++i) {
                    result *= i;
                }
                return result;
            }, "factorial"
        ));
        
        // String concatenation function
        module->export_function("concat", make_native_function(
            [](const std::string& a, const std::string& b) -> std::string {
                return a + b;
            }, "concat"
        ));
        
        // String length function
        module->export_function("string_length", make_native_function(
            [](const std::string& s) -> int {
                return static_cast<int>(s.length());
            }, "string_length"
        ));
        
        // Export some mathematical constants
        module->export_constant("PI", to_zephyr(3.141592653589793));
        module->export_constant("E", to_zephyr(2.718281828459045));
        module->export_constant("SQRT2", to_zephyr(1.4142135623730951));
        
        return module;
    }
    
private:
    engine_t* m_engine = nullptr;
};

} // namespace zephyr::api

// Plugin exports
ZEPHYR_PLUGIN(zephyr::api::simple_math_plugin_t)