#pragma once

#include "zephyr/objects/object.hpp"
#include "zephyr/api/result.hpp"
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <any>
#include <set>

namespace zephyr::api {

// Forward declarations
class engine_t;
using value_t = std::shared_ptr<object_t>;

// Native function signature - takes args and returns result
using native_function_t = std::function<value_result_t(const std::vector<value_t>&)>;

// Property getter/setter signatures
using property_getter_t = std::function<value_result_t()>;
using property_setter_t = std::function<void_result_t(const value_t&)>;

// Native class interface for C++ classes exposed to Zephyr
class native_class_t : public std::enable_shared_from_this<native_class_t> {
public:
    virtual ~native_class_t() = default;
    
    // Constructor function
    auto set_constructor(native_function_t constructor) -> void {
        m_constructor = constructor;
    }
    
    // Method registration
    auto add_method(const std::string& name, native_function_t method) -> void {
        m_methods[name] = method;
    }
    
    auto add_static_method(const std::string& name, native_function_t method) -> void {
        m_static_methods[name] = method;
    }
    
    // Property registration
    auto add_property_getter(const std::string& name, property_getter_t getter) -> void {
        m_property_getters[name] = getter;
    }
    
    auto add_property_setter(const std::string& name, property_setter_t setter) -> void {
        m_property_setters[name] = setter;
    }
    
    auto add_property(const std::string& name, 
                     property_getter_t getter, 
                     property_setter_t setter = nullptr) -> void {
        add_property_getter(name, getter);
        if (setter) {
            add_property_setter(name, setter);
        }
    }
    
    // Access methods
    auto has_method(const std::string& name) const -> bool {
        return m_methods.find(name) != m_methods.end();
    }
    
    auto has_static_method(const std::string& name) const -> bool {
        return m_static_methods.find(name) != m_static_methods.end();
    }
    
    auto has_property(const std::string& name) const -> bool {
        return m_property_getters.find(name) != m_property_getters.end();
    }
    
