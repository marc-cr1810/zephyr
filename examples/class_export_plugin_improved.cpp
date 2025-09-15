#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/api/class_exporter.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include <cmath>
#include <memory>
#include <iostream>

// Forward declare simple_native_class_t from native_module.cpp
namespace zephyr::api {
    class simple_native_class_t : public native_class_t {
    public:
        auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
            return value_result_t::error("Simple native class cannot create instances");
        }
    };
}

using namespace zephyr::api;
using namespace zephyr;

/**
 * Improved Class Export Plugin Example
 * 
 * This plugin demonstrates the new export_class functionality that allows
 * proper class instances with member variable access and method calls.
 * 
 * The plugin exports:
 * - Vector2D: A 2D vector class with x, y members and mathematical operations
 * - Rectangle: A rectangle class with width, height members and calculations
 * - Counter: A simple counter class with value, name members and operations
 */

// C++ Vector2D implementation with public members
class Vector2D {
public:
    double x, y;
    
    Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    double magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2D add(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D multiply(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    double dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
    
    void normalize() {
        double mag = magnitude();
        if (mag > 0.0) {
            x /= mag;
            y /= mag;
        }
    }
    
    std::string to_string() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// C++ Rectangle implementation with public members
class Rectangle {
public:
    double width, height;
    
    Rectangle(double w = 0.0, double h = 0.0) : width(w), height(h) {}
    
    double area() const {
        return width * height;
    }
    
    double perimeter() const {
        return 2 * (width + height);
    }
    
    bool is_square() const {
        return std::abs(width - height) < 1e-9;
    }
    
    void scale(double factor) {
        width *= factor;
        height *= factor;
    }
    
    std::string to_string() const {
        return "Rectangle(" + std::to_string(width) + " x " + std::to_string(height) + ")";
    }
};

// Simple Counter class with public members
class Counter {
public:
    int value;
    std::string name;
    
    Counter(const std::string& n = "counter", int initial = 0) : name(n), value(initial) {}
    
    int increment() {
        return ++value;
    }
    
    int decrement() {
        return --value;
    }
    
    void reset() {
        value = 0;
    }
    
    void set_name(const std::string& new_name) {
        name = new_name;
    }
    
    std::string to_string() const {
        return "Counter('" + name + "': " + std::to_string(value) + ")";
    }
};

// Plugin module implementation using the new class exporter
class improved_class_export_module_t : public native_module_t {
private:
    // Keep exporters alive to prevent memory corruption during binding
    std::unique_ptr<class_exporter_t<Vector2D>> m_vector2d_exporter;
    std::unique_ptr<class_exporter_t<Rectangle>> m_rectangle_exporter;
    std::unique_ptr<class_exporter_t<Counter>> m_counter_exporter;

public:
    auto initialize() -> void_result_t override {
        set_name("class_export_plugin_improved");
        set_version("2.0.0");
        set_description("Improved class export plugin with full member access");
        
        // Export Vector2D class with member access using the fixed class exporter
        std::cout << "DEBUG: Creating Vector2D exporter..." << std::endl;
        m_vector2d_exporter = std::make_unique<class_exporter_t<Vector2D>>(make_class_exporter<Vector2D>("Vector2D")
            .constructor([](double x, double y) {
                std::cout << "DEBUG: Vector2D constructor(double, double) called with " << x << ", " << y << std::endl;
                return std::make_shared<Vector2D>(x, y);
            })
            .constructor([]() {
                std::cout << "DEBUG: Vector2D default constructor called" << std::endl;
                return std::make_shared<Vector2D>();
            })
            .member("x", &Vector2D::x)
            .member("y", &Vector2D::y)
            .method("magnitude", &Vector2D::magnitude)
            .method("normalize", &Vector2D::normalize)
            .method("to_string", &Vector2D::to_string));
        
        std::cout << "DEBUG: Exporting Vector2D class..." << std::endl;
        m_vector2d_exporter->export_to_module(this);
        std::cout << "DEBUG: Vector2D exported successfully" << std::endl;
        
        // Export Rectangle class with member access
        m_rectangle_exporter = std::make_unique<class_exporter_t<Rectangle>>(make_class_exporter<Rectangle>("Rectangle")
            .constructor([](double w, double h) {
                return std::make_shared<Rectangle>(w, h);
            })
            .constructor([]() {
                return std::make_shared<Rectangle>();
            })
            .member("width", &Rectangle::width)
            .member("height", &Rectangle::height)
            .method("area", &Rectangle::area)
            .method("perimeter", &Rectangle::perimeter)
            .method("is_square", &Rectangle::is_square)
            .method("scale", &Rectangle::scale)
            .method("to_string", &Rectangle::to_string));
        
        m_rectangle_exporter->export_to_module(this);
        
        // Export Counter class with member access
        m_counter_exporter = std::make_unique<class_exporter_t<Counter>>(make_class_exporter<Counter>("Counter")
            .constructor([](const std::string& name, int initial) {
                return std::make_shared<Counter>(name, initial);
            })
            .constructor([](const std::string& name) {
                return std::make_shared<Counter>(name, 0);
            })
            .constructor([]() {
                return std::make_shared<Counter>();
            })
            .member("value", &Counter::value)
            .member("name", &Counter::name)
            .method("increment", &Counter::increment)
            .method("decrement", &Counter::decrement)
            .method("reset", &Counter::reset)
            .method("set_name", &Counter::set_name)
            .method("to_string", &Counter::to_string));
        
        m_counter_exporter->export_to_module(this);
        
        // Export utility functions that work with the classes
        export_function("vector_add", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("vector_add requires 2 Vector2D arguments");
            }
            
            auto vec1_wrapper = std::dynamic_pointer_cast<enhanced_cpp_object_wrapper_t<Vector2D>>(args[0]);
            auto vec2_wrapper = std::dynamic_pointer_cast<enhanced_cpp_object_wrapper_t<Vector2D>>(args[1]);
            
            if (!vec1_wrapper || !vec2_wrapper) {
                return value_result_t::error("Both arguments must be Vector2D objects");
            }
            
            auto vec1 = vec1_wrapper->get_cpp_object();
            auto vec2 = vec2_wrapper->get_cpp_object();
            
            auto result = vec1->add(*vec2);
            auto result_obj = std::make_shared<Vector2D>(result);
            
            // Create wrapper for the result
            auto vector2d_exporter_temp = make_class_exporter<Vector2D>("Vector2D");
            auto wrapper = std::make_shared<enhanced_cpp_object_wrapper_t<Vector2D>>(result_obj, vector2d_exporter_temp.get_native_class());
            
            return value_result_t::success(wrapper);
        });
        
        export_function("vector_dot", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("vector_dot requires 2 Vector2D arguments");
            }
            
            auto vec1_wrapper = std::dynamic_pointer_cast<enhanced_cpp_object_wrapper_t<Vector2D>>(args[0]);
            auto vec2_wrapper = std::dynamic_pointer_cast<enhanced_cpp_object_wrapper_t<Vector2D>>(args[1]);
            
            if (!vec1_wrapper || !vec2_wrapper) {
                return value_result_t::error("Both arguments must be Vector2D objects");
            }
            
            auto vec1 = vec1_wrapper->get_cpp_object();
            auto vec2 = vec2_wrapper->get_cpp_object();
            
            double dot_product = vec1->dot(*vec2);
            return value_result_t::success(std::make_shared<float_object_t>(dot_product));
        });
        
