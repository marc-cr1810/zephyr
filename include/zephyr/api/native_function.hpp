#pragma once

#include "zephyr/objects/object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/api/result.hpp"
#include "zephyr/api/type_converter.hpp"
#include <functional>
#include <tuple>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

namespace zephyr::api {

// Forward declarations
using value_t = std::shared_ptr<object_t>;

// Native function signature - takes args and returns result
using native_function_t = std::function<value_result_t(const std::vector<value_t>&)>;

// Property getter/setter signatures
using property_getter_t = std::function<value_result_t()>;
using property_setter_t = std::function<void_result_t(const value_t&)>;

// =============================================================================
// Function Wrapper Templates
// =============================================================================

// Helper to check if type is void
template<typename T>
struct is_void : std::is_same<T, void> {};

// Helper to get argument count
template<typename F>
struct function_arity;

template<typename R, typename... Args>
struct function_arity<R(Args...)> : std::integral_constant<size_t, sizeof...(Args)> {};

template<typename R, typename... Args>
struct function_arity<R(*)(Args...)> : std::integral_constant<size_t, sizeof...(Args)> {};

template<typename R, typename C, typename... Args>
struct function_arity<R(C::*)(Args...)> : std::integral_constant<size_t, sizeof...(Args)> {};

template<typename F>
struct function_arity : function_arity<decltype(&F::operator())> {};

// Helper to extract function signature
template<typename F>
struct function_signature;

template<typename R, typename... Args>
struct function_signature<R(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

template<typename R, typename... Args>
struct function_signature<R(*)(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

template<typename R, typename C, typename... Args>
struct function_signature<R(C::*)(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

template<typename F>
struct function_signature : function_signature<decltype(&F::operator())> {};

// =============================================================================
// Native Function Wrapper Class
// =============================================================================

template<typename R, typename... Args>
class native_function_wrapper_t {
public:
    using function_type = std::function<R(Args...)>;
    using return_type = R;
    static constexpr size_t arity = sizeof...(Args);
    
    native_function_wrapper_t(function_type func, const std::string& name = "")
        : m_func(std::move(func)), m_name(name) {}
    
    auto operator()(const std::vector<value_t>& args) -> value_result_t {
        // Validate argument count
        if (args.size() != arity) {
            return value_result_t::error(create_argument_count_error(args.size()));
        }
        
        try {
            // Convert arguments and call function
            return call_with_conversion(args, std::index_sequence_for<Args...>{});
        } catch (const plugin_exception_t& e) {
            return value_result_t::error(e.info());
        } catch (const std::exception& e) {
            return value_result_t::error(create_plugin_error(
                "Function execution failed: " + std::string(e.what()),
                m_name
            ));
        }
    }
    
    auto get_name() const -> std::string { return m_name; }
    auto set_name(const std::string& name) -> void { m_name = name; }
    
    static constexpr auto get_arity() -> size_t { return arity; }

private:
    template<std::size_t... I>
    auto call_with_conversion(const std::vector<value_t>& args, std::index_sequence<I...>) -> value_result_t {
        // Convert each argument
        auto converted_args = std::make_tuple(convert_arg<Args>(args[I], I)...);
        
        // Check if any conversion failed (this would throw, but we catch above)
        
        // Call the function with converted arguments
        if constexpr (is_void<R>::value) {
            m_func(std::get<I>(converted_args)...);
            return value_result_t::success(std::make_shared<none_object_t>());
        } else {
            auto result = m_func(std::get<I>(converted_args)...);
            return value_result_t::success(to_zephyr(result));
        }
    }
    
    template<typename T>
    auto convert_arg(const value_t& arg, size_t index) -> T {
        auto result = from_zephyr<T>(arg);
        if (!result) {
            throw plugin_exception_t(create_plugin_error(
                "Argument " + std::to_string(index) + " type mismatch: " + result.error(),
                m_name
            ));
        }
        return result.value();
    }
    
    auto create_argument_count_error(size_t actual_count) -> error_info_t {
        return create_plugin_error(
            "Expected " + std::to_string(arity) + " arguments, got " + std::to_string(actual_count),
            m_name,
            "ArgumentError"
        );
    }
    
    function_type m_func;
    std::string m_name;
};

// =============================================================================
// Simple Function Wrapper (No Template Deduction)
// =============================================================================

// Simple wrapper for functions that already return value_result_t
template<typename F>
auto make_simple_native_function(F&& func, const std::string& name = "") -> native_function_t {
    return [func = std::forward<F>(func), name](const std::vector<value_t>& args) -> value_result_t {
        try {
            return func(args);
        } catch (const plugin_exception_t& e) {
            return value_result_t::error(e.info());
        } catch (const std::exception& e) {
            return value_result_t::error(create_plugin_error(
                e.what(), 
                name.empty() ? "lambda" : name
            ));
        }
    };
}

// =============================================================================
// Factory Functions
// =============================================================================

// Create native function from std::function
template<typename R, typename... Args>
auto make_native_function(std::function<R(Args...)> func, const std::string& name = "") -> native_function_t {
    auto wrapper = native_function_wrapper_t<R, Args...>(func, name);
    return [wrapper](const std::vector<value_t>& args) -> value_result_t {
        return wrapper(args);
    };
}

// Create native function from function pointer
template<typename R, typename... Args>
auto make_native_function(R(*func)(Args...), const std::string& name = "") -> native_function_t {
    return make_native_function(std::function<R(Args...)>(func), name);
}

// Helper for tuple-based function creation (moved before usage)
template<typename R, typename... Args>
auto make_native_function_from_tuple(std::function<R(Args...)> func, 
                                    std::tuple<Args...>, 
                                    const std::string& name = "") -> native_function_t {
    return make_native_function<R, Args...>(func, name);
}

// Create native function from lambda/functor
template<typename F>
auto make_native_function(F&& func, const std::string& name = "") -> native_function_t {
    using sig = function_signature<std::decay_t<F>>;
    using R = typename sig::return_type;
    using args_tuple = typename sig::args_tuple;
    
    return make_native_function_from_tuple<R>(
        std::function(std::forward<F>(func)), 
        args_tuple{}, 
        name
    );
}

// =============================================================================
// Method Wrappers (for class instances)
// =============================================================================

template<typename R, typename C, typename... Args>
class native_method_wrapper_t {
public:
    using method_type = R(C::*)(Args...);
    using const_method_type = R(C::*)(Args...) const;
    
    native_method_wrapper_t(method_type method, std::shared_ptr<C> instance, const std::string& name = "")
        : m_method(method), m_instance(instance), m_name(name) {}
    
    native_method_wrapper_t(const_method_type method, std::shared_ptr<C> instance, const std::string& name = "")
        : m_const_method(method), m_instance(instance), m_name(name) {}
    
    auto operator()(const std::vector<value_t>& args) -> value_result_t {
        if (args.size() != sizeof...(Args)) {
            return value_result_t::error(create_plugin_error(
                "Expected " + std::to_string(sizeof...(Args)) + " arguments, got " + std::to_string(args.size()),
                m_name,
                "ArgumentError"
            ));
        }
        
        try {
            return call_method(args, std::index_sequence_for<Args...>{});
        } catch (const plugin_exception_t& e) {
            return value_result_t::error(e.info());
        } catch (const std::exception& e) {
            return value_result_t::error(create_plugin_error(
                "Method execution failed: " + std::string(e.what()),
                m_name
            ));
        }
    }

private:
    template<std::size_t... I>
    auto call_method(const std::vector<value_t>& args, std::index_sequence<I...>) -> value_result_t {
        auto converted_args = std::make_tuple(convert_arg<Args>(args[I], I)...);
        
        if constexpr (is_void<R>::value) {
            if (m_method) {
                (m_instance.get()->*m_method)(std::get<I>(converted_args)...);
            } else {
                (m_instance.get()->*m_const_method)(std::get<I>(converted_args)...);
            }
            return value_result_t::success(std::make_shared<none_object_t>());
        } else {
            R result;
            if (m_method) {
                result = (m_instance.get()->*m_method)(std::get<I>(converted_args)...);
            } else {
                result = (m_instance.get()->*m_const_method)(std::get<I>(converted_args)...);
            }
            return value_result_t::success(to_zephyr(result));
        }
    }
    
    template<typename T>
    auto convert_arg(const value_t& arg, size_t index) -> T {
        auto result = from_zephyr<T>(arg);
        if (!result) {
            throw plugin_exception_t(create_plugin_error(
                "Argument " + std::to_string(index) + " type mismatch: " + result.error(),
                m_name
            ));
        }
        return result.value();
    }
    
    std::optional<method_type> m_method;
    std::optional<const_method_type> m_const_method;
    std::shared_ptr<C> m_instance;
    std::string m_name;
};

// Factory functions for methods
template<typename R, typename C, typename... Args>
auto make_native_method(R(C::*method)(Args...), std::shared_ptr<C> instance, const std::string& name = "") -> native_function_t {
    auto wrapper = native_method_wrapper_t<R, C, Args...>(method, instance, name);
    return [wrapper](const std::vector<value_t>& args) -> value_result_t {
        return wrapper(args);
    };
}

template<typename R, typename C, typename... Args>
auto make_native_method(R(C::*method)(Args...) const, std::shared_ptr<C> instance, const std::string& name = "") -> native_function_t {
    auto wrapper = native_method_wrapper_t<R, C, Args...>(method, instance, name);
    return [wrapper](const std::vector<value_t>& args) -> value_result_t {
        return wrapper(args);
    };
}

// =============================================================================
// Property Wrappers
// =============================================================================

// Property getter wrapper
template<typename T, typename C>
class native_property_getter_t {
public:
    using getter_type = T(C::*)() const;
    using field_type = T C::*;
    
    native_property_getter_t(getter_type getter, std::shared_ptr<C> instance, const std::string& name = "")
        : m_getter(getter), m_instance(instance), m_name(name) {}
    
    native_property_getter_t(field_type field, std::shared_ptr<C> instance, const std::string& name = "")
        : m_field(field), m_instance(instance), m_name(name) {}
    
    auto operator()() -> value_result_t {
        try {
            if (m_getter) {
                auto result = (m_instance.get()->*m_getter.value())();
                return value_result_t::success(to_zephyr(result));
            } else {
                auto result = m_instance.get()->*m_field.value();
                return value_result_t::success(to_zephyr(result));
            }
        } catch (const std::exception& e) {
            return value_result_t::error(create_plugin_error(
                "Property getter failed: " + std::string(e.what()),
                m_name
            ));
        }
    }

private:
    std::optional<getter_type> m_getter;
    std::optional<field_type> m_field;
    std::shared_ptr<C> m_instance;
    std::string m_name;
};

// Property setter wrapper
template<typename T, typename C>
class native_property_setter_t {
public:
    using setter_type = void(C::*)(const T&);
    using field_type = T C::*;
    
    native_property_setter_t(setter_type setter, std::shared_ptr<C> instance, const std::string& name = "")
        : m_setter(setter), m_instance(instance), m_name(name) {}
    
    native_property_setter_t(field_type field, std::shared_ptr<C> instance, const std::string& name = "")
        : m_field(field), m_instance(instance), m_name(name) {}
    
    auto operator()(const value_t& value) -> void_result_t {
        try {
            auto converted_value = from_zephyr<T>(value);
            if (!converted_value) {
                return void_result_t::error(create_plugin_error(
                    "Property setter type mismatch: " + converted_value.error(),
                    m_name
                ));
            }
            
            if (m_setter) {
                (m_instance.get()->*m_setter.value())(converted_value.value());
            } else {
                m_instance.get()->*m_field.value() = converted_value.value();
            }
            
            return void_result_t::success();
        } catch (const std::exception& e) {
            return void_result_t::error(create_plugin_error(
                "Property setter failed: " + std::string(e.what()),
                m_name
            ));
        }
    }

private:
    std::optional<setter_type> m_setter;
    std::optional<field_type> m_field;
    std::shared_ptr<C> m_instance;
    std::string m_name;
};

// Factory functions for properties
template<typename T, typename C>
auto make_property_getter(T(C::*getter)() const, std::shared_ptr<C> instance, const std::string& name = "") -> property_getter_t {
    auto wrapper = native_property_getter_t<T, C>(getter, instance, name);
    return [wrapper]() -> value_result_t {
        return wrapper();
    };
}

template<typename T, typename C>
auto make_property_getter(T C::*field, std::shared_ptr<C> instance, const std::string& name = "") -> property_getter_t {
    auto wrapper = native_property_getter_t<T, C>(field, instance, name);
    return [wrapper]() -> value_result_t {
        return wrapper();
    };
}

template<typename T, typename C>
auto make_property_setter(void(C::*setter)(const T&), std::shared_ptr<C> instance, const std::string& name = "") -> property_setter_t {
    auto wrapper = native_property_setter_t<T, C>(setter, instance, name);
    return [wrapper](const value_t& value) -> void_result_t {
        return wrapper(value);
    };
}

template<typename T, typename C>
auto make_property_setter(T C::*field, std::shared_ptr<C> instance, const std::string& name = "") -> property_setter_t {
    auto wrapper = native_property_setter_t<T, C>(field, instance, name);
    return [wrapper](const value_t& value) -> void_result_t {
        return wrapper(value);
    };
}

// =============================================================================
// Convenience Macros
// =============================================================================

// Macro for easy function registration with automatic type conversion
#define ZEPHYR_FUNCTION(name, func) \
    export_function(name, zephyr::api::make_native_function(func, name))

// Macro for simple functions that already handle value_t conversion
#define ZEPHYR_FUNCTION_SIMPLE(name, func) \
    export_function(name, zephyr::api::make_simple_native_function(func, name))

// Macro for method registration
#define ZEPHYR_METHOD(name, method, instance) \
    add_method(name, zephyr::api::make_native_method(method, instance, name))

// Macro for property registration
#define ZEPHYR_PROPERTY(name, getter, setter, instance) \
    add_property(name, \
        zephyr::api::make_property_getter(getter, instance, name "_getter"), \
        zephyr::api::make_property_setter(setter, instance, name "_setter"))

// Macro for read-only property
#define ZEPHYR_READONLY_PROPERTY(name, getter, instance) \
    add_property_getter(name, zephyr::api::make_property_getter(getter, instance, name "_getter"))

} // namespace zephyr::api