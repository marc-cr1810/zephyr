#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include <cmath>
#include <memory>

using namespace zephyr::api;
using namespace zephyr;

/**
 * Class Export Plugin Example
 * 
 * This plugin demonstrates how to export class-like functionality from C++ to Zephyr.
 * Since Zephyr plugins use a function-based approach for class exports, we implement
 * constructor functions that create and return class instances.
 * 
 * The plugin exports:
 * - Vector2D: A 2D vector class with mathematical operations
 * - Rectangle: A rectangle class with area/perimeter calculations
 * - Counter: A simple counter class with increment/decrement
 */

// C++ Vector2D implementation
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
    
    std::string to_string() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// C++ Rectangle implementation  
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
    
    std::string to_string() const {
        return "Rectangle(" + std::to_string(width) + " x " + std::to_string(height) + ")";
    }
};

// Simple Counter class
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
    
    std::string to_string() const {
        return "Counter('" + name + "': " + std::to_string(value) + ")";
    }
};

// Plugin module implementation
class class_export_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("class_export_plugin");
        set_version("1.0.0");
        set_description("Example plugin showcasing class export functionality");
        
        // Export Vector2D constructor and operations
        export_function("Vector2D", [](const std::vector<value_t>& args) -> value_result_t {
            double x = 0.0, y = 0.0;
            
            // Parse arguments
            if (args.size() >= 1) {
                if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                    x = static_cast<double>(int_obj->value());
                } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                    x = float_obj->value();
                }
            }
            
            if (args.size() >= 2) {
                if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                    y = static_cast<double>(int_obj->value());
                } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                    y = float_obj->value();
                }
            }
            
            Vector2D vec(x, y);
            return value_result_t::success(std::make_shared<string_object_t>(vec.to_string()));
        });
        
        export_function("vector_add", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 4) {
                return value_result_t::error("vector_add requires 4 arguments: x1, y1, x2, y2");
            }
            
            double x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
            
            // Parse all coordinates
            auto parse_coord = [](const value_t& arg) -> double {
                if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(arg)) {
                    return static_cast<double>(int_obj->value());
                } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(arg)) {
                    return float_obj->value();
                }
                return 0.0;
            };
            
            x1 = parse_coord(args[0]);
            y1 = parse_coord(args[1]);
            x2 = parse_coord(args[2]);
            y2 = parse_coord(args[3]);
            
            Vector2D v1(x1, y1);
            Vector2D v2(x2, y2);
            Vector2D result = v1.add(v2);
            
            return value_result_t::success(std::make_shared<string_object_t>(result.to_string()));
        });
        
        export_function("vector_magnitude", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("vector_magnitude requires 2 arguments: x, y");
            }
            
            double x = 0.0, y = 0.0;
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                x = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                x = float_obj->value();
            }
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                y = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                y = float_obj->value();
            }
            
            Vector2D vec(x, y);
            return value_result_t::success(std::make_shared<float_object_t>(vec.magnitude()));
        });
        
        // Export Rectangle constructor and methods
        export_function("Rectangle", [](const std::vector<value_t>& args) -> value_result_t {
            double width = 0.0, height = 0.0;
            
            if (args.size() >= 1) {
                if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                    width = static_cast<double>(int_obj->value());
                } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                    width = float_obj->value();
                }
            }
            
            if (args.size() >= 2) {
                if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                    height = static_cast<double>(int_obj->value());
                } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                    height = float_obj->value();
                }
            }
            
            Rectangle rect(width, height);
            return value_result_t::success(std::make_shared<string_object_t>(rect.to_string()));
        });
        
        export_function("rectangle_area", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("rectangle_area requires 2 arguments: width, height");
            }
            
            double width = 0.0, height = 0.0;
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                width = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                width = float_obj->value();
            }
            
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                height = static_cast<double>(int_obj->value());
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                height = float_obj->value();
            }
            
            Rectangle rect(width, height);
            return value_result_t::success(std::make_shared<float_object_t>(rect.area()));
        });
        
        // Export Counter constructor and methods
        export_function("Counter", [](const std::vector<value_t>& args) -> value_result_t {
            std::string name = "counter";
            int initial_value = 0;
            
            if (args.size() >= 1) {
                if (auto str_obj = std::dynamic_pointer_cast<string_object_t>(args[0])) {
                    name = str_obj->value();
                }
            }
            
            if (args.size() >= 2) {
                if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                    initial_value = int_obj->value();
                }
            }
            
            Counter counter(name, initial_value);
            return value_result_t::success(std::make_shared<string_object_t>(counter.to_string()));
        });
        
        // Export mathematical constants
        export_constant("PI", std::make_shared<float_object_t>(M_PI));
        export_constant("E", std::make_shared<float_object_t>(M_E));
        export_constant("SQRT2", std::make_shared<float_object_t>(M_SQRT2));
        
        // Export plugin metadata
        export_constant("PLUGIN_NAME", std::make_shared<string_object_t>("Class Export Example"));
        export_constant("VERSION", std::make_shared<string_object_t>("1.0.0"));
        export_constant("SUPPORTS_CLASSES", std::make_shared<int_object_t>(1));
        
        return void_result_t::success();
    }
};

// Main plugin class
class class_export_plugin_t : public plugin_interface_t {
public:
    auto get_plugin_info() const -> plugin_info_t override {
        plugin_info_t info;
        info.name = "class_export_plugin";
        info.description = "Example plugin demonstrating class export functionality";
        info.author = "Zephyr Development Team";
        info.version = {1, 0, 0};
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
        auto module = std::make_shared<class_export_module_t>();
        auto init_result = module->initialize();
        
        if (!init_result) {
            // Return empty module on initialization failure
            return std::make_shared<native_module_t>();
        }
        
        return module;
    }
};

// Export the plugin using the ZEPHYR_PLUGIN macro
ZEPHYR_PLUGIN(class_export_plugin_t)