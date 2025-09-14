#include "zephyr/api/native_module.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/objects/module_object.hpp"
#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/builtin_function_object.hpp"

namespace zephyr::api {

// =============================================================================
// Minimal Function Adapter (Temporary Solution)
// =============================================================================

// Simple wrapper class that stores native functions
class native_function_wrapper_t : public builtin_function_object_t {
private:
    native_function_t m_native_func;
    
public:
    native_function_wrapper_t(const native_function_t& func, const std::string& name) 
        : builtin_function_object_t(nullptr, name), m_native_func(func) {}
    
    // Override the call method to use our native function
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override {
        auto result = m_native_func(args);
        if (result) {
            return result.value();
        } else {
            throw std::runtime_error(result.error());
        }
    }
};

// =============================================================================
// Native Class Implementation
// =============================================================================

auto native_class_t::create_zephyr_class(const std::string& name) -> value_result_t {
    try {
        // Create a new class object
        auto class_obj = std::make_shared<class_object_t>(name);
        
        // For now, we'll create a simple class that can be instantiated
        // Full method binding would require extending the existing class system
        
        return value_result_t::success(class_obj);
        
    } catch (const std::exception& e) {
        return value_result_t::error("Failed to create class '" + name + "': " + e.what());
    }
}

// =============================================================================
// Native Module Implementation  
// =============================================================================

auto native_module_t::get_symbol(const std::string& name) const -> std::optional<value_t> {
    // Check functions first
    auto func_it = m_functions.find(name);
    if (func_it != m_functions.end()) {
        // Create our custom wrapper that can handle native functions
        auto wrapper = std::make_shared<native_function_wrapper_t>(func_it->second, name);
        return wrapper;
    }
    
    // Check classes
    auto class_it = m_classes.find(name);
    if (class_it != m_classes.end()) {
        auto class_result = class_it->second->create_zephyr_class(name);
        if (class_result) {
            return class_result.value();
        }
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

// Simple wrapper object that holds a C++ object instance
template<typename T>
class cpp_object_wrapper_t : public object_t {
public:
    explicit cpp_object_wrapper_t(std::shared_ptr<T> obj) : m_cpp_object(obj) {}
    
    auto type() const -> std::shared_ptr<type_t> override {
        // Return nullptr for now - would need proper type system integration
        return nullptr;
    }
    
    auto to_string() const -> std::string override {
        return "cpp_object<" + std::string(typeid(T).name()) + ">";
    }
    
    // Get the wrapped C++ object
    auto get_cpp_object() const -> std::shared_ptr<T> {
        return m_cpp_object;
    }
    
private:
    std::shared_ptr<T> m_cpp_object;
};

// Implementation of the wrap_cpp_object method
template<typename T>
auto cpp_class_wrapper_t<T>::wrap_cpp_object(std::shared_ptr<T> obj) -> value_t {
    return std::make_shared<cpp_object_wrapper_t<T>>(obj);
}

// Explicit template instantiation for common types (add more as needed)
template class cpp_class_wrapper_t<int>;
template class cpp_class_wrapper_t<double>;
template class cpp_class_wrapper_t<std::string>;

} // namespace zephyr::api