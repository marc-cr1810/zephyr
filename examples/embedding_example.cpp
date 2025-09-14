#include "zephyr/zephyr.hpp"
#include <iostream>
#include <vector>
#include <chrono>

using namespace zephyr::api;

// =============================================================================
// Custom C++ Functions to Register
// =============================================================================

// Simple math functions
double multiply(double a, double b) {
    return a * b;
}

int factorial(int n) {
    if (n < 0) throw std::invalid_argument("Factorial of negative number");
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

std::string reverse_string(const std::string& str) {
    return std::string(str.rbegin(), str.rend());
}

std::vector<int> range(int start, int end, int step = 1) {
    if (step == 0) throw std::invalid_argument("Step cannot be zero");
    
    std::vector<int> result;
    if (step > 0) {
        for (int i = start; i < end; i += step) {
            result.push_back(i);
        }
    } else {
        for (int i = start; i > end; i += step) {
            result.push_back(i);
        }
    }
    return result;
}

// =============================================================================
// Custom C++ Class Example
// =============================================================================

class timer_t {
public:
    timer_t() : m_start_time(std::chrono::steady_clock::now()) {}
    
    auto reset() -> void {
        m_start_time = std::chrono::steady_clock::now();
    }
    
    auto elapsed_ms() const -> double {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time);
        return static_cast<double>(duration.count());
    }
    
    auto elapsed_seconds() const -> double {
        return elapsed_ms() / 1000.0;
    }
    
    auto to_string() const -> std::string {
        return "Timer(elapsed: " + std::to_string(elapsed_ms()) + "ms)";
    }

private:
    std::chrono::steady_clock::time_point m_start_time;
};

// =============================================================================
// Custom Module Example
// =============================================================================

class utilities_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("utilities");
        set_version("1.0.0");
        set_description("Utility functions for common tasks");
        set_author("Embedding Example");
        
        // String utilities
        ZEPHYR_FUNCTION("reverse", reverse_string);
        
        export_function("join", make_native_function([](const std::vector<std::string>& strings, const std::string& separator) -> std::string {
            if (strings.empty()) return "";
            
            std::string result = strings[0];
            for (size_t i = 1; i < strings.size(); ++i) {
                result += separator + strings[i];
            }
            return result;
        }));
        
        export_function("split", make_native_function([](const std::string& str, const std::string& delimiter) -> std::vector<std::string> {
            std::vector<std::string> result;
            size_t start = 0;
            size_t end = str.find(delimiter);
            
            while (end != std::string::npos) {
                result.push_back(str.substr(start, end - start));
                start = end + delimiter.length();
                end = str.find(delimiter, start);
            }
            
            result.push_back(str.substr(start));
            return result;
        }));
        
        // Array utilities
        ZEPHYR_FUNCTION("range", range);
        
        export_function("sum_array", make_native_function([](const std::vector<double>& numbers) -> double {
            double total = 0.0;
            for (double num : numbers) {
                total += num;
            }
            return total;
        }));
        
        // Constants
        export_constant("VERSION", to_zephyr("1.0.0"));
        export_constant("MAX_SIZE", to_zephyr(1000));
        
        return void_result_t::success();
    }
};

// =============================================================================
// Main Example Program
// =============================================================================

void demonstrate_basic_api() {
    std::cout << "=== Basic API Demonstration ===" << std::endl;
    
    // Create engine
    auto engine = create_engine();
    
    // Register simple functions
    engine->register_function("multiply", multiply);
    engine->register_function("factorial", factorial);
    
    // Set global variables
    engine->set_global("app_name", std::string("Zephyr Embedding Example"));
    engine->set_global("version", 1.0);
    
    // Execute simple Zephyr code
    auto result = engine->execute_string(R"(
        print("Welcome to", app_name, "version", version)
        
        # Test registered functions
        result1 = multiply(6.5, 4.2)
        print("6.5 * 4.2 =", result1)
        
        fact5 = factorial(5)
        print("5! =", fact5)
        
        # Return a value
        return {"result1": result1, "factorial": fact5}
    )");
    
    if (result) {
        std::cout << "Script executed successfully!" << std::endl;
        // You could convert the result back to C++ types here
    } else {
        std::cout << "Script execution failed: " << result.error() << std::endl;
    }
}

