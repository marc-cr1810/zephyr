#include "zephyr/objects/enum_object.hpp"
#include "zephyr/types/types.hpp"
#include "zephyr/errors.hpp"
#include "zephyr/objects/builtin_function_object.hpp"
#include <sstream>

namespace zephyr
{

// ============================================================================
// enum_variant_object_t Implementation
// ============================================================================

enum_variant_object_t::enum_variant_object_t(const std::string& enum_name, const std::string& variant_name, int variant_index,
                                           std::weak_ptr<enum_object_t> parent_enum)
    : m_enum_name(enum_name)
    , m_variant_name(variant_name)
    , m_variant_index(variant_index)
    , m_has_data(false)
    , m_parent_enum(parent_enum)
{
}

enum_variant_object_t::enum_variant_object_t(const std::string& enum_name, const std::string& variant_name, int variant_index,
                                           std::vector<std::shared_ptr<object_t>> data,
                                           std::weak_ptr<enum_object_t> parent_enum)
    : m_enum_name(enum_name)
    , m_variant_name(variant_name)
    , m_variant_index(variant_index)
    , m_data(std::move(data))
    , m_has_data(true)
    , m_parent_enum(parent_enum)
{
}

auto enum_variant_object_t::type() const -> std::shared_ptr<type_t>
{
    return enum_variant_type_t::get_instance();
}

auto enum_variant_object_t::to_string() const -> std::string
{
    std::ostringstream oss;
    oss << m_enum_name << "." << m_variant_name;
    
    if (m_has_data && !m_data.empty()) {
        oss << "(";
        for (size_t i = 0; i < m_data.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << m_data[i]->to_string();
        }
        oss << ")";
    }
    
    return oss.str();
}

auto enum_variant_object_t::is_truthy() const -> bool
{
    return true; // Enum variants are always truthy
}

auto enum_variant_object_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    throw type_error_t("Enum variant '" + m_enum_name + "." + m_variant_name + "' is not callable");
}

auto enum_variant_object_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Get parent enum to access methods
    auto parent_enum = m_parent_enum.lock();
    if (!parent_enum) {
        throw type_error_t("Parent enum no longer exists");
    }
    
    // Check if method exists on the enum
    if (!parent_enum->has_method(method_name)) {
        throw name_error_t("Enum '" + m_enum_name + "' has no method '" + method_name + "'");
    }
    
    // Execute the method with 'this' bound to this variant
    auto method = parent_enum->get_method(method_name);
    if (!method) {
        throw name_error_t("Enum '" + m_enum_name + "' has no method '" + method_name + "'");
    }

    // Enhanced pattern matching is now built into the interpreter's switch statement handler
    // Method execution will use the enhanced switch support automatically when switch statements
    // with enum patterns are encountered during method body execution
    throw type_error_t("Enum method execution requires proper interpreter context integration - enhanced pattern matching is available in switch statements");
}

auto enum_variant_object_t::get_enum_name() const -> const std::string&
{
    return m_enum_name;
}

auto enum_variant_object_t::get_variant_name() const -> const std::string&
{
    return m_variant_name;
}

auto enum_variant_object_t::get_variant_index() const -> int
{
    return m_variant_index;
}

auto enum_variant_object_t::get_data() const -> const std::vector<std::shared_ptr<object_t>>&
{
    return m_data;
}

auto enum_variant_object_t::has_data() const -> bool
{
    return m_has_data;
}

auto enum_variant_object_t::get_data_count() const -> size_t
{
    return m_data.size();
}

auto enum_variant_object_t::get_data_item(size_t index) const -> std::shared_ptr<object_t>
{
    if (index >= m_data.size()) {
        throw value_error_t("Index " + std::to_string(index) + " out of bounds for variant data");
    }
    return m_data[index];
}

auto enum_variant_object_t::matches_pattern(const std::string& pattern_enum, const std::string& pattern_variant) const -> bool
{
    return m_enum_name == pattern_enum && m_variant_name == pattern_variant;
}

auto enum_variant_object_t::extract_data_for_pattern(const std::vector<std::string>& variable_names) const -> std::map<std::string, std::shared_ptr<object_t>>
{
    std::map<std::string, std::shared_ptr<object_t>> result;
    
    if (variable_names.size() > m_data.size()) {
        throw value_error_t("Pattern expects " + std::to_string(variable_names.size()) + 
                             " variables but variant has " + std::to_string(m_data.size()) + " data items");
    }
    
    for (size_t i = 0; i < variable_names.size(); ++i) {
        result[variable_names[i]] = m_data[i];
    }
    
    return result;
}

auto enum_variant_object_t::get_parent_enum() const -> std::shared_ptr<enum_object_t>
{
    return m_parent_enum.lock();
}

