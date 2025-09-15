#pragma once

#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include "zephyr/objects/function_object.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/boolean_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/api/result.hpp"
#include "zephyr/api/native_module.hpp"
#include <memory>
#include <functional>
#include <map>
#include <any>
#include <typeinfo>

namespace zephyr::api {

/**
 * Native C++ Object Holder
 * 
 * This class stores a C++ object instance and provides type-safe access to it.
 * It's stored as a member variable in the Zephyr class instance.
 */
template<typename T>
class cpp_object_holder_t {
public:
    explicit cpp_object_holder_t(std::shared_ptr<T> obj) : m_cpp_object(obj) {}
    
    std::shared_ptr<T> get() const { return m_cpp_object; }
    T* get_ptr() const { return m_cpp_object.get(); }
    
private:
    std::shared_ptr<T> m_cpp_object;
};

/**
 * Native Function Wrapper for Methods
 * 
 * Wraps a C++ member function as a Zephyr native function that can be called
 * from within a class instance context.
 */
template<typename T>
class native_method_wrapper_t {
public:
    template<typename ReturnType>
    static auto wrap_method(ReturnType (T::*method_ptr)()) -> native_function_t {
        return [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            // First argument should be 'self' (the class instance)
            if (args.empty()) {
                return value_result_t::error("Method call missing 'self' parameter");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter - not a class instance");
            }
            
            // Get the C++ object from the instance's __cpp_object__ member
            auto cpp_obj_member = instance->member("__cpp_object__");
            if (!cpp_obj_member) {
                return value_result_t::error("Class instance missing C++ object");
            }
            
            // Extract the C++ object holder from the stored object
            auto holder_ptr = std::any_cast<std::shared_ptr<cpp_object_holder_t<T>>*>(
                reinterpret_cast<std::any*>(cpp_obj_member.get()));
            
            if (!holder_ptr || !*holder_ptr) {
                return value_result_t::error("Invalid C++ object holder");
            }
            
            T* cpp_obj = (*holder_ptr)->get_ptr();
            if (!cpp_obj) {
                return value_result_t::error("C++ object is null");
            }
            
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj->*method_ptr)();
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else {
                    auto result = (cpp_obj->*method_ptr)();
                    return value_result_t::success(convert_to_zephyr(result));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("C++ method failed: " + std::string(e.what()));
            }
        };
    }
    
    template<typename ReturnType>
    static auto wrap_const_method(ReturnType (T::*method_ptr)() const) -> native_function_t {
        return [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            // Same implementation as non-const version
            if (args.empty()) {
                return value_result_t::error("Method call missing 'self' parameter");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter - not a class instance");
            }
            
            auto cpp_obj_member = instance->member("__cpp_object__");
            if (!cpp_obj_member) {
                return value_result_t::error("Class instance missing C++ object");
            }
            
            auto holder_ptr = std::any_cast<std::shared_ptr<cpp_object_holder_t<T>>*>(
                reinterpret_cast<std::any*>(cpp_obj_member.get()));
            
            if (!holder_ptr || !*holder_ptr) {
                return value_result_t::error("Invalid C++ object holder");
            }
            
            const T* cpp_obj = (*holder_ptr)->get_ptr();
            if (!cpp_obj) {
                return value_result_t::error("C++ object is null");
            }
            
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj->*method_ptr)();
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else {
                    auto result = (cpp_obj->*method_ptr)();
                    return value_result_t::success(convert_to_zephyr(result));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("C++ method failed: " + std::string(e.what()));
            }
        };
    }
    
    template<typename ReturnType, typename ArgType>
    static auto wrap_method_with_arg(ReturnType (T::*method_ptr)(ArgType)) -> native_function_t {
        return [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) { // self + 1 argument
                return value_result_t::error("Method expects 1 argument plus self");
            }
            
            auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
            if (!instance) {
                return value_result_t::error("Invalid 'self' parameter - not a class instance");
            }
            
            auto cpp_obj_member = instance->member("__cpp_object__");
            if (!cpp_obj_member) {
                return value_result_t::error("Class instance missing C++ object");
            }
            
            auto holder_ptr = std::any_cast<std::shared_ptr<cpp_object_holder_t<T>>*>(
                reinterpret_cast<std::any*>(cpp_obj_member.get()));
            
            if (!holder_ptr || !*holder_ptr) {
                return value_result_t::error("Invalid C++ object holder");
            }
            
            T* cpp_obj = (*holder_ptr)->get_ptr();
            if (!cpp_obj) {
                return value_result_t::error("C++ object is null");
            }
            
            // Convert the argument
            auto cpp_arg = convert_from_zephyr<std::remove_cv_t<std::remove_reference_t<ArgType>>>(args[1]);
            if (!cpp_arg) {
                return value_result_t::error("Failed to convert method argument");
            }
            
            try {
                if constexpr (std::is_void_v<ReturnType>) {
                    (cpp_obj->*method_ptr)(cpp_arg.value());
                    return value_result_t::success(std::make_shared<none_object_t>());
                } else {
                    auto result = (cpp_obj->*method_ptr)(cpp_arg.value());
                    return value_result_t::success(convert_to_zephyr(result));
                }
            } catch (const std::exception& e) {
                return value_result_t::error("C++ method failed: " + std::string(e.what()));
            }
        };
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
};

/**
 * Native Class Builder
 * 
 * Provides a fluent interface for building native Zephyr classes that wrap C++ objects.
 */
template<typename T>
class native_class_builder_t {
public:
    explicit native_class_builder_t(const std::string& class_name) 
        : m_class_name(class_name) {
        m_class_obj = std::make_shared<class_object_t>(class_name);
        
        // Add the special __cpp_object__ member to hold the C++ object
        member_variable_info_t cpp_obj_info("__cpp_object__", "cpp_object", nullptr, false, true);
        m_class_obj->add_member_variable(cpp_obj_info);
    }
    
