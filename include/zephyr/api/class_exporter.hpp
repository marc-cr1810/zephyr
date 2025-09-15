#pragma once

#include "zephyr/api/native_module.hpp"
#include "zephyr/api/cpp_object_wrapper.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/boolean_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include <functional>
#include <type_traits>
#include <memory>
#include <iostream>

namespace zephyr::api {

/**
 * Helper class for registering C++ classes with Zephyr plugins.
 * Provides a fluent interface for exporting classes with member variables and methods.
 */
template<typename T>
class class_exporter_t {
public:
    explicit class_exporter_t(const std::string& class_name)
        : m_class_name(class_name) {
        m_native_class = std::make_shared<concrete_native_class_t>();
        m_native_class->set_name(class_name);
        m_native_class->set_class_exporter(this);
    }

    /**
     * Register a constructor for the class.
     * The constructor function should create and return a shared_ptr<T>.
     */
    auto constructor(std::function<std::shared_ptr<T>()> ctor_func) -> class_exporter_t<T>& {
        m_native_class->set_constructor([ctor_func, this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 0) {
                return value_result_t::error("Constructor expects 0 arguments, got " + std::to_string(args.size()));
            }
            
            try {
                auto cpp_obj = ctor_func();
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null object");
                }
                
                auto wrapper = std::make_shared<enhanced_cpp_object_wrapper_t<T>>(cpp_obj, m_native_class);
                return value_result_t::success(wrapper);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        });
        return *this;
    }

