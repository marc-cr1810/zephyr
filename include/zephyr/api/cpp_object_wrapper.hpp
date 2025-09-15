#pragma once

#include "zephyr/objects/object.hpp"
#include "zephyr/api/result.hpp"
#include "zephyr/api/native_module.hpp"
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <typeinfo>


namespace zephyr::api {

// Forward declarations
class native_class_t;

/**
 * Enhanced C++ object wrapper that supports member variable access and method calls.
 * This class bridges C++ objects with Zephyr's object system, allowing plugins to
 * export C++ classes with full member access functionality.
 */
class cpp_object_wrapper_base_t : public object_t {
public:
    explicit cpp_object_wrapper_base_t(std::shared_ptr<native_class_t> class_def);
    virtual ~cpp_object_wrapper_base_t() = default;

    // Object interface implementation
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    
    // Member access - supports both getting and setting member variables
    auto member(const std::string& member_name) -> std::shared_ptr<object_t> override;
    auto member(const std::string& member_name, std::shared_ptr<object_t> value) -> void override;
    auto has_member(const std::string& name) const -> bool;
    
    // Method calls
    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;
    
    // Get the class definition
    auto get_class_definition() const -> std::shared_ptr<native_class_t> { return m_class_definition; }

protected:
    // Pure virtual methods that concrete wrappers must implement
    virtual auto get_member_value(const std::string& member_name) -> value_result_t = 0;
    virtual auto set_member_value(const std::string& member_name, const std::shared_ptr<object_t>& value) -> void_result_t = 0;
    virtual auto call_cpp_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> value_result_t = 0;
    virtual auto get_cpp_object_string() const -> std::string = 0;
    
private:
    std::shared_ptr<native_class_t> m_class_definition;
};

/**
 * Template-based C++ object wrapper for specific C++ types.
 * Provides type-safe access to C++ object members and methods.
 */
template<typename T>
class cpp_object_wrapper_t : public cpp_object_wrapper_base_t {
public:
    explicit cpp_object_wrapper_t(std::shared_ptr<T> obj, std::shared_ptr<native_class_t> class_def)
        : cpp_object_wrapper_base_t(class_def), m_cpp_object(obj) {}
    
    // Get the wrapped C++ object
    auto get_cpp_object() const -> std::shared_ptr<T> { return m_cpp_object; }

protected:
    auto get_member_value(const std::string& member_name) -> value_result_t override {
        auto getter = get_class_definition()->get_property_getter(member_name);
        if (getter) {
            // Create a bound getter that captures this instance
            auto bound_getter = [this, original_getter = getter.value()]() -> value_result_t {
                // Set up context for the getter to access this specific instance
                return original_getter();
            };
            return bound_getter();
        }
        return value_result_t::error("Member '" + member_name + "' not found");
    }
    
    auto set_member_value(const std::string& member_name, const std::shared_ptr<object_t>& value) -> void_result_t override {
        auto setter = get_class_definition()->get_property_setter(member_name);
        if (setter) {
            // Create a bound setter that captures this instance
            auto bound_setter = [this, original_setter = setter.value()](const std::shared_ptr<object_t>& val) -> void_result_t {
                // Set up context for the setter to access this specific instance
                original_setter(val);
                return void_result_t::success();
            };
            return bound_setter(value);
        }
        return void_result_t::error("Member '" + member_name + "' is not settable or not found");
    }
    
    auto call_cpp_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> value_result_t override {
        auto method = get_class_definition()->get_method(method_name);
        if (method) {
            // Create a bound method that captures this instance
            auto bound_method = [this, original_method = method.value()](const std::vector<std::shared_ptr<object_t>>& method_args) -> value_result_t {
                // Set up context for the method to access this specific instance
                return original_method(method_args);
            };
            return bound_method(args);
        }
        return value_result_t::error("Method '" + method_name + "' not found");
    }
    
    auto get_cpp_object_string() const -> std::string override {
        return "cpp_object<" + std::string(typeid(T).name()) + ">@" + 
               std::to_string(reinterpret_cast<uintptr_t>(m_cpp_object.get()));
    }

private:
    std::shared_ptr<T> m_cpp_object;
};

/**
 * Registry for instance-specific member accessors and method implementations.
 * This allows each instance to have its own bound member getters/setters and methods.
 */
class instance_binding_registry_t {
public:
    static auto get_instance() -> instance_binding_registry_t& {
        static instance_binding_registry_t instance;
        return instance;
    }
    
    // Bind member getter for a specific instance
    auto bind_member_getter(void* instance_ptr, const std::string& member_name, property_getter_t getter) -> void {
        m_instance_getters[instance_ptr][member_name] = getter;
    }
    
    // Bind member setter for a specific instance  
    auto bind_member_setter(void* instance_ptr, const std::string& member_name, property_setter_t setter) -> void {
        m_instance_setters[instance_ptr][member_name] = setter;
    }
    