void demonstrate_class_registration() {
    std::cout << "\n=== Class Registration Demonstration ===" << std::endl;
    
    auto engine = create_engine();
    
    // Register Timer class
    auto timer_class = engine->register_class<timer_t>("Timer");
    
    // Add methods to the Timer class
    // Note: In a real implementation, you'd need proper method binding
    // This is simplified for the example
    
    auto result = engine->execute_string(R"(
        # Create timer instance
        timer = Timer()
        
        # Simulate some work
        sum = 0
        for i in range(1000000) {
            sum += i
        }
        
        # Check elapsed time
        elapsed = timer.elapsed_ms()
        print("Calculation took", elapsed, "milliseconds")
        print("Sum =", sum)
    )");
    
    if (!result) {
        std::cout << "Class demo failed: " << result.error() << std::endl;
    }
}

void demonstrate_module_registration() {
    std::cout << "\n=== Module Registration Demonstration ===" << std::endl;
    
    auto engine = create_engine();
    
    // Register custom module
    auto utilities = std::make_shared<utilities_module_t>();
    engine->register_module("utilities", utilities);
    
    auto result = engine->execute_string(R"(
        import utilities
        
        # Test string functions
        original = "Hello, World!"
        reversed = utilities.reverse(original)
        print("Original:", original)
        print("Reversed:", reversed)
        
        # Test array functions
        words = ["apple", "banana", "cherry"]
        joined = utilities.join(words, ", ")
        print("Joined:", joined)
        
        split_result = utilities.split(joined, ", ")
        print("Split result:", split_result)
        
        # Test range function
        numbers = utilities.range(1, 10, 2)
        print("Range 1 to 10 step 2:", numbers)
        
        # Convert to doubles and sum
        doubles = []
        for num in numbers {
            doubles.append(float(num))
        }
        total = utilities.sum_array(doubles)
        print("Sum:", total)
        
        # Access constants
        print("Utilities version:", utilities.VERSION)
        print("Max size:", utilities.MAX_SIZE)
    )");
    
    if (!result) {
        std::cout << "Module demo failed: " << result.error() << std::endl;
    }
}

void demonstrate_plugin_loading() {
    std::cout << "\n=== Plugin Loading Demonstration ===" << std::endl;
    
    auto engine = create_engine();
    
    // Add search path for plugins
    engine->add_library_search_path("./plugins");
    engine->add_library_search_path("../plugins");
    
    // Try to load math plugin
    auto plugin_result = engine->load_plugin("math_plugin.so");
    if (plugin_result) {
        std::cout << "Math plugin loaded successfully!" << std::endl;
        
        auto result = engine->execute_string(R"(
            # Import from the loaded plugin
            import advanced_math
            
            # Test basic math functions
            angle = advanced_math.PI / 4
            sin_val = advanced_math.sin(angle)
            cos_val = advanced_math.cos(angle)
            
            print("sin(π/4) =", sin_val)
            print("cos(π/4) =", cos_val)
            
            # Test vector class
            v1 = advanced_math.Vector3D(1.0, 0.0, 0.0)
            v2 = advanced_math.Vector3D(0.0, 1.0, 0.0)
            
            print("Vector 1:", v1.to_string())
            print("Vector 2:", v2.to_string())
            print("V1 magnitude:", v1.magnitude())
            
            # Cross product
            cross = v1.cross(v2)
            print("V1 × V2 =", cross.to_string())
            
            # Test statistical functions
            data = [1.0, 2.0, 3.0, 4.0, 5.0]
            mean_val = advanced_math.mean(data)
            print("Mean of", data, "=", mean_val)
            
            # Test random functions
            random_val = advanced_math.random()
            random_int = advanced_math.randint(1, 10)
            print("Random value:", random_val)
            print("Random int (1-10):", random_int)
        )");
        
        if (!result) {
            std::cout << "Plugin usage failed: " << result.error() << std::endl;
        }
    } else {
        std::cout << "Failed to load math plugin: " << plugin_result.error() << std::endl;
        std::cout << "Make sure math_plugin.so is built and in the plugins directory" << std::endl;
    }
}

