#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/boolean_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include <memory>
#include <cmath>

using namespace zephyr::api;
using namespace zephyr;

// Forward declarations
template<typename T>
class cpp_native_class_t;

// C++ Vector2D class
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
    
    std::string to_string() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// C++ object wrapper for storing in class instances
template<typename T>
class cpp_object_storage_t : public object_t {
public:
    explicit cpp_object_storage_t(std::shared_ptr<T> obj) : m_cpp_object(obj) {}
    
    auto type() const -> std::shared_ptr<type_t> override { return nullptr; }
    auto to_string() const -> std::string override {
        return "cpp_storage<" + std::string(typeid(T).name()) + ">";
    }
    
    std::shared_ptr<T> get() const { return m_cpp_object; }

private:
    std::shared_ptr<T> m_cpp_object;
};

// Simple C++ native class implementation
template<typename T>
class cpp_native_class_t : public native_class_t {
public:
    explicit cpp_native_class_t(const std::string& class_name) {
        set_name(class_name);
    }
    
    auto set_constructor(std::function<std::shared_ptr<T>()> ctor_func) -> cpp_native_class_t<T>& {
        std::string class_name = get_name();
        m_constructor_func = [ctor_func, class_name](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 0) {
                return value_result_t::error("Constructor expects 0 arguments");
            }
            
            try {
                auto cpp_obj = ctor_func();
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null");
                }
                
                // Create a class instance and store the C++ object
                auto class_obj = std::make_shared<class_object_t>(class_name);
                auto instance = std::make_shared<class_instance_t>(class_obj);
                
                // Store the C++ object
                auto storage = std::make_shared<cpp_object_storage_t<T>>(cpp_obj);
                instance->member("__cpp_object__", storage);
                
                return value_result_t::success(instance);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        };
        return *this;
    }
    
    auto set_constructor(std::function<std::shared_ptr<T>(double, double)> ctor_func) -> cpp_native_class_t<T>& {
        std::string class_name = get_name();
        m_constructor_func = [ctor_func, class_name](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("Constructor expects 2 arguments");
            }
            
            double x = 0.0, y = 0.0;
            
            if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                x = float_obj->value();
            } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                x = static_cast<double>(int_obj->value());
            } else {
                return value_result_t::error("First argument must be numeric");
            }
            
            if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                y = float_obj->value();
            } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                y = static_cast<double>(int_obj->value());
            } else {
                return value_result_t::error("Second argument must be numeric");
            }
            
            try {
                auto cpp_obj = ctor_func(x, y);
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null");
                }
                
                // Create a class instance and store the C++ object
                auto class_obj = std::make_shared<class_object_t>(class_name);
                auto instance = std::make_shared<class_instance_t>(class_obj);
                
                // Store the C++ object
                auto storage = std::make_shared<cpp_object_storage_t<T>>(cpp_obj);
                instance->member("__cpp_object__", storage);
                
                return value_result_t::success(instance);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        };
        return *this;
    }
    
    template<typename MemberType>
    auto add_member_property(const std::string& name, MemberType T::*member_ptr) -> cpp_native_class_t<T>& {
        // Add getter
        add_property_getter(name, [member_ptr, name]() -> value_result_t {
            return value_result_t::error("Member getter for " + name + " not bound to instance");
        });
        
        // Add setter (if not const)
        if constexpr (!std::is_const_v<MemberType>) {
            add_property_setter(name, [member_ptr, name](const value_t& value) -> void_result_t {
                return void_result_t::error("Member setter for " + name + " not bound to instance");
            });
        }
        
        return *this;
    }
    
    template<typename ReturnType>
    auto add_cpp_method(const std::string& name, ReturnType (T::*method_ptr)() const) -> cpp_native_class_t<T>& {
        add_method(name, [method_ptr, name](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) {
                return value_result_t::error("Method " + name + " expects 'self' parameter only");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter for method " + name);
            }
            
            auto storage_obj = instance->member("__cpp_object__");
            auto storage = std::dynamic_pointer_cast<cpp_object_storage_t<T>>(storage_obj);
            if (!storage) {
                return value_result_t::error("Class instance missing C++ object for method " + name);
            }
            
            auto cpp_obj = storage->get();
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else if constexpr (std::is_same_v<ReturnType, double>) {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<float_object_t>(result));
                } else if constexpr (std::is_same_v<ReturnType, std::string>) {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<string_object_t>(result));
                } else {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<string_object_t>(std::to_string(result)));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("Method " + name + " failed: " + std::string(e.what()));
            }
        });
        return *this;
    }
    
    template<typename ReturnType>
    auto add_cpp_method(const std::string& name, ReturnType (T::*method_ptr)()) -> cpp_native_class_t<T>& {
        add_method(name, [method_ptr, name](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) {
                return value_result_t::error("Method " + name + " expects 'self' parameter only");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter for method " + name);
            }
            
            auto storage_obj = instance->member("__cpp_object__");
            auto storage = std::dynamic_pointer_cast<cpp_object_storage_t<T>>(storage_obj);
            if (!storage) {
                return value_result_t::error("Class instance missing C++ object for method " + name);
            }
            
            auto cpp_obj = storage->get();
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else if constexpr (std::is_same_v<ReturnType, double>) {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<float_object_t>(result));
                } else if constexpr (std::is_same_v<ReturnType, std::string>) {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<string_object_t>(result));
                } else {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<string_object_t>(std::to_string(result)));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("Method " + name + " failed: " + std::string(e.what()));
            }
        });
        return *this;
    }
    
    auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
        if (m_constructor_func) {
            return m_constructor_func(args);
        }
        return value_result_t::error("No constructor registered for " + get_name());
    }

