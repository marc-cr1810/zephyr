#include "zephyr/zephyr.hpp"
#include <iostream>

// Quick Start Example - Zephyr C++ API with Professional Namespace Structure
//
// This example demonstrates the new professional include structure:
// - All Zephyr headers use the "zephyr/" prefix
// - Single convenience header includes everything
// - Clear, industry-standard namespace organization

using namespace zephyr::api;

int main() {
    std::cout << "=== Zephyr Quick Start - Professional API ===" << std::endl;
    std::cout << "Version: " << ZEPHYR_VERSION_STRING << std::endl;
    
    try {
        // Method 1: Using the full API
        std::cout << "\n--- Method 1: Full API ---" << std::endl;
        auto engine = create_engine();
        
        // Register some C++ functions
        engine->register_function("multiply", [](double a, double b) -> double {
            return a * b;
        });
        
        engine->register_function("greet", [](const std::string& name) -> std::string {
            return "Hello from C++, " + name + "!";
        });
        
        // Execute Zephyr code
        auto result = engine->execute_string(R"(
            product = multiply(6, 7)
            greeting = greet("World")
            print(greeting)
            print("6 * 7 = " + str(product))
            return product
        )");
        
        if (result) {
            std::cout << "Result: " << result.value()->to_string() << std::endl;
        } else {
            std::cout << "Error: " << result.error() << std::endl;
        }
        
        // Method 2: Using convenience functions
        std::cout << "\n--- Method 2: Quick API ---" << std::endl;
        
        // Register function using convenience API
        zephyr::api::register_function("add", [](double a, double b) -> double {
            return a + b;
        });
        
        // Execute using convenience function
        auto quick_result = zephyr::api::execute(R"(
            sum = add(10, 15)
            print("10 + 15 = " + str(sum))
            return sum
        )");
        
        if (quick_result) {
            std::cout << "Quick result: " << quick_result.value()->to_string() << std::endl;
        }
        
        // Method 3: Ultra-quick execution
        std::cout << "\n--- Method 3: Ultra-Quick ---" << std::endl;
        
        auto ultra_result = zephyr::quick::run(R"(
            message = "Ultra-quick Zephyr execution!"
            print(message)
            return message
        )");
        
        if (ultra_result) {
            std::cout << "Ultra result: " << ultra_result.value()->to_string() << std::endl;
        }
        
        // Demonstrate type conversion
        std::cout << "\n--- Type Conversion Demo ---" << std::endl;
        
        // C++ -> Zephyr
        auto cpp_number = 42;
        auto cpp_string = std::string("Hello");
        auto cpp_vector = std::vector<double>{1.1, 2.2, 3.3};
        
        auto zephyr_number = to_zephyr(cpp_number);
        auto zephyr_string = to_zephyr(cpp_string);
        auto zephyr_list = to_zephyr(cpp_vector);
        
        std::cout << "C++ -> Zephyr conversions:" << std::endl;
        std::cout << "  " << cpp_number << " -> " << zephyr_number->to_string() << std::endl;
        std::cout << "  \"" << cpp_string << "\" -> " << zephyr_string->to_string() << std::endl;
        std::cout << "  vector -> " << zephyr_list->to_string() << std::endl;
        
        // Zephyr -> C++
        auto back_to_cpp_number = from_zephyr<int>(zephyr_number);
        auto back_to_cpp_string = from_zephyr<std::string>(zephyr_string);
        
        if (back_to_cpp_number && back_to_cpp_string) {
            std::cout << "Zephyr -> C++ conversions:" << std::endl;
            std::cout << "  " << zephyr_number->to_string() << " -> " << back_to_cpp_number.value() << std::endl;
            std::cout << "  " << zephyr_string->to_string() << " -> \"" << back_to_cpp_string.value() << "\"" << std::endl;
        }
        
        // Show engine statistics
        std::cout << "\n--- Engine Statistics ---" << std::endl;
        auto stats = engine->get_stats();
        std::cout << "Functions registered: " << stats.functions_registered << std::endl;
        std::cout << "Scripts executed: " << stats.scripts_executed << std::endl;
        std::cout << "Total execution time: " << stats.total_execution_time.count() << "ms" << std::endl;
        
        std::cout << "\n=== Quick Start Complete! ===" << std::endl;
        std::cout << "The Zephyr C++ API is working perfectly!" << std::endl;
        std::cout << "Professional namespace structure: ✓" << std::endl;
        std::cout << "Convenient all-in-one header: ✓" << std::endl;
        std::cout << "Industry-standard organization: ✓" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

/*
 * === About This Example ===
 * 
 * This quick start demonstrates the new professional Zephyr API structure:
 * 
 * 1. Professional Includes:
 *    - #include "zephyr/zephyr.hpp" - All-in-one convenience header
 *    - Clear namespace: All Zephyr headers start with "zephyr/"
 *    - Industry standard: Follows patterns like Qt, Boost, etc.
 * 
 * 2. Multiple Usage Patterns:
 *    - Full API: zephyr::api::create_engine() for full control
 *    - Quick API: zephyr::api::execute() for simple cases
 *    - Ultra-quick: zephyr::quick::run() for one-liners
 * 
 * 3. Type Safety:
 *    - Automatic C++ ↔ Zephyr type conversion
 *    - Template-based function registration
 *    - Comprehensive error handling
 * 
 * 4. Professional Features:
 *    - Smart pointer memory management
 *    - RAII resource handling
 *    - Thread-safe design
 *    - Performance statistics
 * 
 * === Compilation ===
 * 
 * To compile this example:
 * 
 * g++ -std=c++17 -I../include quick_start.cpp -lzephyr -o quick_start
 * 
 * Or with CMake:
 * 
 * find_package(Zephyr REQUIRED)
 * add_executable(quick_start quick_start.cpp)
 * target_link_libraries(quick_start zephyr::zephyr)
 * 
 * === Next Steps ===
 * 
 * - See api_demo.cpp for comprehensive examples
 * - See simple_plugin.cpp for plugin development
 * - Read CPP_API_USAGE_GUIDE.md for detailed documentation
 * - Visit PLUGIN_SYSTEM_STATUS.md for implementation status
 */