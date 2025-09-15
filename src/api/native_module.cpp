#include "zephyr/api/native_module.hpp"
#include "zephyr/api/cpp_object_wrapper.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/objects/module_object.hpp"
#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include "zephyr/objects/builtin_function_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/types/builtin_function_type.hpp"
#include <iostream>

namespace zephyr::api {

// =============================================================================
// Minimal Function Adapter (Temporary Solution)
// =============================================================================

// Simple wrapper class that stores native functions
class native_function_wrapper_t : public object_t {
private:
    native_function_t m_native_func;
    std::string m_name;
    
public:
    native_function_wrapper_t(const native_function_t& func, const std::string& name) 
        : m_native_func(func), m_name(name) {}
    
    auto type() const -> std::shared_ptr<type_t> override {
        return builtin_function_type_t::get_instance();
    }
    
    auto to_string() const -> std::string override {
        return "native_function<" + m_name + ">";
    }
    
    // Override the call method to use our native function
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override {
        std::cout << "DEBUG: native_function_wrapper_t::call() called for " << m_name << " with " << args.size() << " arguments" << std::endl;
        auto result = m_native_func(args);
        if (result) {
            std::cout << "DEBUG: native_function_wrapper_t::call() succeeded for " << m_name << std::endl;
            return result.value();
        } else {
            std::cout << "DEBUG: native_function_wrapper_t::call() failed for " << m_name << ": " << result.error() << std::endl;
            throw std::runtime_error(result.error());
        }
    }
};

// =============================================================================
// Native Class Implementation
// =============================================================================

// Custom class object that handles native C++ object creation
class native_class_object_t : public class_object_t {
private:
    std::shared_ptr<native_class_t> m_native_class;
    
public:
    native_class_object_t(const std::string& name, std::shared_ptr<native_class_t> native_cls) 
        : class_object_t(name), m_native_class(native_cls) {
        
        // Add a dummy member variable for init to make has_method return true
        member_variable_info_t init_info("init", "function", nullptr, false, false);
        add_member_variable(init_info);
    }
    
    auto has_method(const std::string& method_name) const -> bool {
        std::cout << "DEBUG: native_class_object_t::has_method called for '" << method_name << "'" << std::endl;
        if (method_name == "init") {
            std::cout << "DEBUG: Returning true for init method" << std::endl;
            return true;
        }
        bool result = class_object_t::has_method(method_name);
        std::cout << "DEBUG: class_object_t::has_method('" << method_name << "') returned " << (result ? "true" : "false") << std::endl;
        return result;
    }
    
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override {
        try {
            // Always create using the native constructor - it handles both cases
            auto result = m_native_class->create_instance(args);
            if (result) {
                return result.value();
            } else {
                throw std::runtime_error("Constructor failed: " + result.error());
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("Class instantiation failed: " + std::string(e.what()));
        }
    }
};

auto native_class_t::create_zephyr_class(const std::string& name) -> value_result_t {
    try {
        // Create a custom class object that uses our native constructor
        auto class_obj = std::make_shared<native_class_object_t>(name, 
            std::static_pointer_cast<native_class_t>(shared_from_this()));
        
        return value_result_t::success(class_obj);
        
    } catch (const std::exception& e) {
        return value_result_t::error("Failed to create class '" + name + "': " + e.what());
    }
}

// =============================================================================
// Native Module Implementation  
// =============================================================================

auto native_module_t::get_symbol(const std::string& name) const -> std::optional<value_t> {
    std::cout << "DEBUG: get_symbol called for '" << name << "'" << std::endl;
    
    // Check functions first
    auto func_it = m_functions.find(name);
    if (func_it != m_functions.end()) {
        std::cout << "DEBUG: Found function '" << name << "'" << std::endl;
        // Create our custom wrapper that can handle native functions
        auto wrapper = std::make_shared<native_function_wrapper_t>(func_it->second, name);
        return wrapper;
    }
    
    // Check classes
    auto class_it = m_classes.find(name);
    if (class_it != m_classes.end()) {
        std::cout << "DEBUG: Found class '" << name << "'" << std::endl;
        auto class_result = class_it->second->create_zephyr_class(name);
        if (class_result) {
            std::cout << "DEBUG: Successfully created Zephyr class for '" << name << "'" << std::endl;
            return class_result.value();
        } else {
            std::cout << "DEBUG: Failed to create Zephyr class for '" << name << "': " << class_result.error() << std::endl;
        }
    } else {
        std::cout << "DEBUG: Class '" << name << "' not found in m_classes" << std::endl;
        std::cout << "DEBUG: Available classes: ";
        for (const auto& [class_name, _] : m_classes) {
            std::cout << class_name << " ";
        }
        std::cout << std::endl;
    }
    
    // Check constants
    auto const_it = m_constants.find(name);
    if (const_it != m_constants.end()) {
        return const_it->second;
    }
    
    // Check variables
    auto var_it = m_variables.find(name);
    if (var_it != m_variables.end()) {
        return var_it->second;
    }
    
    return std::nullopt;
}

auto native_module_t::get_all_symbol_values() const -> std::map<std::string, value_t> {
    std::map<std::string, value_t> symbols;
    
    // Add functions - using the same approach as get_symbol()
    for (const auto& [name, func] : m_functions) {
        auto symbol = get_symbol(name);
        if (symbol) {
            symbols[name] = symbol.value();
        }
    }
    
    // Add classes
    for (const auto& [name, cls] : m_classes) {
        auto class_result = cls->create_zephyr_class(name);
        if (class_result) {
            symbols[name] = class_result.value();
        }
    }
    
    // Add constants
    for (const auto& [name, value] : m_constants) {
        symbols[name] = value;
    }
    
    // Add variables
    for (const auto& [name, value] : m_variables) {
        symbols[name] = value;
    }
    
    return symbols;
}

auto native_module_t::get_exported_symbol_values() const -> std::map<std::string, value_t> {
    auto all_symbols = get_all_symbol_values();
    std::map<std::string, value_t> exported_symbols;
    
    for (const auto& [name, value] : all_symbols) {
        if (is_exported(name)) {
            exported_symbols[name] = value;
        }
    }
    
    return exported_symbols;
}

// Create a custom module object that works with our native module
class native_module_object_t : public object_t {
public:
    explicit native_module_object_t(std::shared_ptr<native_module_t> native_mod, const std::string& name)
        : m_native_module(native_mod), m_module_name(name) {}
    