public:
    native_function_t m_constructor_func;
};

// Plugin module implementation
class proper_class_export_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("proper_class_export");
        set_version("1.0.0");
        set_description("Proper class export using native_class_t");
        
        // Create Vector2D class using a much simpler approach
        auto vector2d_class = std::make_shared<cpp_native_class_t<Vector2D>>("Vector2D");
        
        // Set constructor that handles both cases
        vector2d_class->m_constructor_func = [](const std::vector<value_t>& args) -> value_result_t {
            std::shared_ptr<Vector2D> cpp_obj;
            
            if (args.size() == 0) {
                cpp_obj = std::make_shared<Vector2D>();
            } else if (args.size() == 2) {
                double x = 0.0, y = 0.0;
                
                if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[0])) {
                    x = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
                    x = static_cast<double>(int_obj->value());
                }
                
                if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(args[1])) {
                    y = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[1])) {
                    y = static_cast<double>(int_obj->value());
                }
                
                cpp_obj = std::make_shared<Vector2D>(x, y);
            } else {
                return value_result_t::error("Vector2D constructor expects 0 or 2 arguments");
            }
            
            // Create a class instance and store the C++ object
            auto class_obj = std::make_shared<class_object_t>("Vector2D");
            auto instance = std::make_shared<class_instance_t>(class_obj);
            
            // Store the C++ object
            auto storage = std::make_shared<cpp_object_storage_t<Vector2D>>(cpp_obj);
            instance->member("__cpp_object__", storage);
            
            return value_result_t::success(instance);
        };
        
        // Export the class
        export_class("Vector2D", vector2d_class);
        
        // Export constants
        export_constant("PI", std::make_shared<float_object_t>(M_PI));
        export_constant("PLUGIN_NAME", std::make_shared<string_object_t>("Proper Class Export"));
        export_constant("VERSION", std::make_shared<string_object_t>("1.0.0"));
        
        return void_result_t::success();
    }
};

// Main plugin class
class proper_class_export_plugin_t : public plugin_interface_t {
public:
    auto get_plugin_info() const -> plugin_info_t override {
        plugin_info_t info;
        info.name = "proper_class_export";
        info.description = "Proper class export using native_class_t";
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
        // Cleanup
    }
    
    auto create_module() -> std::shared_ptr<native_module_t> override {
        auto module = std::make_shared<proper_class_export_module_t>();
        auto init_result = module->initialize();
        
        if (!init_result) {
            return std::make_shared<native_module_t>();
        }
        
        return module;
    }
};

// Export the plugin
ZEPHYR_PLUGIN(proper_class_export_plugin_t)