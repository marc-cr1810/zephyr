#pragma once

#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/boolean_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include <memory>
#include <functional>
#include <map>
#include <typeinfo>
#include <type_traits>

namespace zephyr::api {

/**
 * C++ Object Wrapper for Class Instances
 * 
 * This object stores a C++ object instance within a Zephyr class instance.
 * It provides the bridge between C++ objects and Zephyr's object system.
 */
template<typename T>
class cpp_instance_wrapper_t : public object_t {
public:
    explicit cpp_instance_wrapper_t(std::shared_ptr<T> cpp_obj) 
        : m_cpp_object(cpp_obj) {}
    
    auto type() const -> std::shared_ptr<type_t> override {
        return nullptr; // Could be extended to return proper type
    }
    
    auto to_string() const -> std::string override {
        return "cpp_instance<" + std::string(typeid(T).name()) + ">";
    }
    
    std::shared_ptr<T> get_cpp_object() const { return m_cpp_object; }

private:
    std::shared_ptr<T> m_cpp_object;
};

/**
 * C++ Native Class Implementation
 * 
 * Implements native_class_t for C++ classes, providing proper integration
 * with Zephyr's class system while allowing direct member access and method calls.
 */
template<typename T>
class cpp_native_class_t : public native_class_t {
public:
    explicit cpp_native_class_t(const std::string& class_name) {
        set_name(class_name);
    }
    
    /**
     * Register a constructor function
     */
    template<typename... Args>
    auto set_constructor(std::function<std::shared_ptr<T>(Args...)> ctor_func) -> cpp_native_class_t<T>& {
        m_constructor = [ctor_func](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != sizeof...(Args)) {
                return value_result_t::error("Constructor expects " + std::to_string(sizeof...(Args)) + " arguments");
            }
            
            try {
                auto cpp_obj = call_constructor_with_args(ctor_func, args, std::index_sequence_for<Args...>{});
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null object");
                }
                
                // Create a Zephyr class instance
                auto class_obj = std::make_shared<class_object_t>(get_name());
                auto instance = std::make_shared<class_instance_t>(class_obj);
                
                // Store the C++ object in a special member
                auto wrapper = std::make_shared<cpp_instance_wrapper_t<T>>(cpp_obj);
                instance->member("__cpp_object__", wrapper);
                
                return value_result_t::success(instance);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        };
        return *this;
    }
    
    /**
     * Register a default constructor
     */
    auto set_default_constructor() -> cpp_native_class_t<T>& {
        return set_constructor<>([]() { return std::make_shared<T>(); });
    }
    
    /**
     * Register a member variable
     */
    template<typename MemberType>
    auto add_member(const std::string& name, MemberType T::*member_ptr) -> cpp_native_class_t<T>& {
        // Add getter
        add_property_getter(name, [member_ptr]() -> value_result_t {
            // This will be overridden by the instance-specific getter
            return value_result_t::error("Member getter not bound to instance");
        });
        
        // Add setter (if not const)
        if constexpr (!std::is_const_v<MemberType>) {
            add_property_setter(name, [member_ptr](const value_t& value) -> void_result_t {
                // This will be overridden by the instance-specific setter
                return void_result_t::error("Member setter not bound to instance");
            });
        }
        
        // Store member info for instance creation
        m_member_bindings[name] = [member_ptr](std::shared_ptr<class_instance_t> instance, std::shared_ptr<T> cpp_obj) {
            // Create instance-specific getter
            auto getter_func = [cpp_obj, member_ptr]() -> value_result_t {
                auto value = cpp_obj.get()->*member_ptr;
                return value_result_t::success(convert_to_zephyr(value));
            };
            
            // Override the member access for this specific instance
            auto old_member_func = instance->member;
            instance->member = [old_member_func, getter_func, name](const std::string& member_name) -> std::shared_ptr<object_t> {
                if (member_name == name) {
                    auto result = getter_func();
                    return result ? result.value() : nullptr;
                }
                return old_member_func(member_name);
            };
            
            // Create instance-specific setter (if not const)
            if constexpr (!std::is_const_v<MemberType>) {
                auto setter_func = [cpp_obj, member_ptr](const value_t& value) -> void_result_t {
                    auto cpp_value = convert_from_zephyr<MemberType>(value);
                    if (cpp_value) {
                        cpp_obj.get()->*member_ptr = cpp_value.value();
                        return void_result_t::success();
                    }
                    return void_result_t::error("Failed to convert value");
                };
                
                // Override the member setter
                auto old_member_setter = instance->member;
                instance->member = [old_member_setter, setter_func, name](const std::string& member_name, std::shared_ptr<object_t> value) -> void {
                    if (member_name == name) {
                        auto result = setter_func(value);
                        if (!result) {
                            throw std::runtime_error(result.error());
                        }
                        return;
                    }
                    old_member_setter(member_name, value);
                };
            }
        };
        
        return *this;
    }
    
