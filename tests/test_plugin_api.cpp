#include "zephyr/api/zephyr_api.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/api/plugin_interface.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace zephyr::api;

// =============================================================================
// Test Utilities
// =============================================================================

void test_assert(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "TEST FAILED: " << message << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: " << message << std::endl;
}

template<typename T>
void test_equal(const T& expected, const T& actual, const std::string& message) {
    if (expected != actual) {
        std::cerr << "TEST FAILED: " << message << std::endl;
        std::cerr << "  Expected: " << expected << std::endl;
        std::cerr << "  Actual: " << actual << std::endl;
        std::exit(1);
    }
    std::cout << "PASS: " << message << std::endl;
}

// =============================================================================
// Type Conversion Tests
// =============================================================================

void test_type_conversions() {
    std::cout << "\n=== Testing Type Conversions ===" << std::endl;
    
    // Test basic types
    {
        auto int_val = to_zephyr(42);
        auto converted_back = from_zephyr<int>(int_val);
        test_assert(converted_back.is_success(), "Integer conversion roundtrip");
        test_equal(42, converted_back.value(), "Integer value preservation");
    }
    
    {
        auto double_val = to_zephyr(3.14159);
        auto converted_back = from_zephyr<double>(double_val);
        test_assert(converted_back.is_success(), "Double conversion roundtrip");
        test_assert(std::abs(converted_back.value() - 3.14159) < 1e-6, "Double value preservation");
    }
    
    {
        std::string test_str = "Hello, Zephyr!";
        auto str_val = to_zephyr(test_str);
        auto converted_back = from_zephyr<std::string>(str_val);
        test_assert(converted_back.is_success(), "String conversion roundtrip");
        test_equal(test_str, converted_back.value(), "String value preservation");
    }
    
    {
        auto bool_val = to_zephyr(true);
        auto converted_back = from_zephyr<bool>(bool_val);
        test_assert(converted_back.is_success(), "Boolean conversion roundtrip");
        test_equal(true, converted_back.value(), "Boolean value preservation");
    }
    
    // Test container types
    {
        std::vector<int> test_vec = {1, 2, 3, 4, 5};
        auto vec_val = to_zephyr(test_vec);
        auto converted_back = from_zephyr<std::vector<int>>(vec_val);
        test_assert(converted_back.is_success(), "Vector<int> conversion roundtrip");
        test_equal(test_vec.size(), converted_back.value().size(), "Vector size preservation");
        
        for (size_t i = 0; i < test_vec.size(); ++i) {
            test_equal(test_vec[i], converted_back.value()[i], "Vector element " + std::to_string(i));
        }
    }
    
    {
        std::map<std::string, double> test_map = {
            {"pi", 3.14159},
            {"e", 2.71828},
            {"phi", 1.61803}
        };
        auto map_val = to_zephyr(test_map);
        auto converted_back = from_zephyr<std::map<std::string, double>>(map_val);
        test_assert(converted_back.is_success(), "Map<string,double> conversion roundtrip");
        test_equal(test_map.size(), converted_back.value().size(), "Map size preservation");
        
        for (const auto& [key, value] : test_map) {
            auto it = converted_back.value().find(key);
            test_assert(it != converted_back.value().end(), "Map contains key: " + key);
            test_assert(std::abs(it->second - value) < 1e-6, "Map value for key: " + key);
        }
    }
}

// =============================================================================
// Native Function Tests
// =============================================================================