auto enum_variant_object_t::equals(std::shared_ptr<object_t> other) const -> bool
{
    auto other_variant = std::dynamic_pointer_cast<enum_variant_object_t>(other);
    if (!other_variant) {
        return false;
    }
    
    // Check enum and variant name match
    if (m_enum_name != other_variant->m_enum_name || m_variant_name != other_variant->m_variant_name) {
        return false;
    }
    
    // Check data equality
    if (m_data.size() != other_variant->m_data.size()) {
        return false;
    }
    
    for (size_t i = 0; i < m_data.size(); ++i) {
        // TODO: Implement proper object equality comparison
        if (m_data[i]->to_string() != other_variant->m_data[i]->to_string()) {
            return false;
        }
    }
    
    return true;
}

// ============================================================================
// enum_object_t Implementation
// ============================================================================

enum_object_t::enum_object_t(const std::string& enum_name)
    : m_enum_name(enum_name)
    , m_next_variant_index(0)
{
}

auto enum_object_t::type() const -> std::shared_ptr<type_t>
{
    return enum_type_t::get_instance();
}

auto enum_object_t::to_string() const -> std::string
{
    return "enum " + m_enum_name;
}

auto enum_object_t::is_truthy() const -> bool
{
    return true;
}

auto enum_object_t::member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    // Check for variant access
    if (has_variant(member_name)) {
        return create_variant_constructor(member_name);
    }
    
    // Check for method access
    if (has_method(member_name)) {
        // Return bound method object for static calls
        auto enum_ptr = std::dynamic_pointer_cast<enum_object_t>(shared_from_this());
        return std::make_shared<bound_enum_method_t>(enum_ptr, member_name);
    }
    
    throw name_error_t("Enum '" + m_enum_name + "' has no member '" + member_name + "'");
}

auto enum_object_t::add_variant(const std::string& variant_name) -> void
{
    if (has_variant(variant_name)) {
        throw value_error_t("Variant '" + variant_name + "' already exists in enum '" + m_enum_name + "'");
    }
    
    m_variants.emplace(variant_name, variant_definition_t(variant_name, m_next_variant_index++));
}

auto enum_object_t::add_variant(const std::string& variant_name, const std::vector<std::string>& parameter_names,
                               const std::vector<std::string>& parameter_types) -> void
{
    if (has_variant(variant_name)) {
        throw value_error_t("Variant '" + variant_name + "' already exists in enum '" + m_enum_name + "'");
    }
    
    m_variants.emplace(variant_name, variant_definition_t(variant_name, m_next_variant_index++, parameter_names, parameter_types));
}

auto enum_object_t::add_method(const std::string& method_name, std::shared_ptr<function_definition_t> method) -> void
{
    m_methods[method_name] = method;
}

auto enum_object_t::get_enum_name() const -> const std::string&
{
    return m_enum_name;
}

auto enum_object_t::get_variant_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(m_variants.size());
    
    for (const auto& [name, _] : m_variants) {
        names.push_back(name);
    }
    
    return names;
}

auto enum_object_t::get_variant_definition(const std::string& variant_name) const -> const variant_definition_t*
{
    auto it = m_variants.find(variant_name);
    if (it != m_variants.end()) {
        return &it->second;
    }
    return nullptr;
}

auto enum_object_t::has_variant(const std::string& variant_name) const -> bool
{
    return m_variants.find(variant_name) != m_variants.end();
}

auto enum_object_t::get_variant_count() const -> size_t
{
    return m_variants.size();
}

auto enum_object_t::create_variant(const std::string& variant_name) -> std::shared_ptr<enum_variant_object_t>
{
    const auto* variant_def = get_variant_definition(variant_name);
    if (!variant_def) {
        throw name_error_t("Unknown variant '" + variant_name + "' in enum '" + m_enum_name + "'");
    }
    
    if (variant_def->has_parameters) {
        throw type_error_t("Variant '" + variant_name + "' requires " + 
                             std::to_string(variant_def->parameter_names.size()) + " parameters");
    }
    
    auto enum_ptr = std::dynamic_pointer_cast<enum_object_t>(shared_from_this());
    return std::make_shared<enum_variant_object_t>(m_enum_name, variant_name, variant_def->index, enum_ptr);
}