    // Bind method for a specific instance
    auto bind_method(void* instance_ptr, const std::string& method_name, native_function_t method) -> void {
        m_instance_methods[instance_ptr][method_name] = method;
    }
    
    // Get bound getter for instance
    auto get_bound_getter(void* instance_ptr, const std::string& member_name) -> std::optional<property_getter_t> {
        auto instance_it = m_instance_getters.find(instance_ptr);
        if (instance_it != m_instance_getters.end()) {
            auto member_it = instance_it->second.find(member_name);
            if (member_it != instance_it->second.end()) {
                return member_it->second;
            }
        }
        return std::nullopt;
    }
    
    // Get bound setter for instance
    auto get_bound_setter(void* instance_ptr, const std::string& member_name) -> std::optional<property_setter_t> {
        auto instance_it = m_instance_setters.find(instance_ptr);
        if (instance_it != m_instance_setters.end()) {
            auto member_it = instance_it->second.find(member_name);
            if (member_it != instance_it->second.end()) {
                return member_it->second;
            }
        }
        return std::nullopt;
    }
    
    // Get bound method for instance
    auto get_bound_method(void* instance_ptr, const std::string& method_name) -> std::optional<native_function_t> {
        auto instance_it = m_instance_methods.find(instance_ptr);
        if (instance_it != m_instance_methods.end()) {
            auto method_it = instance_it->second.find(method_name);
            if (method_it != instance_it->second.end()) {
                return method_it->second;
            }
        }
        return std::nullopt;
    }
    
    // Clean up bindings for an instance when it's destroyed
    auto cleanup_instance(void* instance_ptr) -> void {
        m_instance_getters.erase(instance_ptr);
        m_instance_setters.erase(instance_ptr);
        m_instance_methods.erase(instance_ptr);
    }

private:
    std::map<void*, std::map<std::string, property_getter_t>> m_instance_getters;
    std::map<void*, std::map<std::string, property_setter_t>> m_instance_setters;
    std::map<void*, std::map<std::string, native_function_t>> m_instance_methods;
};

/**
 * Specialized template wrapper that uses the instance binding registry
 * for more efficient and flexible member/method access.
 */
template<typename T>
class enhanced_cpp_object_wrapper_t : public cpp_object_wrapper_base_t {
public:
    explicit enhanced_cpp_object_wrapper_t(std::shared_ptr<T> obj, std::shared_ptr<native_class_t> class_def)
        : cpp_object_wrapper_base_t(class_def), m_cpp_object(obj) {
        m_instance_ptr = obj.get();
    }
    
    ~enhanced_cpp_object_wrapper_t() {
        // Clean up instance bindings when object is destroyed
        instance_binding_registry_t::get_instance().cleanup_instance(m_instance_ptr);
    }
    
    // Get the wrapped C++ object
    auto get_cpp_object() const -> std::shared_ptr<T> { return m_cpp_object; }
    auto get_instance_ptr() const -> void* { return m_instance_ptr; }

protected:
    auto get_member_value(const std::string& member_name) -> value_result_t override {
        // First try instance-specific bound getter
        auto bound_getter = instance_binding_registry_t::get_instance().get_bound_getter(m_instance_ptr, member_name);
        if (bound_getter) {
            return bound_getter.value()();
        }
        
        // Fall back to class-level getter
        auto getter = get_class_definition()->get_property_getter(member_name);
        if (getter) {
            return getter.value()();
        }
        
        return value_result_t::error("Member '" + member_name + "' not found");
    }
    
    auto set_member_value(const std::string& member_name, const std::shared_ptr<object_t>& value) -> void_result_t override {
        // First try instance-specific bound setter
        auto bound_setter = instance_binding_registry_t::get_instance().get_bound_setter(m_instance_ptr, member_name);
        if (bound_setter) {
            bound_setter.value()(value);
            return void_result_t::success();
        }
        
        // Fall back to class-level setter
        auto setter = get_class_definition()->get_property_setter(member_name);
        if (setter) {
            setter.value()(value);
            return void_result_t::success();
        }
        
        return void_result_t::error("Member '" + member_name + "' is not settable or not found");
    }
    
    auto call_cpp_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> value_result_t override {
        // First try instance-specific bound method
        auto bound_method = instance_binding_registry_t::get_instance().get_bound_method(m_instance_ptr, method_name);
        if (bound_method) {
            return bound_method.value()(args);
        }
        
        // Fall back to class-level method
        auto method = get_class_definition()->get_method(method_name);
        if (method) {
            return method.value()(args);
        }
        
        return value_result_t::error("Method '" + method_name + "' not found");
    }
    
    auto get_cpp_object_string() const -> std::string override {
        return "cpp_object<" + std::string(typeid(T).name()) + ">@" + 
               std::to_string(reinterpret_cast<uintptr_t>(m_cpp_object.get()));
    }

private:
    std::shared_ptr<T> m_cpp_object;
    void* m_instance_ptr;
};

} // namespace zephyr::api