    auto get_method(const std::string& name) const -> std::optional<native_function_t> {
        auto it = m_methods.find(name);
        return it != m_methods.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    auto get_static_method(const std::string& name) const -> std::optional<native_function_t> {
        auto it = m_static_methods.find(name);
        return it != m_static_methods.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    auto get_property_getter(const std::string& name) const -> std::optional<property_getter_t> {
        auto it = m_property_getters.find(name);
        return it != m_property_getters.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    auto get_property_setter(const std::string& name) const -> std::optional<property_setter_t> {
        auto it = m_property_setters.find(name);
        return it != m_property_setters.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    // Instance creation
    virtual auto create_instance(const std::vector<value_t>& args) -> value_result_t = 0;
    
    // Create Zephyr class object from this native class
    auto create_zephyr_class(const std::string& name) -> value_result_t;
    
    // Access methods
    auto get_methods() const -> const std::map<std::string, native_function_t>& { return m_methods; }
    
    // Metadata
    auto set_name(const std::string& name) -> void { m_name = name; }
    auto get_name() const -> std::string { return m_name; }
    
    auto set_description(const std::string& description) -> void { m_description = description; }
    auto get_description() const -> std::string { return m_description; }

protected:
    std::string m_name;
    std::string m_description;
    std::map<std::string, native_function_t> m_methods;
    std::map<std::string, native_function_t> m_static_methods;
    std::map<std::string, property_getter_t> m_property_getters;
    std::map<std::string, property_setter_t> m_property_setters;
    native_function_t m_constructor;
};

// Native module interface - represents a collection of functions, classes, and constants
class native_module_t : public std::enable_shared_from_this<native_module_t> {
public:
    virtual ~native_module_t() = default;
    
    // Export functions
    auto export_function(const std::string& name, native_function_t func) -> void {
        m_functions[name] = func;
        m_exports.insert(name);
    }
    
    // Export classes
    auto export_class(const std::string& name, std::shared_ptr<native_class_t> cls) -> void {
        cls->set_name(name);
        m_classes[name] = cls;
        m_exports.insert(name);
    }
    
    // Export constants (immutable values)
    auto export_constant(const std::string& name, value_t value) -> void {
        m_constants[name] = value;
        m_exports.insert(name);
    }
    
    // Export variables (mutable values)
    auto export_variable(const std::string& name, value_t value) -> void {
        m_variables[name] = value;
        m_exports.insert(name);
    }
    
    // Internal exports (not visible outside module unless explicitly imported)
    auto export_internal_function(const std::string& name, native_function_t func) -> void {
        m_functions[name] = func;
        // Not added to m_exports
    }
    
    auto export_internal_constant(const std::string& name, value_t value) -> void {
        m_constants[name] = value;
        // Not added to m_exports
    }
    
    // Access methods
    auto has_function(const std::string& name) const -> bool {
        return m_functions.find(name) != m_functions.end();
    }
    
    auto has_class(const std::string& name) const -> bool {
        return m_classes.find(name) != m_classes.end();
    }
    
    auto has_constant(const std::string& name) const -> bool {
        return m_constants.find(name) != m_constants.end();
    }
    
    auto has_variable(const std::string& name) const -> bool {
        return m_variables.find(name) != m_variables.end();
    }
    
    auto has_symbol(const std::string& name) const -> bool {
        return has_function(name) || has_class(name) || 
               has_constant(name) || has_variable(name);
    }
    
    auto is_exported(const std::string& name) const -> bool {
        return m_exports.find(name) != m_exports.end();
    }
    
    // Get symbols
    auto get_function(const std::string& name) const -> std::optional<native_function_t> {
        auto it = m_functions.find(name);
        return it != m_functions.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    auto get_class(const std::string& name) const -> std::optional<std::shared_ptr<native_class_t>> {
        auto it = m_classes.find(name);
        return it != m_classes.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    auto get_constant(const std::string& name) const -> std::optional<value_t> {
        auto it = m_constants.find(name);
        return it != m_constants.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    auto get_variable(const std::string& name) const -> std::optional<value_t> {
        auto it = m_variables.find(name);
        return it != m_variables.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    // Get any symbol as value
    auto get_symbol(const std::string& name) const -> std::optional<value_t>;
    
    // List all exports
    auto get_exported_symbols() const -> std::vector<std::string> {
        return std::vector<std::string>(m_exports.begin(), m_exports.end());
    }
    
    auto get_all_symbols() const -> std::vector<std::string> {
        std::vector<std::string> symbols;
        for (const auto& [name, _] : m_functions) symbols.push_back(name);
        for (const auto& [name, _] : m_classes) symbols.push_back(name);
        for (const auto& [name, _] : m_constants) symbols.push_back(name);
        for (const auto& [name, _] : m_variables) symbols.push_back(name);
        return symbols;
    }
    
    // Get all symbols as values (for import *)
    auto get_all_symbol_values() const -> std::map<std::string, value_t>;
    auto get_exported_symbol_values() const -> std::map<std::string, value_t>;
    
    // Module initialization - called when module is first loaded
    virtual auto initialize() -> void_result_t { 
        return void_result_t::success(); 
    }
    
    // Module cleanup - called when module is unloaded
    virtual auto finalize() -> void {
        // Default: no cleanup needed
    }
    
    // Create Zephyr module object from this native module
    auto create_module_object() -> value_result_t;
    
    // Module metadata
    auto set_name(const std::string& name) -> void { m_name = name; }
    auto get_name() const -> std::string { return m_name; }
    
    auto set_version(const std::string& version) -> void { m_version = version; }
    auto get_version() const -> std::string { return m_version; }
    
    auto set_description(const std::string& description) -> void { m_description = description; }
    auto get_description() const -> std::string { return m_description; }
    
    auto set_author(const std::string& author) -> void { m_author = author; }
    auto get_author() const -> std::string { return m_author; }
    
    // Documentation
    auto set_function_doc(const std::string& func_name, const std::string& doc) -> void {
        m_function_docs[func_name] = doc;
    }
    
    auto get_function_doc(const std::string& func_name) const -> std::string {
        auto it = m_function_docs.find(func_name);
        return it != m_function_docs.end() ? it->second : "";
    }

protected:
    std::string m_name;
    std::string m_version;
    std::string m_description;
    std::string m_author;
    
    std::map<std::string, native_function_t> m_functions;
    std::map<std::string, std::shared_ptr<native_class_t>> m_classes;
    std::map<std::string, value_t> m_constants;
    std::map<std::string, value_t> m_variables;
    std::map<std::string, std::string> m_function_docs;
    
    std::set<std::string> m_exports; // Track which symbols are public
};

// Template helper for C++ class wrapping
template<typename T>
class cpp_class_wrapper_t : public native_class_t {
public:
    auto create_instance(const std::vector<value_t>& args) -> value_result_t override {
        try {
            if (m_constructor) {
                return m_constructor(args);
            } else {
                // Default constructor - this would need to be specialized
                // for different constructor signatures in real implementation
                auto instance = std::make_shared<T>();
                return value_result_t::success(wrap_cpp_object(instance));
            }
        } catch (const std::exception& e) {
            return value_result_t::error(create_plugin_error(
                "Failed to create instance: " + std::string(e.what()),
                get_name()
            ));
        }
    }
    
private:
    // This would be implemented to create a Zephyr object that wraps the C++ object
    auto wrap_cpp_object(std::shared_ptr<T> obj) -> value_t;
};

// Factory functions for creating native classes and modules
template<typename T>
auto make_native_class(const std::string& name = "") -> std::shared_ptr<native_class_t> {
    auto cls = std::make_shared<cpp_class_wrapper_t<T>>();
    if (!name.empty()) {
        cls->set_name(name);
    }
    return cls;
}

auto make_native_module(const std::string& name = "") -> std::shared_ptr<native_module_t>;

} // namespace zephyr::api