auto enum_object_t::create_variant(const std::string& variant_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<enum_variant_object_t>
{
    const auto* variant_def = get_variant_definition(variant_name);
    if (!variant_def) {
        throw name_error_t("Unknown variant '" + variant_name + "' in enum '" + m_enum_name + "'");
    }
    
    if (variant_def->has_parameters && args.size() != variant_def->parameter_names.size()) {
        throw type_error_t("Variant '" + variant_name + "' expects " + 
                             std::to_string(variant_def->parameter_names.size()) + 
                             " arguments, got " + std::to_string(args.size()));
    }
    
    if (!variant_def->has_parameters && !args.empty()) {
        throw type_error_t("Variant '" + variant_name + "' expects no arguments, got " + 
                             std::to_string(args.size()));
    }
    
    auto enum_ptr = std::dynamic_pointer_cast<enum_object_t>(shared_from_this());
    return std::make_shared<enum_variant_object_t>(m_enum_name, variant_name, variant_def->index, args, enum_ptr);
}

auto enum_object_t::get_method(const std::string& method_name) const -> std::shared_ptr<function_definition_t>
{
    auto it = m_methods.find(method_name);
    if (it != m_methods.end()) {
        return it->second;
    }
    return nullptr;
}

auto enum_object_t::has_method(const std::string& method_name) const -> bool
{
    return m_methods.find(method_name) != m_methods.end();
}

auto enum_object_t::create_variant_constructor(const std::string& variant_name) -> std::shared_ptr<object_t>
{
    auto enum_ptr = std::dynamic_pointer_cast<enum_object_t>(shared_from_this());
    return std::make_shared<enum_variant_constructor_t>(enum_ptr, variant_name);
}

// ============================================================================
// enum_variant_constructor_t Implementation
// ============================================================================

enum_variant_constructor_t::enum_variant_constructor_t(std::shared_ptr<enum_object_t> enum_obj, const std::string& variant_name)
    : m_enum_object(enum_obj)
    , m_variant_name(variant_name)
{
}

auto enum_variant_constructor_t::type() const -> std::shared_ptr<type_t>
{
    return enum_variant_constructor_type_t::get_instance();
}

auto enum_variant_constructor_t::to_string() const -> std::string
{
    if (auto enum_obj = m_enum_object.lock()) {
        return enum_obj->get_enum_name() + "." + m_variant_name;
    }
    return "DetachedVariant." + m_variant_name;
}

auto enum_variant_constructor_t::is_truthy() const -> bool
{
    return true;
}

auto enum_variant_constructor_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    auto enum_obj = m_enum_object.lock();
    if (!enum_obj) {
        throw type_error_t("Enum object has been destroyed");
    }
    
    const auto* variant_def = enum_obj->get_variant_definition(m_variant_name);
    if (!variant_def) {
        throw name_error_t("Variant '" + m_variant_name + "' no longer exists");
    }
    
    // If variant has no parameters, calling it should create a simple variant
    if (!variant_def->has_parameters) {
        if (!args.empty()) {
            throw type_error_t("Variant '" + m_variant_name + "' expects no arguments, got " + 
                                 std::to_string(args.size()));
        }
        return enum_obj->create_variant(m_variant_name);
    }
    
    // Create variant with provided arguments
    return enum_obj->create_variant(m_variant_name, args);
}

auto enum_variant_constructor_t::get_enum_name() const -> std::string
{
    if (auto enum_obj = m_enum_object.lock()) {
        return enum_obj->get_enum_name();
    }
    return "DetachedEnum";
}

auto enum_variant_constructor_t::get_variant_name() const -> std::string
{
    return m_variant_name;
}

// ============================================================================
// bound_enum_method_t Implementation
// ============================================================================

bound_enum_method_t::bound_enum_method_t(std::shared_ptr<enum_object_t> enum_obj, const std::string& method_name)
    : m_enum_object(enum_obj)
    , m_method_name(method_name)
{
}

auto bound_enum_method_t::type() const -> std::shared_ptr<type_t>
{
    return bound_enum_method_type_t::get_instance();
}

auto bound_enum_method_t::to_string() const -> std::string
{
    if (auto enum_obj = m_enum_object.lock()) {
        return "<bound method " + enum_obj->get_enum_name() + "." + m_method_name + ">";
    }
    return "<bound method DetachedEnum." + m_method_name + ">";
}

auto bound_enum_method_t::is_truthy() const -> bool
{
    return true;
}

auto bound_enum_method_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    auto enum_obj = m_enum_object.lock();
    if (!enum_obj) {
        throw type_error_t("Enum object has been destroyed");
    }
    
    // Get the method definition
    auto method_func = enum_obj->get_method(m_method_name);
    if (!method_func) {
        throw name_error_t("Method '" + m_method_name + "' no longer exists");
    }
    
    // DEBUG: This method is being called - the interpreter handler is not working
    throw type_error_t("DEBUG: bound_enum_method_t::call() was reached - interpreter handler failed for method '" + m_method_name + "'");
}

auto bound_enum_method_t::get_enum_name() const -> std::string
{
    if (auto enum_obj = m_enum_object.lock()) {
        return enum_obj->get_enum_name();
    }
    return "DetachedEnum";
}

auto bound_enum_method_t::get_method_name() const -> std::string
{
    return m_method_name;
}

auto bound_enum_method_t::get_enum_object() const -> std::shared_ptr<enum_object_t>
{
    return m_enum_object.lock();
}

} // namespace zephyr