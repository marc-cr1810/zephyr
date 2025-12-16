#pragma once

#include "zephyr/objects/enum_object.hpp"
#include "zephyr/api/result.hpp"
#include "zephyr/api/type_converter.hpp"
#include "zephyr/api/native_function.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>

namespace zephyr::api {

// =============================================================================
// Enum API Classes
// =============================================================================

// Forward declarations
class enum_builder_t;
class variant_pattern_t;

// =============================================================================
// Enum Variant API
// =============================================================================

class enum_variant_api_t {
public:
    explicit enum_variant_api_t(std::shared_ptr<enum_variant_object_t> variant);
    
    // Variant information
    auto get_enum_name() const -> std::string;
    auto get_variant_name() const -> std::string;
    auto get_variant_index() const -> int;
    
    // Data access
    auto has_data() const -> bool;
    auto get_data_count() const -> size_t;
    
    template<typename T>
    auto get_data(size_t index) const -> std::optional<T> {
        if (index >= get_data_count()) {
            return std::nullopt;
        }
        auto data_obj = m_variant->get_data_item(index);
        return try_from_zephyr<T>(data_obj);
    }
    
    auto get_data_value(size_t index) const -> std::optional<value_t>;
    auto get_all_data() const -> std::vector<value_t>;
    
    // Pattern matching
    auto matches(const std::string& enum_name, const std::string& variant_name) const -> bool;
    
    // Equality comparison
    auto equals(const enum_variant_api_t& other) const -> bool;
    auto equals(std::shared_ptr<enum_variant_object_t> other) const -> bool;
    
    // Conversion
    auto to_string() const -> std::string;
    auto to_zephyr() const -> value_t;
    
    // Internal access
    auto get_internal() const -> std::shared_ptr<enum_variant_object_t>;

private:
    std::shared_ptr<enum_variant_object_t> m_variant;
};

// =============================================================================
// Enum Definition API
// =============================================================================

class enum_api_t {
public:
    explicit enum_api_t(std::shared_ptr<enum_object_t> enum_obj);
    
    // Enum information
    auto get_name() const -> std::string;
    auto get_variant_names() const -> std::vector<std::string>;
    auto get_variant_count() const -> size_t;
    auto has_variant(const std::string& name) const -> bool;
    
    // Variant creation
    auto create_variant(const std::string& variant_name) -> enum_variant_api_t;
    
    template<typename... Args>
    auto create_variant(const std::string& variant_name, Args&&... args) -> enum_variant_api_t {
        std::vector<value_t> zephyr_args;
        (zephyr_args.push_back(to_zephyr(std::forward<Args>(args))), ...);
        return create_variant_with_data(variant_name, zephyr_args);
    }
    
    auto create_variant_with_data(const std::string& variant_name, 
                                 const std::vector<value_t>& data) -> enum_variant_api_t;
    
    // Variant access (returns constructor functions)
    auto get_variant_constructor(const std::string& variant_name) -> value_t;
    
    // Method access
    auto has_method(const std::string& method_name) const -> bool;
    auto call_method(const std::string& method_name, const std::vector<value_t>& args) -> value_result_t;
    
    template<typename... Args>
    auto call_method(const std::string& method_name, Args&&... args) -> value_result_t {
        std::vector<value_t> zephyr_args;
        (zephyr_args.push_back(to_zephyr(std::forward<Args>(args))), ...);
        return call_method(method_name, zephyr_args);
    }
    
    // Conversion
    auto to_string() const -> std::string;
    auto to_zephyr() const -> value_t;
    
    // Internal access
    auto get_internal() const -> std::shared_ptr<enum_object_t>;

private:
    std::shared_ptr<enum_object_t> m_enum;
};

// =============================================================================
// Enum Builder API
// =============================================================================

class enum_builder_t {
public:
    explicit enum_builder_t(const std::string& enum_name);
    
    // Add simple variant (no data)
    auto add_variant(const std::string& variant_name) -> enum_builder_t&;
    
    // Add variant with typed parameters
    auto add_variant(const std::string& variant_name, 
                    const std::vector<std::string>& parameter_names,
                    const std::vector<std::string>& parameter_types = {}) -> enum_builder_t&;
    
    // Convenience method for single parameter variant
    auto add_variant(const std::string& variant_name,
                    const std::string& parameter_name,
                    const std::string& parameter_type = "") -> enum_builder_t&;
    
    // Add method to enum
    auto add_method(const std::string& method_name, const native_function_t& method) -> enum_builder_t&;
    
    template<typename F>
    auto add_method(const std::string& method_name, F&& func) -> enum_builder_t& {
        return add_method(method_name, make_native_function(std::forward<F>(func), method_name));
    }
    
    // Set internal visibility
    auto set_internal(bool is_internal = true) -> enum_builder_t&;
    
