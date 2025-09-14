#include "zephyr/zephyr.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <cmath>

using namespace zephyr::api;

// Simple example class to demonstrate C++ class binding
class Vector2D {
public:
    Vector2D(double x = 0.0, double y = 0.0) : m_x(x), m_y(y) {}
    
    double get_x() const { return m_x; }
    double get_y() const { return m_y; }
    void set_x(double x) { m_x = x; }
    void set_y(double y) { m_y = y; }
    
    double magnitude() const {
        return std::sqrt(m_x * m_x + m_y * m_y);
    }
    
    Vector2D add(const Vector2D& other) const {
        return Vector2D(m_x + other.m_x, m_y + other.m_y);
    }
    
    std::string to_string() const {
        return "Vector2D(" + std::to_string(m_x) + ", " + std::to_string(m_y) + ")";
    }

private:
    double m_x, m_y;
};

int main() {
    std::cout << "=== Zephyr C++ API Demo ===" << std::endl;
    
    try {
        // Create a Zephyr engine
        auto engine = create_engine();
        
        std::cout << "Engine created successfully!" << std::endl;
        std::cout << "Engine name: " << engine->get_name() << std::endl;
        std::cout << "Engine version: " << engine->get_version() << std::endl;
        
        // Register some simple C++ functions
        std::cout << "\n--- Registering C++ Functions ---" << std::endl;
        
        engine->register_function("add_numbers", [](double a, double b) -> double {
            return a + b;
        });
        
        engine->register_function("multiply", [](double a, double b) -> double {
            return a * b;
        });
        
        engine->register_function("greet", [](const std::string& name) -> std::string {
            return "Hello, " + name + "!";
        });
        
        engine->register_function("fibonacci", [](int n) -> int {
            if (n <= 1) return n;
            int a = 0, b = 1;
            for (int i = 2; i <= n; ++i) {
                int temp = a + b;
                a = b;
                b = temp;
            }
            return b;
        });
        
        std::cout << "Registered functions: ";
        for (const auto& func_name : engine->list_functions()) {
            std::cout << func_name << " ";
        }
        std::cout << std::endl;
        
        // Set some global variables
        std::cout << "\n--- Setting Global Variables ---" << std::endl;
        engine->set_global("version", std::string("1.0.0"));
        engine->set_global("debug_mode", true);
        engine->set_global("max_iterations", 1000);
        
        std::cout << "Set global variables: ";
        for (const auto& var_name : engine->list_globals()) {
            std::cout << var_name << " ";
        }
        std::cout << std::endl;
        
        // Execute some Zephyr code
        std::cout << "\n--- Executing Zephyr Code ---" << std::endl;
        
        // Test basic arithmetic
        auto result1 = engine->execute_string(R"(
            result = add_numbers(5.5, 3.2)
            print("5.5 + 3.2 = " + str(result))
            return result
        )");
        
        if (result1) {
            std::cout << "Arithmetic result: " << result1.value()->to_string() << std::endl;
        } else {
            std::cout << "Arithmetic failed: " << result1.error() << std::endl;
        }
        
        // Test string operations
        auto result2 = engine->execute_string(R"(
            greeting = greet("World")
            print(greeting)
            return greeting
        )");
        
        if (result2) {
            std::cout << "Greeting result: " << result2.value()->to_string() << std::endl;
        } else {
            std::cout << "Greeting failed: " << result2.error() << std::endl;
        }
        
        // Test fibonacci calculation
        auto result3 = engine->execute_string(R"(
            fib_10 = fibonacci(10)
            print("Fibonacci(10) = " + str(fib_10))
            return fib_10
        )");
        
        if (result3) {
            std::cout << "Fibonacci result: " << result3.value()->to_string() << std::endl;
        } else {
            std::cout << "Fibonacci failed: " << result3.error() << std::endl;
        }
        
        // Test global variable access
        auto result4 = engine->execute_string(R"(
            print("Version: " + version)
            print("Debug mode: " + str(debug_mode))
            print("Max iterations: " + str(max_iterations))
            return version + " (debug: " + str(debug_mode) + ")"
        )");
        
        if (result4) {
            std::cout << "Global vars result: " << result4.value()->to_string() << std::endl;
        } else {
            std::cout << "Global vars failed: " << result4.error() << std::endl;
        }
        
        // Test function calls from C++
        std::cout << "\n--- Calling Functions from C++ ---" << std::endl;
        
        auto call_result = engine->call_function("multiply", 6.0, 7.0);
        if (call_result) {
            std::cout << "6 * 7 = " << call_result.value()->to_string() << std::endl;
        } else {
            std::cout << "Function call failed: " << call_result.error() << std::endl;
        }
        
        // Test complex expression evaluation
        std::cout << "\n--- Evaluating Complex Expressions ---" << std::endl;
        
        auto expr_result = engine->evaluate_expression("add_numbers(multiply(3, 4), fibonacci(5))");
        if (expr_result) {
            std::cout << "add_numbers(multiply(3, 4), fibonacci(5)) = " << expr_result.value()->to_string() << std::endl;
        } else {
            std::cout << "Expression evaluation failed: " << expr_result.error() << std::endl;
        }
        
        // Display engine statistics
        std::cout << "\n--- Engine Statistics ---" << std::endl;
        auto stats = engine->get_stats();
        std::cout << "Functions registered: " << stats.functions_registered << std::endl;
        std::cout << "Classes registered: " << stats.classes_registered << std::endl;
        std::cout << "Modules registered: " << stats.modules_registered << std::endl;
        std::cout << "Scripts executed: " << stats.scripts_executed << std::endl;
        std::cout << "Expressions evaluated: " << stats.expressions_evaluated << std::endl;
        std::cout << "Total execution time: " << stats.total_execution_time.count() << "ms" << std::endl;
        
        // Test plugin loading (if available)
        std::cout << "\n--- Plugin System Test ---" << std::endl;
        std::cout << "Library search paths: ";
        for (const auto& path : engine->get_library_search_paths()) {
            std::cout << path << " ";
        }
        std::cout << std::endl;
        
        // Try to load a plugin (this might fail if no plugin is available)
        std::cout << "Attempting to load simple_math plugin..." << std::endl;
        auto plugin_result = engine->load_plugin("simple_math.so");
        if (plugin_result) {
            std::cout << "Plugin loaded successfully!" << std::endl;
            auto plugin_info = plugin_result.value()->get_plugin_info();
            std::cout << "Plugin info: " << plugin_info.to_string() << std::endl;
            
            // Test using plugin functions
            auto plugin_test = engine->execute_string(R"(
                import simple_math
                result = simple_math.add(10, 20)
                print("Plugin add(10, 20) = " + str(result))
                return result
            )");
            
            if (plugin_test) {
                std::cout << "Plugin function result: " << plugin_test.value()->to_string() << std::endl;
            }
        } else {
            std::cout << "Plugin loading failed (expected if plugin not built): " << plugin_result.error() << std::endl;
        }
        
        // Test error handling
        std::cout << "\n--- Error Handling Test ---" << std::endl;
        
        auto error_result = engine->execute_string("nonexistent_function(123)");
        if (!error_result) {
            std::cout << "Error handled correctly: " << error_result.error() << std::endl;
        }
        
        // Final message
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "The Zephyr C++ API is working successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}