    auto type() const -> std::shared_ptr<type_t> override {
        // Return a module type - this would need to be implemented
        // For now, return nullptr and handle in the calling code
        return nullptr;
    }
    
    auto to_string() const -> std::string override {
        return "module<" + m_module_name + ">";
    }
    
    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override {
        // Try to get the function from the native module
        auto func = m_native_module->get_function(method_name);
        if (func) {
            auto result = func.value()(args);
            if (result) {
                return result.value();
            } else {
                throw std::runtime_error("Method call failed: " + result.error());
            }
        }
        
        throw std::runtime_error("Method not found: " + method_name);
    }
    
    auto member(const std::string& member_name) -> std::shared_ptr<object_t> override {
        // Try to get any symbol from the native module
        auto symbol = m_native_module->get_symbol(member_name);
        if (symbol) {
            return symbol.value();
        }
        
        throw std::runtime_error("Member not found: " + member_name);
    }

private:
    std::shared_ptr<native_module_t> m_native_module;
    std::string m_module_name;
};

auto native_module_t::create_module_object() -> value_result_t {
    try {
        // Create our custom module object
        auto module_obj = std::make_shared<native_module_object_t>(
            std::static_pointer_cast<native_module_t>(shared_from_this()), 
            m_name
        );
        
        return value_result_t::success(module_obj);
        
    } catch (const std::exception& e) {
        return value_result_t::error("Failed to create module object: " + std::string(e.what()));
    }
}

// =============================================================================
// Factory Functions
// =============================================================================

auto make_native_module(const std::string& name) -> std::shared_ptr<native_module_t> {
    auto module = std::make_shared<native_module_t>();
    if (!name.empty()) {
        module->set_name(name);
    }
    return module;
}

// =============================================================================
// C++ Class Wrapper Implementation
// =============================================================================

// Simple concrete native class for basic wrapping
class simple_native_class_t : public native_class_t {
public:
    auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
        return value_result_t::error("Simple native class cannot create instances");
    }
};

// Implementation of the wrap_cpp_object method
template<typename T>
auto cpp_class_wrapper_t<T>::wrap_cpp_object(std::shared_ptr<T> obj) -> value_t {
    // Create a new native class instance - this is a simple fallback implementation
    auto native_class = std::make_shared<simple_native_class_t>();
    return std::make_shared<enhanced_cpp_object_wrapper_t<T>>(obj, native_class);
}

// Explicit template instantiation for common types (add more as needed)
template class cpp_class_wrapper_t<int>;
template class cpp_class_wrapper_t<double>;
template class cpp_class_wrapper_t<std::string>;

} // namespace zephyr::api