    // Build the enum
    auto build() -> enum_api_t;
    auto build_zephyr() -> value_t;
    
    // Get name for debugging
    auto get_name() const -> const std::string&;

private:
    std::string m_enum_name;
    std::vector<std::pair<std::string, std::vector<std::string>>> m_simple_variants;
    std::vector<std::tuple<std::string, std::vector<std::string>, std::vector<std::string>>> m_typed_variants;
    std::map<std::string, native_function_t> m_methods;
    bool m_is_internal = false;
};

// =============================================================================
// Pattern Matching API
// =============================================================================

class variant_pattern_t {
public:
    // Create pattern for simple variant
    static auto simple(const std::string& enum_name, const std::string& variant_name) -> variant_pattern_t;
    
    // Create pattern for variant with data extraction
    static auto with_data(const std::string& enum_name, const std::string& variant_name,
                         const std::vector<std::string>& variable_names) -> variant_pattern_t;
    
    // Check if variant matches pattern
    auto matches(const enum_variant_api_t& variant) const -> bool;
    auto matches(std::shared_ptr<enum_variant_object_t> variant) const -> bool;
    
    // Extract data if pattern matches
    auto extract_data(const enum_variant_api_t& variant) const -> std::map<std::string, value_t>;
    auto extract_data(std::shared_ptr<enum_variant_object_t> variant) const -> std::map<std::string, value_t>;
    
    // Pattern information
    auto get_enum_name() const -> const std::string&;
    auto get_variant_name() const -> const std::string&;
    auto has_data_extraction() const -> bool;
    auto get_variable_names() const -> const std::vector<std::string>&;

private:
    variant_pattern_t(std::string enum_name, std::string variant_name, std::vector<std::string> variable_names);
    
    std::string m_enum_name;
    std::string m_variant_name;
    std::vector<std::string> m_variable_names;
    bool m_has_data_extraction;
};

// =============================================================================
// Pattern Matching Helper Functions
// =============================================================================

// Match against multiple patterns
template<typename Visitor>
auto match_variant(const enum_variant_api_t& variant, Visitor&& visitor) -> decltype(auto) {
    return visitor(variant);
}

template<typename PatternHandler, typename... RestHandlers>
auto match_variant(const enum_variant_api_t& variant, 
                  const variant_pattern_t& pattern, 
                  PatternHandler&& handler,
                  RestHandlers&&... rest) -> decltype(auto) {
    if (pattern.matches(variant)) {
        auto data = pattern.extract_data(variant);
        return handler(data);
    }
    
    if constexpr (sizeof...(rest) > 0) {
        return match_variant(variant, std::forward<RestHandlers>(rest)...);
    } else {
        throw std::runtime_error("No matching pattern for variant: " + variant.to_string());
    }
}

// =============================================================================
// Type Converter Extensions for Enums
// =============================================================================

// Convert enum variant to Zephyr value
template<>
inline auto to_zephyr<enum_variant_api_t>(const enum_variant_api_t& variant) -> value_t {
    return variant.to_zephyr();
}

// Convert enum to Zephyr value
template<>
inline auto to_zephyr<enum_api_t>(const enum_api_t& enum_obj) -> value_t {
    return enum_obj.to_zephyr();
}

// Convert from Zephyr value to enum variant
template<>
inline auto try_from_zephyr<enum_variant_api_t>(const value_t& value) -> std::optional<enum_variant_api_t> {
    auto variant_obj = std::dynamic_pointer_cast<enum_variant_object_t>(value);
    if (variant_obj) {
        return enum_variant_api_t(variant_obj);
    }
    return std::nullopt;
}

// Convert from Zephyr value to enum
template<>
inline auto try_from_zephyr<enum_api_t>(const value_t& value) -> std::optional<enum_api_t> {
    auto enum_obj = std::dynamic_pointer_cast<enum_object_t>(value);
    if (enum_obj) {
        return enum_api_t(enum_obj);
    }
    return std::nullopt;
}

// =============================================================================
// Utility Functions
// =============================================================================

// Check if value is an enum variant
auto is_enum_variant(const value_t& value) -> bool;

// Check if value is an enum type
auto is_enum(const value_t& value) -> bool;

// Get enum variant from value (throws if not a variant)
auto as_enum_variant(const value_t& value) -> enum_variant_api_t;

// Get enum from value (throws if not an enum)
auto as_enum(const value_t& value) -> enum_api_t;

// Create common enum patterns
namespace patterns {
    // Option<T> pattern
    auto some(const std::string& var_name = "value") -> variant_pattern_t;
    auto none() -> variant_pattern_t;
    
    // Result<T, E> pattern  
    auto ok(const std::string& var_name = "value") -> variant_pattern_t;
    auto error(const std::string& var_name = "error") -> variant_pattern_t;
}

} // namespace zephyr::api