    auto constructor(std::function<std::shared_ptr<T>(double, double)> ctor_func) -> class_exporter_t<T>& {
        m_native_class->set_constructor([ctor_func, this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("Constructor expects 2 arguments, got " + std::to_string(args.size()));
            }
            
            try {
                double arg1, arg2;
                
                if (auto float_obj = std::dynamic_pointer_cast<zephyr::float_object_t>(args[0])) {
                    arg1 = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(args[0])) {
                    arg1 = static_cast<double>(int_obj->value());
                } else {
                    return value_result_t::error("First argument must be numeric");
                }
                
                if (auto float_obj = std::dynamic_pointer_cast<zephyr::float_object_t>(args[1])) {
                    arg2 = float_obj->value();
                } else if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(args[1])) {
                    arg2 = static_cast<double>(int_obj->value());
                } else {
                    return value_result_t::error("Second argument must be numeric");
                }
                
                auto cpp_obj = ctor_func(arg1, arg2);
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null object");
                }
                
                auto wrapper = std::make_shared<enhanced_cpp_object_wrapper_t<T>>(cpp_obj, m_native_class);
                return value_result_t::success(wrapper);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        });
        return *this;
    }

    auto constructor(std::function<std::shared_ptr<T>(const std::string&, int)> ctor_func) -> class_exporter_t<T>& {
        m_native_class->set_constructor([ctor_func, this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) {
                return value_result_t::error("Constructor expects 2 arguments, got " + std::to_string(args.size()));
            }
            
            try {
                std::string arg1;
                int arg2;
                
                if (auto str_obj = std::dynamic_pointer_cast<zephyr::string_object_t>(args[0])) {
                    arg1 = str_obj->value();
                } else {
                    return value_result_t::error("First argument must be a string");
                }
                
                if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(args[1])) {
                    arg2 = int_obj->value();
                } else {
                    return value_result_t::error("Second argument must be an integer");
                }
                
                auto cpp_obj = ctor_func(arg1, arg2);
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null object");
                }
                
                auto wrapper = std::make_shared<enhanced_cpp_object_wrapper_t<T>>(cpp_obj, m_native_class);
                return value_result_t::success(wrapper);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        });
        return *this;
    }

    auto constructor(std::function<std::shared_ptr<T>(const std::string&)> ctor_func) -> class_exporter_t<T>& {
        m_native_class->set_constructor([ctor_func, this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) {
                return value_result_t::error("Constructor expects 1 argument, got " + std::to_string(args.size()));
            }
            
            try {
                std::string arg1;
                
                if (auto str_obj = std::dynamic_pointer_cast<zephyr::string_object_t>(args[0])) {
                    arg1 = str_obj->value();
                } else {
                    return value_result_t::error("Argument must be a string");
                }
                
                auto cpp_obj = ctor_func(arg1);
                if (!cpp_obj) {
                    return value_result_t::error("Constructor returned null object");
                }
                
                auto wrapper = std::make_shared<enhanced_cpp_object_wrapper_t<T>>(cpp_obj, m_native_class);
                return value_result_t::success(wrapper);
            } catch (const std::exception& e) {
                return value_result_t::error("Constructor failed: " + std::string(e.what()));
            }
        });
        return *this;
    }



    /**
     * Register a member variable getter.
     */
    template<typename MemberType>
    auto member(const std::string& name, MemberType T::*member_ptr) -> class_exporter_t<T>& {
        // Register getter
        m_native_class->add_property_getter(name, [member_ptr]() -> value_result_t {
            // This will be bound to specific instances later
            return value_result_t::error("Member getter not bound to instance");
        });
        
        // Register setter (if the member type is not const)
        if constexpr (!std::is_const_v<MemberType>) {
            m_native_class->add_property_setter(name, [member_ptr](const value_t& value) -> void_result_t {
                // This will be bound to specific instances later
                return void_result_t::error("Member setter not bound to instance");
            });
        }
        
        // Store member binding information for later use - capture by value to avoid lifetime issues
        std::string member_name_copy = name;
        m_member_bindings[member_name_copy] = [member_ptr, member_name_copy](void* instance_ptr) {
            T* obj = static_cast<T*>(instance_ptr);
            
            // Bind getter - capture by value
            auto getter = [obj, member_ptr]() -> value_result_t {
                auto result = obj->*member_ptr;
                if constexpr (std::is_same_v<MemberType, int>) {
                    return value_result_t::success(std::make_shared<zephyr::int_object_t>(result));
                } else if constexpr (std::is_same_v<MemberType, double> || std::is_same_v<MemberType, float>) {
                    return value_result_t::success(std::make_shared<zephyr::float_object_t>(static_cast<double>(result)));
                } else if constexpr (std::is_same_v<MemberType, std::string>) {
                    return value_result_t::success(std::make_shared<zephyr::string_object_t>(result));
                } else if constexpr (std::is_same_v<MemberType, bool>) {
                    return value_result_t::success(std::make_shared<zephyr::boolean_object_t>(result));
                } else {
                    return value_result_t::success(std::make_shared<zephyr::string_object_t>(std::to_string(result)));
                }
            };
            instance_binding_registry_t::get_instance().bind_member_getter(instance_ptr, member_name_copy, getter);
            
            // Bind setter (if not const)
            if constexpr (!std::is_const_v<MemberType>) {
                auto setter = [obj, member_ptr, member_name_copy](const value_t& value) -> void_result_t {
                    if constexpr (std::is_same_v<MemberType, int>) {
                        if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(value)) {
                            obj->*member_ptr = int_obj->value();
                            return void_result_t::success();
                        }
                    } else if constexpr (std::is_same_v<MemberType, double>) {
                        if (auto float_obj = std::dynamic_pointer_cast<zephyr::float_object_t>(value)) {
                            obj->*member_ptr = float_obj->value();
                        } else if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(value)) {
                            obj->*member_ptr = static_cast<double>(int_obj->value());
                        }
                        return void_result_t::success();
                    } else if constexpr (std::is_same_v<MemberType, float>) {
                        if (auto float_obj = std::dynamic_pointer_cast<zephyr::float_object_t>(value)) {
                            obj->*member_ptr = static_cast<float>(float_obj->value());
                        } else if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(value)) {
                            obj->*member_ptr = static_cast<float>(int_obj->value());
                        }
                        return void_result_t::success();
                    } else if constexpr (std::is_same_v<MemberType, std::string>) {
                        if (auto str_obj = std::dynamic_pointer_cast<zephyr::string_object_t>(value)) {
                            obj->*member_ptr = str_obj->value();
                            return void_result_t::success();
                        }
                    } else if constexpr (std::is_same_v<MemberType, bool>) {
                        if (auto bool_obj = std::dynamic_pointer_cast<zephyr::boolean_object_t>(value)) {
                            obj->*member_ptr = bool_obj->m_value;
                            return void_result_t::success();
                        }
                    }
                    return void_result_t::error("Failed to convert value for member " + member_name_copy);
                };
                instance_binding_registry_t::get_instance().bind_member_setter(instance_ptr, member_name_copy, setter);
            }
        };
        
        return *this;
    }

    /**
     * Register a method with no arguments.
     */
    template<typename ReturnType>
    auto method(const std::string& name, ReturnType (T::*method_ptr)()) -> class_exporter_t<T>& {
        m_native_class->add_method(name, [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            return value_result_t::error("Method not bound to instance");
        });
        
        std::string method_name_copy = name;
        m_method_bindings[method_name_copy] = [method_ptr, method_name_copy](void* instance_ptr) {
            T* obj = static_cast<T*>(instance_ptr);
            auto bound_method = [obj, method_ptr, method_name_copy](const std::vector<value_t>& args) -> value_result_t {
                if (args.size() != 0) {
                    return value_result_t::error("Method " + method_name_copy + " expects 0 arguments, got " + std::to_string(args.size()));
                }
                
                try {
                    if constexpr (std::is_void_v<ReturnType>) {
                        (obj->*method_ptr)();
                        return value_result_t::success(std::make_shared<zephyr::none_object_t>());
                    } else {
                        auto result = (obj->*method_ptr)();
                        if constexpr (std::is_same_v<ReturnType, int>) {
                            return value_result_t::success(std::make_shared<zephyr::int_object_t>(result));
                        } else if constexpr (std::is_same_v<ReturnType, double> || std::is_same_v<ReturnType, float>) {
                            return value_result_t::success(std::make_shared<zephyr::float_object_t>(static_cast<double>(result)));
                        } else if constexpr (std::is_same_v<ReturnType, std::string>) {
                            return value_result_t::success(std::make_shared<zephyr::string_object_t>(result));
                        } else if constexpr (std::is_same_v<ReturnType, bool>) {
                            return value_result_t::success(std::make_shared<zephyr::boolean_object_t>(result));
                        } else {
                            return value_result_t::success(std::make_shared<zephyr::string_object_t>(std::to_string(result)));
                        }
                    }
                } catch (const std::exception& e) {
                    return value_result_t::error("Method " + method_name_copy + " failed: " + std::string(e.what()));
                }
            };
            instance_binding_registry_t::get_instance().bind_method(instance_ptr, method_name_copy, bound_method);
        };
        
        return *this;
    }

    /**
     * Register a const method with no arguments.
     */
    template<typename ReturnType>
    auto method(const std::string& name, ReturnType (T::*method_ptr)() const) -> class_exporter_t<T>& {
        m_native_class->add_method(name, [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            return value_result_t::error("Method not bound to instance");
        });
        
        std::string method_name_copy = name;
        m_method_bindings[method_name_copy] = [method_ptr, method_name_copy](void* instance_ptr) {
            T* obj = static_cast<T*>(instance_ptr);
            auto bound_method = [obj, method_ptr, method_name_copy](const std::vector<value_t>& args) -> value_result_t {
                if (args.size() != 0) {
                    return value_result_t::error("Method " + method_name_copy + " expects 0 arguments, got " + std::to_string(args.size()));
                }
                
                try {
                    if constexpr (std::is_void_v<ReturnType>) {
                        (obj->*method_ptr)();
                        return value_result_t::success(std::make_shared<zephyr::none_object_t>());
                    } else {
                        auto result = (obj->*method_ptr)();
                        if constexpr (std::is_same_v<ReturnType, int>) {
                            return value_result_t::success(std::make_shared<zephyr::int_object_t>(result));
                        } else if constexpr (std::is_same_v<ReturnType, double> || std::is_same_v<ReturnType, float>) {
                            return value_result_t::success(std::make_shared<zephyr::float_object_t>(static_cast<double>(result)));
                        } else if constexpr (std::is_same_v<ReturnType, std::string>) {
                            return value_result_t::success(std::make_shared<zephyr::string_object_t>(result));
                        } else if constexpr (std::is_same_v<ReturnType, bool>) {
                            return value_result_t::success(std::make_shared<zephyr::boolean_object_t>(result));
                        } else {
                            return value_result_t::success(std::make_shared<zephyr::string_object_t>(std::to_string(result)));
                        }
                    }
                } catch (const std::exception& e) {
                    return value_result_t::error("Method " + method_name_copy + " failed: " + std::string(e.what()));
                }
            };
            instance_binding_registry_t::get_instance().bind_method(instance_ptr, method_name_copy, bound_method);
        };
        
        return *this;
    }

    /**
     * Register a method with one argument.
     */
    template<typename ReturnType, typename ArgType>
    auto method(const std::string& name, ReturnType (T::*method_ptr)(ArgType)) -> class_exporter_t<T>& {
        m_native_class->add_method(name, [method_ptr](const std::vector<value_t>& args) -> value_result_t {
            return value_result_t::error("Method not bound to instance");
        });
        
        std::string method_name_copy = name;
        m_method_bindings[method_name_copy] = [method_ptr, method_name_copy](void* instance_ptr) {
            T* obj = static_cast<T*>(instance_ptr);
            auto bound_method = [obj, method_ptr, method_name_copy](const std::vector<value_t>& args) -> value_result_t {
                if (args.size() != 1) {
                    return value_result_t::error("Method " + method_name_copy + " expects 1 argument, got " + std::to_string(args.size()));
                }
                
                try {
                    using PlainArgType = std::remove_cv_t<std::remove_reference_t<ArgType>>;
                    PlainArgType arg_value;
                    
                    if constexpr (std::is_same_v<PlainArgType, int>) {
                        if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(args[0])) {
                            arg_value = int_obj->value();
                        } else {
                            return value_result_t::error("Expected int argument for method " + method_name_copy);
                        }
                    } else if constexpr (std::is_same_v<PlainArgType, double>) {
                        if (auto float_obj = std::dynamic_pointer_cast<zephyr::float_object_t>(args[0])) {
                            arg_value = float_obj->value();
                        } else if (auto int_obj = std::dynamic_pointer_cast<zephyr::int_object_t>(args[0])) {
                            arg_value = static_cast<double>(int_obj->value());
                        } else {
                            return value_result_t::error("Expected numeric argument for method " + method_name_copy);
                        }
                    } else if constexpr (std::is_same_v<PlainArgType, std::string>) {
                        if (auto str_obj = std::dynamic_pointer_cast<zephyr::string_object_t>(args[0])) {
                            arg_value = str_obj->value();
                        } else {
                            return value_result_t::error("Expected string argument for method " + method_name_copy);
                        }
                    } else {
                        return value_result_t::error("Unsupported argument type for method " + method_name_copy);
                    }
                    
                    if constexpr (std::is_void_v<ReturnType>) {
                        (obj->*method_ptr)(arg_value);
                        return value_result_t::success(std::make_shared<zephyr::none_object_t>());
                    } else {
                        auto result = (obj->*method_ptr)(arg_value);
                        if constexpr (std::is_same_v<ReturnType, int>) {
                            return value_result_t::success(std::make_shared<zephyr::int_object_t>(result));
                        } else if constexpr (std::is_same_v<ReturnType, double> || std::is_same_v<ReturnType, float>) {
                            return value_result_t::success(std::make_shared<zephyr::float_object_t>(static_cast<double>(result)));
                        } else if constexpr (std::is_same_v<ReturnType, std::string>) {
                            return value_result_t::success(std::make_shared<zephyr::string_object_t>(result));
                        } else if constexpr (std::is_same_v<ReturnType, bool>) {
                            return value_result_t::success(std::make_shared<zephyr::boolean_object_t>(result));
                        } else {
                            return value_result_t::success(std::make_shared<zephyr::string_object_t>(std::to_string(result)));
                        }
                    }
                } catch (const std::exception& e) {
                    return value_result_t::error("Method " + method_name_copy + " failed: " + std::string(e.what()));
                }
            };
            instance_binding_registry_t::get_instance().bind_method(instance_ptr, method_name_copy, bound_method);
        };
        
        return *this;
    }

    /**
     * Get the native class definition.
     */
    auto get_native_class() -> std::shared_ptr<native_class_t> {
        return m_native_class;
    }
    
    /**
     * Export the class to a native module using export_function instead of export_class
     * for better compatibility with the Zephyr interpreter.
     */
    auto export_to_module(native_module_t* module) -> void {
        // Export the constructor as a function
        std::string class_name = m_class_name;  // Capture by value to avoid lambda capture issues
        auto native_class = m_native_class;     // Capture native class by value
        module->export_function(class_name, [class_name, native_class](const std::vector<value_t>& args) -> value_result_t {
            std::cout << "DEBUG: Constructor function called for " << class_name << " with " << args.size() << " arguments" << std::endl;
            try {
                auto result = native_class->create_instance(args);
                if (result) {
                    std::cout << "DEBUG: create_instance succeeded for " << class_name << std::endl;
                } else {
                    std::cout << "DEBUG: create_instance failed for " << class_name << ": " << result.error() << std::endl;
                }
                return result;
            } catch (const std::exception& e) {
                std::cout << "DEBUG: Exception in constructor for " << class_name << ": " << e.what() << std::endl;
                return value_result_t::error("Constructor exception: " + std::string(e.what()));
            }
        });
    }

    /**
     * Concrete implementation of native_class_t for our exported class.
     */
    class concrete_native_class_t : public native_class_t {
    public:
        auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
            std::cout << "DEBUG: concrete_native_class_t::create_instance called for " << get_name() << std::endl;
            if (m_constructor) {
                std::cout << "DEBUG: Calling m_constructor for " << get_name() << std::endl;
                auto result = m_constructor(args);
                
                // If successful, bind members and methods to the instance
                if (result && m_class_exporter) {
                    std::cout << "DEBUG: Constructor succeeded, binding members and methods for " << get_name() << std::endl;
                    if (auto wrapper = std::dynamic_pointer_cast<enhanced_cpp_object_wrapper_t<T>>(result.value())) {
                        std::cout << "DEBUG: Got wrapper, binding to instance" << std::endl;
                        void* instance_ptr = wrapper->get_instance_ptr();
                        
                        // Safety check for instance pointer
                        if (!instance_ptr) {
                            std::cout << "DEBUG: ERROR - instance_ptr is null" << std::endl;
                            return value_result_t::error("Instance pointer is null");
                        }
                        
                        // Bind all registered members
                        for (const auto& [name, binding] : m_class_exporter->m_member_bindings) {
                            binding(instance_ptr);
                        }
                        
                        // Bind all registered methods
                        for (const auto& [name, binding] : m_class_exporter->m_method_bindings) {
                            binding(instance_ptr);
                        }
                        std::cout << "DEBUG: All bindings complete for " << get_name() << std::endl;
                    } else {
                        std::cout << "DEBUG: Failed to cast to enhanced_cpp_object_wrapper_t for " << get_name() << std::endl;
                    }
                } else if (!result) {
                    std::cout << "DEBUG: Constructor failed for " << get_name() << ": " << result.error() << std::endl;
                } else {
                    std::cout << "DEBUG: Constructor succeeded but no class_exporter for " << get_name() << std::endl;
                }
                
                return result;
            }
            std::cout << "DEBUG: No constructor registered for class " << get_name() << std::endl;
            return value_result_t::error("No constructor registered for class " + get_name());
        }
        
        auto set_class_exporter(class_exporter_t<T>* exporter) -> void {
            m_class_exporter = exporter;
        }

    private:
        class_exporter_t<T>* m_class_exporter = nullptr;
    };

    // Type conversion helpers



private:
    std::string m_class_name;
    std::shared_ptr<concrete_native_class_t> m_native_class;
    std::map<std::string, std::function<void(void*)>> m_member_bindings;
    std::map<std::string, std::function<void(void*)>> m_method_bindings;
    
    friend class concrete_native_class_t;
};

/**
 * Helper function to create a class exporter.
 */
template<typename T>
auto make_class_exporter(const std::string& class_name) -> class_exporter_t<T> {
    return class_exporter_t<T>(class_name);
}

} // namespace zephyr::api