        // Export mathematical constants
        export_constant("PI", std::make_shared<float_object_t>(M_PI));
        export_constant("E", std::make_shared<float_object_t>(M_E));
        export_constant("SQRT2", std::make_shared<float_object_t>(M_SQRT2));
        
        // Export plugin metadata
        export_constant("PLUGIN_NAME", std::make_shared<string_object_t>("Improved Class Export"));
        export_constant("VERSION", std::make_shared<string_object_t>("2.0.0"));
        export_constant("SUPPORTS_MEMBER_ACCESS", std::make_shared<int_object_t>(1));
        export_constant("SUPPORTS_METHOD_CALLS", std::make_shared<int_object_t>(1));
        
        return void_result_t::success();
    }
};

// Main plugin class
class improved_class_export_plugin_t : public plugin_interface_t {
public:
    auto get_plugin_info() const -> plugin_info_t override {
        plugin_info_t info;
        info.name = "class_export_plugin_improved";
        info.description = "Improved class export plugin with full member access and method calls";
        info.author = "Zephyr Development Team";
        info.version = {2, 0, 0};
        info.min_zephyr_version = {1, 0, 0};
        info.license = "MIT";
        info.website = "https://github.com/zephyr-lang/zephyr";
        return info;
    }
    
    auto initialize(engine_t* engine) -> plugin_result_t override {
        // Plugin initialization - nothing special needed
        return plugin_result_t::success();
    }
    
    auto finalize() -> void override {
        // Cleanup - nothing to clean up in this example
    }
    
    auto create_module() -> std::shared_ptr<native_module_t> override {
        auto module = std::make_shared<improved_class_export_module_t>();
        auto init_result = module->initialize();
        
        if (!init_result) {
            // Return empty module on initialization failure
            return std::make_shared<native_module_t>();
        }
        
        return module;
    }
};

// Export the plugin using the ZEPHYR_PLUGIN macro
ZEPHYR_PLUGIN(improved_class_export_plugin_t)