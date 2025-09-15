#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/dictionary_object.hpp"
#include <memory>
#include <unordered_map>

using namespace zephyr::api;
using namespace zephyr;

/**
 * Working Class Export System
 * 
 * This implementation provides a working solution for class exports with member access.
 * It uses a hybrid approach combining export_function with object handles stored
 * in a registry, allowing for member variable access and method calls.
 */

// Forward declarations
class ObjectRegistry;
static std::unique_ptr<ObjectRegistry> g_object_registry;

// Object handle type
using object_handle_t = uint64_t;

// Object registry to keep C++ objects alive and provide access
class ObjectRegistry {
public:
    static ObjectRegistry& instance() {
        if (!g_object_registry) {
            g_object_registry = std::make_unique<ObjectRegistry>();
        }
        return *g_object_registry;
    }
    
    template<typename T>
    object_handle_t store_object(std::shared_ptr<T> obj) {
        object_handle_t handle = next_handle_++;
        objects_[handle] = obj;
        return handle;
    }
    
    template<typename T>
    std::shared_ptr<T> get_object(object_handle_t handle) {
        auto it = objects_.find(handle);
        if (it != objects_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }
    
    void remove_object(object_handle_t handle) {
        objects_.erase(handle);
    }

private:
    std::unordered_map<object_handle_t, std::shared_ptr<void>> objects_;
    object_handle_t next_handle_ = 1;
};

// Initialize global registry
std::unique_ptr<ObjectRegistry> g_object_registry = nullptr;

// Helper to create object handle objects that store the handle and class info
class ObjectHandle : public object_t {
public:
    ObjectHandle(object_handle_t handle, const std::string& class_name) 
        : handle_(handle), class_name_(class_name) {}
    
    auto type() const -> std::shared_ptr<type_t> override {
        return nullptr; // Simplified for now
    }
    
    auto to_string() const -> std::string override {
        return class_name_ + "_instance@" + std::to_string(handle_);
    }
    
    object_handle_t get_handle() const { return handle_; }
    std::string get_class_name() const { return class_name_; }
    
    // Override member access to provide C++ member access
    auto member(const std::string& member_name) -> std::shared_ptr<object_t> override {
        if (class_name_ == "Vector2D") {
            auto obj = ObjectRegistry::instance().get_object<Vector2D>(handle_);
            if (obj) {
                if (member_name == "x") {
                    return std::make_shared<float_object_t>(obj->x);
                } else if (member_name == "y") {
                    return std::make_shared<float_object_t>(obj->y);
                }
            }
        } else if (class_name_ == "Rectangle") {
            auto obj = ObjectRegistry::instance().get_object<Rectangle>(handle_);
            if (obj) {
                if (member_name == "width") {
                    return std::make_shared<float_object_t>(obj->width);
                } else if (member_name == "height") {
                    return std::make_shared<float_object_t>(obj->height);
                }
            }
        } else if (class_name_ == "Counter") {
            auto obj = ObjectRegistry::instance().get_object<Counter>(handle_);
            if (obj) {
                if (member_name == "value") {
                    return std::make_shared<int_object_t>(obj->value);
                } else if (member_name == "name") {
                    return std::make_shared<string_object_t>(obj->name);
                }
            }
        }
        return nullptr;
    }
    
    // Override member setting to provide C++ member modification
    auto member(const std::string& member_name, std::shared_ptr<object_t> value) -> void override {
        if (class_name_ == "Vector2D") {
            auto obj = ObjectRegistry::instance().get_object<Vector2D>(handle_);
            if (obj) {
                if (member_name == "x") {
                    if (auto float_val = std::dynamic_pointer_cast<float_object_t>(value)) {
                        obj->x = float_val->value();
                        return;
                    }
                } else if (member_name == "y") {
                    if (auto float_val = std::dynamic_pointer_cast<float_object_t>(value)) {
                        obj->y = float_val->value();
                        return;
                    }
                }
            }
        } else if (class_name_ == "Rectangle") {
            auto obj = ObjectRegistry::instance().get_object<Rectangle>(handle_);
            if (obj) {
                if (member_name == "width") {
                    if (auto float_val = std::dynamic_pointer_cast<float_object_t>(value)) {
                        obj->width = float_val->value();
                        return;
                    }
                } else if (member_name == "height") {
                    if (auto float_val = std::dynamic_pointer_cast<float_object_t>(value)) {
                        obj->height = float_val->value();
                        return;
                    }
                }
            }
        } else if (class_name_ == "Counter") {
            auto obj = ObjectRegistry::instance().get_object<Counter>(handle_);
            if (obj) {
                if (member_name == "value") {
                    if (auto int_val = std::dynamic_pointer_cast<int_object_t>(value)) {
                        obj->value = int_val->value();
                        return;
                    }
                } else if (member_name == "name") {
                    if (auto str_val = std::dynamic_pointer_cast<string_object_t>(value)) {
                        obj->name = str_val->value();
                        return;
                    }
                }
            }
        }
        throw std::runtime_error("Cannot set member " + member_name + " on " + class_name_);
    }
    