    template<typename... Args>
    auto constructor(std::function<std::shared_ptr<T>(Args...)> ctor_func) -> native_class_builder_t<T>& {
        // Create an init method that serves as the constructor
        auto init_func = std::make_shared<function_object_t>("init", 
            [ctor_func](const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
                // First argument is 'self'
                if (args.size() != sizeof...(Args) + 1) {
                    throw std::runtime_error("Constructor expects " + std::to_string(sizeof...(Args)) + " arguments");
                }
                
                auto instance = std::dynamic_pointer_cast<class_instance_t>(args[0]);
                if (!instance) {
                    throw std::runtime_error("Invalid self parameter");
                }
                
                try {
                    // Convert arguments and call C++ constructor
                    auto cpp_obj = call_constructor_with_converted_args(ctor_func, args, 
                        std::make_index_sequence<sizeof...(Args)>{});
                    
                    // Store the C++ object in the instance
                    auto holder = std::make_shared<cpp_object_holder_t<T>>(cpp_obj);
                    
                    // Create a custom object to store the holder
                    auto holder_container = std::make_shared<object_t>();
                    // Store the holder pointer in the container somehow...
                    // This needs a custom object type to hold std::any
                    
                    instance->member("__cpp_object__", holder_container);
                    
                    return std::make_shared<none_object_t>();
                } catch (const std::exception& e) {
                    throw std::runtime_error("Constructor failed: " + std::string(e.what()));
                }
            });
        
        auto init_method = std::dynamic_pointer_cast<function_definition_t>(init_func);
        m_class_obj->add_method("init", init_method);
        
        return *this;
    }
    
    auto default_constructor() -> native_class_builder_t<T>& {
        return constructor<>([]() { return std::make_shared<T>(); });
    }
    
    template<typename MemberType>
    auto member(const std::string& name, MemberType T::*member_ptr) -> native_class_builder_t<T>& {
        // Add member variable info
        member_variable_info_t member_info(name, typeid(MemberType).name(), nullptr, false, false);
        m_class_obj->add_member_variable(member_info);
        
        // Store getter/setter info for later use when instances are created
        m_member_accessors[name] = [member_ptr](class_instance_t* instance) {
            // This would need to be called when the instance is created to set up
            // custom getter/setter behavior
        };
        
        return *this;
    }
    
    template<typename ReturnType>
    auto method(const std::string& name, ReturnType (T::*method_ptr)()) -> native_class_builder_t<T>& {
        auto native_func = native_method_wrapper_t<T>::wrap_method(method_ptr);
        auto func_obj = std::make_shared<function_object_t>(name, 
            [native_func](const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
                auto result = native_func(args);
                if (result) {
                    return result.value();
                } else {
                    throw std::runtime_error(result.error());
                }
            });
        
        auto method_def = std::dynamic_pointer_cast<function_definition_t>(func_obj);
        m_class_obj->add_method(name, method_def);
        
        return *this;
    }
    
    template<typename ReturnType>
    auto method(const std::string& name, ReturnType (T::*method_ptr)() const) -> native_class_builder_t<T>& {
        auto native_func = native_method_wrapper_t<T>::wrap_const_method(method_ptr);
        auto func_obj = std::make_shared<function_object_t>(name,
            [native_func](const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
                auto result = native_func(args);
                if (result) {
                    return result.value();
                } else {
                    throw std::runtime_error(result.error());
                }
            });
        
        auto method_def = std::dynamic_pointer_cast<function_definition_t>(func_obj);
        m_class_obj->add_method(name, method_def);
        
        return *this;
    }
    
    template<typename ReturnType, typename ArgType>
    auto method(const std::string& name, ReturnType (T::*method_ptr)(ArgType)) -> native_class_builder_t<T>& {
        auto native_func = native_method_wrapper_t<T>::wrap_method_with_arg(method_ptr);
        auto func_obj = std::make_shared<function_object_t>(name,
            [native_func](const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
                auto result = native_func(args);
                if (result) {
                    return result.value();
                } else {
                    throw std::runtime_error(result.error());
                }
            });
        
        auto method_def = std::dynamic_pointer_cast<function_definition_t>(func_obj);
        m_class_obj->add_method(name, method_def);
        
        return *this;
    }
    
    auto build() -> std::shared_ptr<class_object_t> {
        return m_class_obj;
    }

private:
    template<typename CtorFunc, std::size_t... I>
    auto call_constructor_with_converted_args(CtorFunc ctor_func, 
                                            const std::vector<std::shared_ptr<object_t>>& args,
                                            std::index_sequence<I...>) -> std::shared_ptr<T> {
        // This would need proper argument conversion based on the constructor signature
        // For now, simplified implementation
        return ctor_func();
    }

private:
    std::string m_class_name;
    std::shared_ptr<class_object_t> m_class_obj;
    std::map<std::string, std::function<void(class_instance_t*)>> m_member_accessors;
};

/**
 * Helper function to create a native class builder
 */
template<typename T>
auto make_native_class(const std::string& class_name) -> native_class_builder_t<T> {
    return native_class_builder_t<T>(class_name);
}

/**
 * Native Module Extension
 * 
 * Extends native_module_t to support exporting native classes that integrate
 * with Zephyr's class system.
 */
class integrated_native_module_t : public native_module_t {
public:
    template<typename T>
    auto export_native_class(const std::string& name, native_class_builder_t<T>& builder) -> void {
        auto class_obj = builder.build();
        
        // Export the class object directly - when called, it will create instances
        // using the normal Zephyr class instantiation mechanism
        export_constant(name, class_obj);
    }
};

} // namespace zephyr::api