void demonstrate_named_imports() {
    std::cout << "\n=== Named Import Demonstration ===" << std::endl;
    
    auto engine = create_engine();
    
    // Add search path for plugins
    engine->add_library_search_path("./plugins");
    
    auto plugin_result = engine->load_plugin("math_plugin.so");
    if (plugin_result) {
        auto result = engine->execute_string(R"(
            # Named imports from plugin
            import sin, cos, PI, sqrt from "math_plugin.so"
            
            # Use imported functions directly
            angle = PI / 6  # 30 degrees
            sin_30 = sin(angle)
            cos_30 = cos(angle)
            
            print("sin(30°) =", sin_30)
            print("cos(30°) =", cos_30)
            
            # Test sqrt
            sqrt_16 = sqrt(16.0)
            print("sqrt(16) =", sqrt_16)
            
            # Namespace import
            import * as math from "math_plugin.so"
            
            # Use with namespace
            result = math.pow(2.0, 8.0)
            print("2^8 =", result)
        )");
        
        if (!result) {
            std::cout << "Named import demo failed: " << result.error() << std::endl;
        }
    }
}

void demonstrate_error_handling() {
    std::cout << "\n=== Error Handling Demonstration ===" << std::endl;
    
    auto engine = create_engine();
    
    // Set custom error handler
    engine->set_error_handler([](const error_info_t& error) {
        std::cout << "Custom Error Handler: " << error.to_string() << std::endl;
    });
    
    // Register function that can throw
    engine->register_function("divide", [](double a, double b) -> double {
        if (b == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return a / b;
    });
    
    // Test error handling
    auto result = engine->execute_string(R"(
        # This should work
        result1 = divide(10.0, 2.0)
        print("10 / 2 =", result1)
        
        # This should cause an error
        try {
            result2 = divide(10.0, 0.0)
            print("This shouldn't print")
        } catch (e) {
            print("Caught error:", e)
        }
    )");
    
    if (!result) {
        std::cout << "Error handling test failed: " << result.error() << std::endl;
    }
}

void demonstrate_performance() {
    std::cout << "\n=== Performance Demonstration ===" << std::endl;
    
    auto engine = create_engine();
    
    // Register performance-critical function
    engine->register_function("fast_sum", [](const std::vector<double>& numbers) -> double {
        double sum = 0.0;
        for (double num : numbers) {
            sum += num;
        }
        return sum;
    });
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto result = engine->execute_string(R"(
        # Create large array
        large_array = []
        for i in range(100000) {
            large_array.append(float(i))
        }
        
        # Use native function for performance
        total = fast_sum(large_array)
        print("Sum of 0-99999 =", total)
        
        return total
    )");
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    if (result) {
        std::cout << "Performance test completed in " << duration.count() << "ms" << std::endl;
    } else {
        std::cout << "Performance test failed: " << result.error() << std::endl;
    }
}

int main() {
    try {
        std::cout << "Zephyr Plugin API Demonstration" << std::endl;
        std::cout << "===============================" << std::endl;
        
        demonstrate_basic_api();
        demonstrate_class_registration();
        demonstrate_module_registration();
        demonstrate_plugin_loading();
        demonstrate_named_imports();
        demonstrate_error_handling();
        demonstrate_performance();
        
        std::cout << "\n=== Engine Statistics ===" << std::endl;
        auto engine = create_engine();
        auto stats = engine->get_stats();
        
        std::cout << "Functions registered: " << stats.functions_registered << std::endl;
        std::cout << "Classes registered: " << stats.classes_registered << std::endl;
        std::cout << "Modules registered: " << stats.modules_registered << std::endl;
        std::cout << "Plugins loaded: " << stats.plugins_loaded << std::endl;
        std::cout << "Scripts executed: " << stats.scripts_executed << std::endl;
        
        std::cout << "\nDemo completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Demo failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}