    // Override method calls to provide C++ method access
    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override {
        if (class_name_ == "Vector2D") {
            auto obj = ObjectRegistry::instance().get_object<Vector2D>(handle_);
            if (obj) {
                if (method_name == "magnitude" && args.empty()) {
                    return std::make_shared<float_object_t>(obj->magnitude());
                } else if (method_name == "normalize" && args.empty()) {
                    obj->normalize();
                    return std::make_shared<zephyr::none_object_t>();
                } else if (method_name == "to_string" && args.empty()) {
                    return std::make_shared<string_object_t>(obj->to_string());
                }
            }
        } else if (class_name_ == "Rectangle") {
            auto obj = ObjectRegistry::instance().get_object<Rectangle>(handle_);
            if (obj) {
                if (method_name == "area" && args.empty()) {
                    return std::make_shared<float_object_t>(obj->area());
                } else if (method_name == "perimeter" && args.empty()) {
                    return std::make_shared<float_object_t>(obj->perimeter());
                } else if (method_name == "is_square" && args.empty()) {
                    return std::make_shared<zephyr::boolean_object_t>(obj->is_square());
                } else if (method_name == "to_string" && args.empty()) {
                    return std::make_shared<string_object_t>(obj->to_string());
                } else if (method_name == "scale" && args.size() == 1) {
                    if (auto factor = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                        obj->scale(factor->value());
                        return std::make_shared<zephyr::none_object_t>();
                    }
                }
            }
        } else if (class_name_ == "Counter") {
            auto obj = ObjectRegistry::instance().get_object<Counter>(handle_);
            if (obj) {
                if (method_name == "increment" && args.empty()) {
                    return std::make_shared<int_object_t>(obj->increment());
                } else if (method_name == "decrement" && args.empty()) {
                    return std::make_shared<int_object_t>(obj->decrement());
                } else if (method_name == "reset" && args.empty()) {
                    obj->reset();
                    return std::make_shared<zephyr::none_object_t>();
                } else if (method_name == "to_string" && args.empty()) {
                    return std::make_shared<string_object_t>(obj->to_string());
                } else if (method_name == "set_name" && args.size() == 1) {
                    if (auto name = std::dynamic_pointer_cast<string_object_t>(args[0])) {
                        obj->set_name(name->value());
                        return std::make_shared<zephyr::none_object_t>();
                    }
                }
            }
        }
        throw std::runtime_error("Method " + method_name + " not found on " + class_name_);
    }

private:
    object_handle_t handle_;
    std::string class_name_;
};

// C++ class definitions
class Vector2D {
public:
    double x, y;
    
    Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    double magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    void normalize() {
        double mag = magnitude();
        if (mag > 0.0) {
            x /= mag;
            y /= mag;
        }
    }
    
    Vector2D add(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    std::string to_string() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

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

// Plugin module implementation
class working_class_export_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("working_class_export");
        set_version("1.0.0");
        set_description("Working class export plugin with full member access");
        
        // Export Vector2D constructor
        export_function("Vector2D", [](const std::vector<value_t>& args) -> value_result_t {
            double x = 0.0, y = 0.0;
            
            if (args.size() >= 1) {
                if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                    x = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                    x = static_cast<double>(int_obj->value());
                }
            }
            
            if (args.size() >= 2) {
                if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                    y = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                    y = static_cast<double>(int_obj->value());
                }
            }
            
            auto obj = std::make_shared<Vector2D>(x, y);
            auto handle = ObjectRegistry::instance().store_object(obj);
            return value_result_t::success(std::make_shared<ObjectHandle>(handle, "Vector2D"));
        });
        
