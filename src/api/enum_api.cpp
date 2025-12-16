#include "zephyr/api/enum_api.hpp"
#include "zephyr/objects/enum_object.hpp"
#include "zephyr/errors.hpp"
#include "zephyr/runtime_error.hpp"
#include "zephyr/api/native_function.hpp"
#include <stdexcept>

namespace zephyr::api {

// =============================================================================
// enum_variant_api_t Implementation
// =============================================================================

enum_variant_api_t::enum_variant_api_t(std::shared_ptr<enum_variant_object_t> variant)
    : m_variant(std::move(variant))
{
    if (!m_variant) {
        throw std::invalid_argument("enum_variant_api_t: variant cannot be null");
    }
}

auto enum_variant_api_t::get_enum_name() const -> std::string
{
    return m_variant->get_enum_name();
}

auto enum_variant_api_t::get_variant_name() const -> std::string
{
    return m_variant->get_variant_name();
}

auto enum_variant_api_t::get_variant_index() const -> int
{
    return m_variant->get_variant_index();
}

auto enum_variant_api_t::has_data() const -> bool
{
    return m_variant->has_data();
}

auto enum_variant_api_t::get_data_count() const -> size_t
{
    return m_variant->get_data_count();
}

auto enum_variant_api_t::get_data_value(size_t index) const -> std::optional<value_t>
{
    try {
        return m_variant->get_data_item(index);
    } catch (const runtime_error_with_location_t&) {
        return std::nullopt;
    }
}

auto enum_variant_api_t::get_all_data() const -> std::vector<value_t>
{
    std::vector<value_t> result;
    const auto& data = m_variant->get_data();
    result.reserve(data.size());
    
    for (const auto& item : data) {
        result.push_back(item);
    }
    
    return result;
}

auto enum_variant_api_t::matches(const std::string& enum_name, const std::string& variant_name) const -> bool
{
    return m_variant->matches_pattern(enum_name, variant_name);
}

auto enum_variant_api_t::equals(const enum_variant_api_t& other) const -> bool
{
    return m_variant->equals(other.m_variant);
}

auto enum_variant_api_t::equals(std::shared_ptr<enum_variant_object_t> other) const -> bool
{
    return m_variant->equals(other);
}

auto enum_variant_api_t::to_string() const -> std::string
{
    return m_variant->to_string();
}

auto enum_variant_api_t::to_zephyr() const -> value_t
{
    return m_variant;
}

auto enum_variant_api_t::get_internal() const -> std::shared_ptr<enum_variant_object_t>
{
    return m_variant;
}

// =============================================================================
// enum_api_t Implementation
// =============================================================================

enum_api_t::enum_api_t(std::shared_ptr<enum_object_t> enum_obj)
    : m_enum(std::move(enum_obj))
{
    if (!m_enum) {
        throw std::invalid_argument("enum_api_t: enum object cannot be null");
    }
}

auto enum_api_t::get_name() const -> std::string
{
    return m_enum->get_enum_name();
}

auto enum_api_t::get_variant_names() const -> std::vector<std::string>
{
    return m_enum->get_variant_names();
}

auto enum_api_t::get_variant_count() const -> size_t
{
    return m_enum->get_variant_count();
}

auto enum_api_t::has_variant(const std::string& name) const -> bool
{
    return m_enum->has_variant(name);
}

auto enum_api_t::create_variant(const std::string& variant_name) -> enum_variant_api_t
{
    try {
        auto variant = m_enum->create_variant(variant_name);
        return enum_variant_api_t(variant);
    } catch (const runtime_error_with_location_t& e) {
        throw std::runtime_error(e.what());
    }
}

auto enum_api_t::create_variant_with_data(const std::string& variant_name, 
                                         const std::vector<value_t>& data) -> enum_variant_api_t
{
    try {
        auto variant = m_enum->create_variant(variant_name, data);
        return enum_variant_api_t(variant);
    } catch (const runtime_error_with_location_t& e) {
        throw std::runtime_error(e.what());
    }
}

auto enum_api_t::get_variant_constructor(const std::string& variant_name) -> value_t
{
    try {
        return m_enum->member(variant_name);
    } catch (const runtime_error_with_location_t& e) {
        throw std::runtime_error(e.what());
    }
}

auto enum_api_t::has_method(const std::string& method_name) const -> bool
{
    return m_enum->has_method(method_name);
}

auto enum_api_t::call_method(const std::string& method_name, const std::vector<value_t>& args) -> value_result_t
{
    try {
        auto method = m_enum->get_method(method_name);
        if (!method) {
            return value_result_t::error("Method '" + method_name + "' not found");
        }
        
        // TODO: Implement method calling with proper context
        return value_result_t::error("Enum method calling not yet implemented");
    } catch (const std::exception& e) {
        return value_result_t::error(e.what());
    }
}

auto enum_api_t::to_string() const -> std::string
{
    return m_enum->to_string();
}

auto enum_api_t::to_zephyr() const -> value_t
{
    return m_enum;
}

auto enum_api_t::get_internal() const -> std::shared_ptr<enum_object_t>
{
    return m_enum;
}

// =============================================================================
// enum_builder_t Implementation
// =============================================================================

enum_builder_t::enum_builder_t(const std::string& enum_name)
    : m_enum_name(enum_name)
{
    if (enum_name.empty()) {
        throw std::invalid_argument("enum_builder_t: enum name cannot be empty");
    }
}

auto enum_builder_t::add_variant(const std::string& variant_name) -> enum_builder_t&
{
    if (variant_name.empty()) {
        throw std::invalid_argument("Variant name cannot be empty");
    }
    
    m_simple_variants.emplace_back(variant_name, std::vector<std::string>{});
    return *this;
}

auto enum_builder_t::add_variant(const std::string& variant_name, 
                                const std::vector<std::string>& parameter_names,
                                const std::vector<std::string>& parameter_types) -> enum_builder_t&
{
    if (variant_name.empty()) {
        throw std::invalid_argument("Variant name cannot be empty");
    }
    
    if (!parameter_types.empty() && parameter_types.size() != parameter_names.size()) {
        throw std::invalid_argument("Parameter names and types count mismatch");
    }
    
    m_typed_variants.emplace_back(variant_name, parameter_names, parameter_types);
    return *this;
}

auto enum_builder_t::add_variant(const std::string& variant_name,
                                const std::string& parameter_name,
                                const std::string& parameter_type) -> enum_builder_t&
{
    std::vector<std::string> param_names = {parameter_name};
    std::vector<std::string> param_types;
    
    if (!parameter_type.empty()) {
        param_types = {parameter_type};
    }
    
    return add_variant(variant_name, param_names, param_types);
}

auto enum_builder_t::add_method(const std::string& method_name, const native_function_t& method) -> enum_builder_t&
{
    if (method_name.empty()) {
        throw std::invalid_argument("Method name cannot be empty");
    }
    
    m_methods[method_name] = std::move(method);
    return *this;
}

auto enum_builder_t::set_internal(bool is_internal) -> enum_builder_t&
{
    m_is_internal = is_internal;
    return *this;
}

auto enum_builder_t::build() -> enum_api_t
{
    auto enum_obj = std::make_shared<enum_object_t>(m_enum_name);
    
    // Add simple variants
    for (const auto& [variant_name, _] : m_simple_variants) {
        enum_obj->add_variant(variant_name);
    }
    
    // Add typed variants
    for (const auto& [variant_name, param_names, param_types] : m_typed_variants) {
        enum_obj->add_variant(variant_name, param_names, param_types);
    }
    
    // TODO: Add methods to enum object
    // This requires converting native_function_t to function_definition_t
    // For now, we'll skip this functionality
    
    return enum_api_t(enum_obj);
}

auto enum_builder_t::build_zephyr() -> value_t
{
    return build().to_zephyr();
}

auto enum_builder_t::get_name() const -> const std::string&
{
    return m_enum_name;
}

// =============================================================================
// variant_pattern_t Implementation
// =============================================================================

variant_pattern_t::variant_pattern_t(std::string enum_name, std::string variant_name, std::vector<std::string> variable_names)
    : m_enum_name(std::move(enum_name))
    , m_variant_name(std::move(variant_name))
    , m_variable_names(std::move(variable_names))
    , m_has_data_extraction(!m_variable_names.empty())
{
}

auto variant_pattern_t::simple(const std::string& enum_name, const std::string& variant_name) -> variant_pattern_t
{
    return variant_pattern_t(enum_name, variant_name, {});
}

auto variant_pattern_t::with_data(const std::string& enum_name, const std::string& variant_name,
                                 const std::vector<std::string>& variable_names) -> variant_pattern_t
{
    return variant_pattern_t(enum_name, variant_name, variable_names);
}

auto variant_pattern_t::matches(const enum_variant_api_t& variant) const -> bool
{
    return variant.matches(m_enum_name, m_variant_name);
}

auto variant_pattern_t::matches(std::shared_ptr<enum_variant_object_t> variant) const -> bool
{
    return variant->matches_pattern(m_enum_name, m_variant_name);
}

auto variant_pattern_t::extract_data(const enum_variant_api_t& variant) const -> std::map<std::string, value_t>
{
    return extract_data(variant.get_internal());
}

auto variant_pattern_t::extract_data(std::shared_ptr<enum_variant_object_t> variant) const -> std::map<std::string, value_t>
{
    try {
        return variant->extract_data_for_pattern(m_variable_names);
    } catch (const runtime_error_with_location_t& e) {
        throw std::runtime_error(e.what());
    }
}

auto variant_pattern_t::get_enum_name() const -> const std::string&
{
    return m_enum_name;
}

auto variant_pattern_t::get_variant_name() const -> const std::string&
{
    return m_variant_name;
}

auto variant_pattern_t::has_data_extraction() const -> bool
{
    return m_has_data_extraction;
}

auto variant_pattern_t::get_variable_names() const -> const std::vector<std::string>&
{
    return m_variable_names;
}

// =============================================================================
// Utility Functions Implementation
// =============================================================================

auto is_enum_variant(const value_t& value) -> bool
{
    return std::dynamic_pointer_cast<enum_variant_object_t>(value) != nullptr;
}

auto is_enum(const value_t& value) -> bool
{
    return std::dynamic_pointer_cast<enum_object_t>(value) != nullptr;
}

auto as_enum_variant(const value_t& value) -> enum_variant_api_t
{
    auto variant = std::dynamic_pointer_cast<enum_variant_object_t>(value);
    if (!variant) {
        throw std::runtime_error("Value is not an enum variant");
    }
    return enum_variant_api_t(variant);
}

auto as_enum(const value_t& value) -> enum_api_t
{
    auto enum_obj = std::dynamic_pointer_cast<enum_object_t>(value);
    if (!enum_obj) {
        throw std::runtime_error("Value is not an enum");
    }
    return enum_api_t(enum_obj);
}

// =============================================================================
// Common Pattern Implementations
// =============================================================================

namespace patterns {

auto some(const std::string& var_name) -> variant_pattern_t
{
    return variant_pattern_t::with_data("Option", "SOME", {var_name});
}

auto none() -> variant_pattern_t
{
    return variant_pattern_t::simple("Option", "NONE");
}

auto ok(const std::string& var_name) -> variant_pattern_t
{
    return variant_pattern_t::with_data("Result", "OK", {var_name});
}

auto error(const std::string& var_name) -> variant_pattern_t
{
    return variant_pattern_t::with_data("Result", "ERROR", {var_name});
}

} // namespace patterns

} // namespace zephyr::api