void test_native_functions() {
    std::cout << "\n=== Testing Native Functions ===" << std::endl;
    
    // Simple function
    auto add_func = make_native_function([](int a, int b) -> int {
        return a + b;
    });
    
    std::vector<value_t> args = {to_zephyr(5), to_zephyr(3)};
    auto result = add_func(args);
    
    test_assert(result.is_success(), "Simple function execution");
    auto int_result = from_zephyr<int>(result.value());
    test_assert(int_result.is_success(), "Function result conversion");
    test_equal(8, int_result.value(), "Function result value");
    
    // Function with error handling
    auto divide_func = make_native_function([](double a, double b) -> double {
        if (b == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return a / b;
    });
    
    // Test normal case
    std::vector<value_t> divide_args = {to_zephyr(10.0), to_zephyr(2.0)};
    auto divide_result = divide_func(divide_args);
    test_assert(divide_result.is_success(), "Division function normal case");
    
    auto double_result = from_zephyr<double>(divide_result.value());
    test_assert(double_result.is_success(), "Division result conversion");
    test_assert(std::abs(double_result.value() - 5.0) < 1e-6, "Division result value");
    
    // Test error case
    std::vector<value_t> divide_error_args = {to_zephyr(10.0), to_zephyr(0.0)};
    auto divide_error_result = divide_func(divide_error_args);
    test_assert(divide_error_result.is_error(), "Division by zero error handling");
    
    // Function with containers
    auto sum_func = make_native_function([](const std::vector<double>& numbers) -> double {
        double sum = 0.0;
        for (double num : numbers) {
            sum += num;
        }
        return sum;
    });
    
    std::vector<value_t> sum_args = {to_zephyr(std::vector<double>{1.5, 2.5, 3.0, 4.0})};
    auto sum_result = sum_func(sum_args);
    test_assert(sum_result.is_success(), "Container function execution");
    
    auto sum_double_result = from_zephyr<double>(sum_result.value());
    test_assert(sum_double_result.is_success(), "Container function result conversion");
    test_assert(std::abs(sum_double_result.value() - 11.0) < 1e-6, "Container function result value");
}

// =============================================================================
// Native Module Tests
// =============================================================================

class test_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("test_module");
        set_version("1.0.0");
        set_description("Test module for plugin API");
        
        // Add functions
        export_function("multiply", make_native_function([](double a, double b) -> double {
            return a * b;
        }));
        
        export_function("greet", make_native_function([](const std::string& name) -> std::string {
            return "Hello, " + name + "!";
        }));
        
        export_function("max", make_native_function([](const std::vector<double>& numbers) -> double {
            if (numbers.empty()) {
                throw std::runtime_error("Cannot find max of empty list");
            }
            return *std::max_element(numbers.begin(), numbers.end());
        }));
        
        // Add constants
        export_constant("VERSION", to_zephyr("1.0.0"));
        export_constant("PI", to_zephyr(3.14159265359));
        
        return void_result_t::success();
    }
};

void test_native_modules() {
    std::cout << "\n=== Testing Native Modules ===" << std::endl;
    
    auto module = std::make_shared<test_module_t>();
    auto init_result = module->initialize();
    test_assert(init_result.is_success(), "Module initialization");
    
    // Test function access
    auto multiply_func = module->get_function("multiply");
    test_assert(multiply_func.has_value(), "Module function retrieval");
    
    std::vector<value_t> multiply_args = {to_zephyr(6.0), to_zephyr(7.0)};
    auto multiply_result = multiply_func.value()(multiply_args);
    test_assert(multiply_result.is_success(), "Module function execution");
    
    auto result_val = from_zephyr<double>(multiply_result.value());
    test_assert(result_val.is_success(), "Module function result conversion");
    test_assert(std::abs(result_val.value() - 42.0) < 1e-6, "Module function result value");
    
    // Test constant access
    auto pi_const = module->get_constant("PI");
    test_assert(pi_const.has_value(), "Module constant retrieval");
    
    auto pi_val = from_zephyr<double>(pi_const.value());
    test_assert(pi_val.is_success(), "Module constant conversion");
    test_assert(std::abs(pi_val.value() - 3.14159265359) < 1e-10, "Module constant value");
    
    // Test symbol listing
    auto exported_symbols = module->get_exported_symbols();
    test_assert(exported_symbols.size() >= 5, "Module exports count"); // 3 functions + 2 constants
    
    bool has_multiply = std::find(exported_symbols.begin(), exported_symbols.end(), "multiply") != exported_symbols.end();
    test_assert(has_multiply, "Module exports multiply function");
    
    bool has_pi = std::find(exported_symbols.begin(), exported_symbols.end(), "PI") != exported_symbols.end();
    test_assert(has_pi, "Module exports PI constant");
}

// =============================================================================
// Engine Integration Tests
// =============================================================================