        // Export Rectangle constructor
        export_function("Rectangle", [](const std::vector<value_t>& args) -> value_result_t {
            double width = 0.0, height = 0.0;
            
            if (args.size() >= 1) {
                if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                    width = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                    width = static_cast<double>(int_obj->value());
                }
            }
            
            if (args.size() >= 2) {
                if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                    height = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                    height = static_cast<double>(int_obj->value());
                }
            }
            
            auto obj = std::make_shared<Rectangle>(width, height);
            auto handle = ObjectRegistry::instance().store_object(obj);
            return value_result_t::success(std::make_shared<ObjectHandle>(handle, "Rectangle"));
        });
        
        // Export Counter constructor
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
            
            auto obj = std::make_shared<Counter>(name, initial_value);
            auto handle = ObjectRegistry::instance().store_object(obj);
            return value_result_t::success(std::make_shared<ObjectHandle>(handle, "Counter"));
        });
        
        // Export utility functions that work with objects
        export_function("vector_add", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("vector_add requires 2 Vector2D arguments");
            }
            
            auto obj1 = std::dynamic_pointer_cast<ObjectHandle>(args[0]);
            auto obj2 = std::dynamic_pointer_cast<ObjectHandle>(args[1]);
            
            if (!obj1 || !obj2 || obj1->get_class_name() != "Vector2D" || obj2->get_class_name() != "Vector2D") {
                return value_result_t::error("Both arguments must be Vector2D objects");
            }
            
            auto vec1 = ObjectRegistry::instance().get_object<Vector2D>(obj1->get_handle());
            auto vec2 = ObjectRegistry::instance().get_object<Vector2D>(obj2->get_handle());
            
            if (!vec1 || !vec2) {
                return value_result_t::error("Invalid Vector2D objects");
            }
            
            auto result = vec1->add(*vec2);
            auto result_obj = std::make_shared<Vector2D>(result);
            auto handle = ObjectRegistry::instance().store_object(result_obj);
            return value_result_t::success(std::make_shared<ObjectHandle>(handle, "Vector2D"));
        });
        
        // Export mathematical constants
        export_constant("PI", std::make_shared<float_object_t>(M_PI));
        export_constant("E", std::make_shared<float_object_t>(M_E));
        export_constant("SQRT2", std::make_shared<float_object_t>(M_SQRT2));
        
        // Export plugin metadata
        export_constant("PLUGIN_NAME", std::make_shared<string_object_t>("Working Class Export"));
        export_constant("VERSION", std::make_shared<string_object_t>("1.0.0"));
        export_constant("SUPPORTS_MEMBER_ACCESS", std::make_shared<int_object_t>(1));
        export_constant("SUPPORTS_METHOD_CALLS", std::make_shared<int_object_t>(1));
        
        return void_result_t::success();
    }
};

// Main plugin class
class working_class_export_plugin_t : public plugin_interface_t {
public:
    auto get_plugin_info() const -> plugin_info_t override {
        plugin_info_t info;
        info.name = "working_class_export";
        info.description = "Working class export plugin with full member access and method calls";
        info.author = "Zephyr Development Team";
        info.version = {1, 0, 0};
        info.min_zephyr_version = {1, 0, 0};
        info.license = "MIT";
        info.website = "https://github.com/zephyr-lang/zephyr";
        return info;
    }
    
    auto initialize(engine_t* engine) -> plugin_result_t override {
        return plugin_result_t::success();
    }
    
    auto finalize() -> void override {
        // Cleanup the global registry
        g_object_registry.reset();
    }
    
    auto create_module() -> std::shared_ptr<native_module_t> override {
        auto module = std::make_shared<working_class_export_module_t>();
        auto init_result = module->initialize();
        
        if (!init_result) {
            return std::make_shared<native_module_t>();
        }
        
        return module;
    }
};

// Export the plugin using the ZEPHYR_PLUGIN macro
ZEPHYR_PLUGIN(working_class_export_plugin_t)