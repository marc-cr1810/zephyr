#pragma once

#include "object.hpp"
#include "zephyr/function_overload_resolver.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace zephyr
{

// Forward declarations
class function_definition_t;
class enum_variant_object_t;
class enum_object_t;

// Represents a single enum variant (e.g., Color.RED or Option.SOME(value))
class enum_variant_object_t : public object_t
{
public:
    // Constructor for simple variants (no data)
    enum_variant_object_t(const std::string& enum_name, const std::string& variant_name, int variant_index,
                         std::weak_ptr<enum_object_t> parent_enum = {});
    
    // Constructor for variants with data
    enum_variant_object_t(const std::string& enum_name, const std::string& variant_name, int variant_index,
                         std::vector<std::shared_ptr<object_t>> data,
                         std::weak_ptr<enum_object_t> parent_enum = {});

    // Object interface
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    auto is_truthy() const -> bool override;
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;
    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;

    // Enum variant specific methods
    auto get_enum_name() const -> const std::string&;
    auto get_variant_name() const -> const std::string&;
    auto get_variant_index() const -> int;
    auto get_data() const -> const std::vector<std::shared_ptr<object_t>>&;
    auto has_data() const -> bool;
    auto get_data_count() const -> size_t;
    auto get_data_item(size_t index) const -> std::shared_ptr<object_t>;
    
    // Pattern matching support
    auto matches_pattern(const std::string& pattern_enum, const std::string& pattern_variant) const -> bool;
    auto extract_data_for_pattern(const std::vector<std::string>& variable_names) const -> std::map<std::string, std::shared_ptr<object_t>>;

    // Equality comparison for enum variants
    auto equals(std::shared_ptr<object_t> other) const -> bool;

    // Method support
    auto get_parent_enum() const -> std::shared_ptr<enum_object_t>;

private:
    std::string m_enum_name;
    std::string m_variant_name;
    int m_variant_index;
    std::vector<std::shared_ptr<object_t>> m_data;
    bool m_has_data;
    std::weak_ptr<enum_object_t> m_parent_enum;
};

// Represents an enum type definition (e.g., the Color enum itself)
class enum_object_t : public object_t
{
public:
    // Variant definition structure
    struct variant_definition_t
    {
        std::string name;
        int index;
        std::vector<std::string> parameter_names;
        std::vector<std::string> parameter_types;
        bool has_parameters;
        
        variant_definition_t(const std::string& variant_name, int variant_index)
            : name(variant_name), index(variant_index), has_parameters(false) {}
            
        variant_definition_t(const std::string& variant_name, int variant_index, 
                           const std::vector<std::string>& param_names,
                           const std::vector<std::string>& param_types = {})
            : name(variant_name), index(variant_index), parameter_names(param_names), 
              parameter_types(param_types), has_parameters(true) {}
    };

    explicit enum_object_t(const std::string& enum_name);

    // Object interface
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    auto is_truthy() const -> bool override;
    auto member(const std::string& member_name) -> std::shared_ptr<object_t> override;

    // Enum definition methods
    auto add_variant(const std::string& variant_name) -> void;
    auto add_variant(const std::string& variant_name, const std::vector<std::string>& parameter_names,
                    const std::vector<std::string>& parameter_types = {}) -> void;
    auto add_method(const std::string& method_name, std::shared_ptr<function_definition_t> method) -> void;
    
    // Enum access methods
    auto get_enum_name() const -> const std::string&;
    auto get_variant_names() const -> std::vector<std::string>;
    auto get_variant_definition(const std::string& variant_name) const -> const variant_definition_t*;
    auto has_variant(const std::string& variant_name) const -> bool;
    auto get_variant_count() const -> size_t;
    
    // Create variant instances
    auto create_variant(const std::string& variant_name) -> std::shared_ptr<enum_variant_object_t>;
    auto create_variant(const std::string& variant_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<enum_variant_object_t>;
    
    // Method resolution
    auto get_method(const std::string& method_name) const -> std::shared_ptr<function_definition_t>;
    auto has_method(const std::string& method_name) const -> bool;

private:
    std::string m_enum_name;
    std::map<std::string, variant_definition_t> m_variants;
    std::map<std::string, std::shared_ptr<function_definition_t>> m_methods;
    function_overload_resolver_t m_method_resolver;
    int m_next_variant_index;
    
    // Helper method to create variant constructor functions
    auto create_variant_constructor(const std::string& variant_name) -> std::shared_ptr<object_t>;
};

// Variant constructor function object (callable that creates enum variants)
class enum_variant_constructor_t : public object_t
{
public:
    enum_variant_constructor_t(std::shared_ptr<enum_object_t> enum_obj, const std::string& variant_name);

    // Object interface
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    auto is_truthy() const -> bool override;
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;

    // Variant constructor specific methods
    auto get_enum_name() const -> std::string;
    auto get_variant_name() const -> std::string;

private:
    std::weak_ptr<enum_object_t> m_enum_object;
    std::string m_variant_name;
};

// Bound enum method object (for static method calls on enum types)
class bound_enum_method_t : public object_t
{
public:
    bound_enum_method_t(std::shared_ptr<enum_object_t> enum_obj, const std::string& method_name);

    // Object interface
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    auto is_truthy() const -> bool override;
    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;

    // Bound method specific methods
    auto get_enum_name() const -> std::string;
    auto get_method_name() const -> std::string;
    auto get_enum_object() const -> std::shared_ptr<enum_object_t>;

private:
    std::weak_ptr<enum_object_t> m_enum_object;
    std::string m_method_name;
};

} // namespace zephyr