void test_engine_integration() {
    std::cout << "\n=== Testing Engine Integration ===" << std::endl;
    
    auto engine = create_engine();
    test_assert(engine != nullptr, "Engine creation");
    
    // Test function registration
    engine->register_function("test_add", [](int a, int b) -> int {
        return a + b;
    });
    
    test_assert(engine->has_function("test_add"), "Function registration check");
    
    // Test function execution
    auto call_result = engine->call_function("test_add", {to_zephyr(15), to_zephyr(27)});
    test_assert(call_result.is_success(), "Engine function call");
    
    auto add_result = from_zephyr<int>(call_result.value());
    test_assert(add_result.is_success(), "Engine function result conversion");
    test_equal(42, add_result.value(), "Engine function result value");
    
    // Test module registration
    auto test_module = std::make_shared<test_module_t>();
    engine->register_module("test_module", test_module);
    
    test_assert(engine->has_module("test_module"), "Module registration check");
    
    auto retrieved_module = engine->get_module("test_module");
    test_assert(retrieved_module != nullptr, "Module retrieval");
    test_equal(std::string("test_module"), retrieved_module->get_name(), "Retrieved module name");
    
    // Test global variable setting/getting
    engine->set_global("test_var", 123);
    auto test_var = engine->get_global<int>("test_var");
    test_assert(test_var.has_value(), "Global variable retrieval");
    test_equal(123, test_var.value(), "Global variable value");
    
    // Test string execution (basic) - commented out as it requires full parser
    // auto exec_result = engine->execute_string("result = 2 + 3; return result");
    // test_assert(exec_result.is_success(), "Basic script execution");
    
    // Test expression evaluation - commented out as it requires full parser
    // auto eval_result = engine->evaluate_expression("10 * 5");
    // test_assert(eval_result.is_success(), "Expression evaluation");
    
    // auto eval_val = from_zephyr<int>(eval_result.value());
    // test_assert(eval_val.is_success(), "Expression result conversion");
    // test_equal(50, eval_val.value(), "Expression result value");
}

// =============================================================================
// Error Handling Tests
// =============================================================================

void test_error_handling() {
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    
    // Test type conversion errors
    auto int_val = to_zephyr(42);
    auto wrong_conversion = from_zephyr<std::string>(int_val);
    test_assert(wrong_conversion.is_error(), "Type mismatch error detection");
    
    // Test function argument count errors
    auto strict_func = make_native_function([](int a, int b) -> int {
        return a + b;
    });
    
    // Wrong number of arguments
    std::vector<value_t> wrong_args = {to_zephyr(5)}; // Only 1 arg instead of 2
    auto wrong_result = strict_func(wrong_args);
    test_assert(wrong_result.is_error(), "Argument count error detection");
    
    // Test exception propagation
    auto throwing_func = make_native_function([]() -> int {
        throw std::runtime_error("Test exception");
    });
    
    std::vector<value_t> no_args;
    auto exception_result = throwing_func(no_args);
    test_assert(exception_result.is_error(), "Exception error propagation");
    
    // Test error info creation
    auto error_info = create_error_info("Test error message", "TestError", "test_plugin");
    test_equal(std::string("Test error message"), error_info.message, "Error message preservation");
    test_equal(std::string("TestError"), error_info.type, "Error type preservation");
    test_equal(std::string("test_plugin"), error_info.plugin_name, "Error plugin name preservation");
}

// =============================================================================
// Performance Tests
// =============================================================================

void test_performance() {
    std::cout << "\n=== Testing Performance ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test many type conversions
    const int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
        auto val = to_zephyr(i);
        auto converted = from_zephyr<int>(val);
        test_assert(converted.is_success(), "Performance test conversion");
        test_equal(i, converted.value(), "Performance test value");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Completed " << iterations << " type conversions in " 
              << duration.count() << "ms" << std::endl;
    
    // Test function calls
    auto perf_func = make_native_function([](int x) -> int {
        return x * x;
    });
    
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::vector<value_t> args = {to_zephyr(i)};
        auto result = perf_func(args);
        test_assert(result.is_success(), "Performance function call");
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Completed " << iterations << " function calls in " 
              << duration.count() << "ms" << std::endl;
}

// =============================================================================
// Main Test Runner
// =============================================================================

int main() {
    std::cout << "Zephyr Plugin API Test Suite" << std::endl;
    std::cout << "=============================" << std::endl;
    
    try {
        test_type_conversions();
        test_native_functions();
        test_native_modules();
        test_engine_integration();
        test_error_handling();
        test_performance();
        
        std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "Plugin API is working correctly!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\nTest suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nTest suite failed with unknown exception" << std::endl;
        return 1;
    }
}