    /**
     * Register a method with no arguments
     */
    template<typename ReturnType>
    auto add_method(const std::string& name, ReturnType (T::*method_ptr)()) -> cpp_native_class_t<T>& {
        native_class_t::add_method(name, [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) { // First arg should be 'self'
                return value_result_t::error("Method expects 'self' parameter");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter");
            }
            
            auto wrapper_obj = instance->member("__cpp_object__");
            auto wrapper = std::dynamic_pointer_cast<cpp_instance_wrapper_t<T>>(wrapper_obj);
            if (!wrapper) {
                return value_result_t::error("Class instance missing C++ object");
            }
            
            auto cpp_obj = wrapper->get_cpp_object();
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(convert_to_zephyr(result));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("Method failed: " + std::string(e.what()));
            }
        });
        return *this;
    }
    
    /**
     * Register a const method with no arguments
     */
    template<typename ReturnType>
    auto add_method(const std::string& name, ReturnType (T::*method_ptr)() const) -> cpp_native_class_t<T>& {
        native_class_t::add_method(name, [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) {
                return value_result_t::error("Method expects 'self' parameter");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter");
            }
            
            auto wrapper_obj = instance->member("__cpp_object__");
            auto wrapper = std::dynamic_pointer_cast<cpp_instance_wrapper_t<T>>(wrapper_obj);
            if (!wrapper) {
                return value_result_t::error("Class instance missing C++ object");
            }
            
            auto cpp_obj = wrapper->get_cpp_object();
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else {
                    auto result = (cpp_obj.get()->*method_ptr)();
                    return value_result_t::success(convert_to_zephyr(result));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("Method failed: " + std::string(e.what()));
            }
        });
        return *this;
    }
    
    /**
     * Register a method with one argument
     */
    template<typename ReturnType, typename ArgType>
    auto add_method(const std::string& name, ReturnType (T::*method_ptr)(ArgType)) -> cpp_native_class_t<T>& {
        native_class_t::add_method(name, [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) { // self + 1 argument
                return value_result_t::error("Method expects 1 argument plus 'self'");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter");
            }
            
            auto wrapper_obj = instance->member("__cpp_object__");
            auto wrapper = std::dynamic_pointer_cast<cpp_instance_wrapper_t<T>>(wrapper_obj);
            if (!wrapper) {
                return value_result_t::error("Class instance missing C++ object");
            }
            
            auto cpp_arg = convert_from_zephyr<std::remove_cv_t<std::remove_reference_t<ArgType>>>(args[1]);
            if (!cpp_arg) {
                return value_result_t::error("Failed to convert method argument");
            }
            
            auto cpp_obj = wrapper->get_cpp_object();
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj.get()->*method_ptr)(cpp_arg.value());
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else {
                    auto result = (cpp_obj.get()->*method_ptr)(cpp_arg.value());
                    return value_result_t::success(convert_to_zephyr(result));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("Method failed: " + std::string(e.what()));
            }
        });
        return *this;
    }
    
    /**
     * Implementation of native_class_t::create_instance
     */
    auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
        if (m_constructor) {
            auto result = m_constructor(args);
            
            if (result) {
                // Bind member accessors to the created instance
                auto instance = std::dynamic_pointer_cast<class_instance_t>(result.value());
                if (instance) {
                    auto wrapper_obj = instance->member("__cpp_object__");
                    auto wrapper = std::dynamic_pointer_cast<cpp_instance_wrapper_t<T>>(wrapper_obj);
                    if (wrapper) {
                        auto cpp_obj = wrapper->get_cpp_object();
                        
                        // Apply all member bindings
                        for (const auto& [name, binding] : m_member_bindings) {
                            binding(instance, cpp_obj);
                        }
                    }
                }
            }
            
            return result;
        }
        return value_result_t::error("No constructor registered");
    }

private:
    template<typename ValueType>
    static auto convert_to_zephyr(const ValueType& value) -> std::shared_ptr<object_t> {
        if constexpr (std::is_same_v<ValueType, int>) {
            return std::make_shared<int_object_t>(value);
        } else if constexpr (std::is_same_v<ValueType, double> || std::is_same_v<ValueType, float>) {
            return std::make_shared<float_object_t>(static_cast<double>(value));
        } else if constexpr (std::is_same_v<ValueType, std::string>) {
            return std::make_shared<string_object_t>(value);
        } else if constexpr (std::is_same_v<ValueType, bool>) {
            return std::make_shared<boolean_object_t>(value);
        } else {
            return std::make_shared<string_object_t>(std::to_string(value));
        }
    }
    
    template<typename ValueType>
    static auto convert_from_zephyr(const std::shared_ptr<object_t>& obj) -> std::optional<ValueType> {
        if constexpr (std::is_same_v<ValueType, int>) {
            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
                return int_obj->value();
            } else if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(obj)) {
                return static_cast<int>(float_obj->value());
            }
        } else if constexpr (std::is_same_v<ValueType, double>) {
            if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(obj)) {
                return float_obj->value();
            } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
                return static_cast<double>(int_obj->value());
            }
        } else if constexpr (std::is_same_v<ValueType, float>) {
            if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(obj)) {
                return static_cast<float>(float_obj->value());
            } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
                return static_cast<float>(int_obj->value());
            }
        } else if constexpr (std::is_same_v<ValueType, std::string>) {
            if (auto str_obj = std::dynamic_pointer_cast<string_object_t>(obj)) {
                return str_obj->value();
            }
        } else if constexpr (std::is_same_v<ValueType, bool>) {
            if (auto bool_obj = std::dynamic_pointer_cast<boolean_object_t>(obj)) {
                return bool_obj->m_value;
            }
        }
        return std::nullopt;
    }
    
    template<typename CtorFunc, typename... Args, std::size_t... I>
    static auto call_constructor_with_args(CtorFunc ctor_func, const std::vector<value_t>& args, std::index_sequence<I...>) -> std::shared_ptr<T> {
        return ctor_func(convert_from_zephyr<Args>(args[I]).value()...);
    }

private:
    std::map<std::string, std::function<void(std::shared_ptr<class_instance_t>, std::shared_ptr<T>)>> m_member_bindings;
};

/**
 * Helper function to create a C++ native class
 */
template<typename T>
auto make_cpp_native_class(const std::string& class_name) -> std::shared_ptr<cpp_native_class_t<T>> {
    return std::make_shared<cpp_native_class_t<T>>(class_name);
}

} // namespace zephyr::api