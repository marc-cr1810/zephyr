#include "zephyr/types/enum_type.hpp"
#include "zephyr/objects/enum_object.hpp"
#include "zephyr/errors.hpp"

namespace zephyr
{

// ============================================================================
// enum_type_t Implementation
// ============================================================================

std::shared_ptr<enum_type_t> enum_type_t::s_instance = nullptr;

auto enum_type_t::name() const -> std::string
{
    return "enum";
}

auto enum_type_t::is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
{
    auto enum_obj = std::dynamic_pointer_cast<enum_object_t>(self);
    if (!enum_obj) {
        return false;
    }
    
    return enum_obj->has_variant(name) || enum_obj->has_method(name);
}

auto enum_type_t::member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    auto enum_obj = std::dynamic_pointer_cast<enum_object_t>(self);
    if (!enum_obj) {
        throw type_error_t("Cannot access member on non-enum object");
    }
    
    return enum_obj->member(name);
}

auto enum_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    auto self_enum = std::dynamic_pointer_cast<enum_object_t>(self);
    auto other_enum = std::dynamic_pointer_cast<enum_object_t>(other);
    
    if (!self_enum || !other_enum) {
        return false;
    }
    
    return self_enum->get_enum_name() == other_enum->get_enum_name();
}

auto enum_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Enum types are always truthy
}

auto enum_type_t::get_instance() -> std::shared_ptr<enum_type_t>
{
    if (!s_instance) {
        s_instance = std::shared_ptr<enum_type_t>(new enum_type_t());
    }
    return s_instance;
}

// ============================================================================
// enum_variant_type_t Implementation
// ============================================================================

std::shared_ptr<enum_variant_type_t> enum_variant_type_t::s_instance = nullptr;

auto enum_variant_type_t::name() const -> std::string
{
    return "enum_variant";
}

auto enum_variant_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    auto self_variant = std::dynamic_pointer_cast<enum_variant_object_t>(self);
    if (!self_variant) {
        return false;
    }
    
    return self_variant->equals(other);
}

auto enum_variant_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Enum variants are always truthy
}

auto enum_variant_type_t::get_instance() -> std::shared_ptr<enum_variant_type_t>
{
    if (!s_instance) {
        s_instance = std::shared_ptr<enum_variant_type_t>(new enum_variant_type_t());
    }
    return s_instance;
}

// ============================================================================
// enum_variant_constructor_type_t Implementation
// ============================================================================

std::shared_ptr<enum_variant_constructor_type_t> enum_variant_constructor_type_t::s_instance = nullptr;

auto enum_variant_constructor_type_t::name() const -> std::string
{
    return "enum_variant_constructor";
}

auto enum_variant_constructor_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    auto self_constructor = std::dynamic_pointer_cast<enum_variant_constructor_t>(self);
    auto other_constructor = std::dynamic_pointer_cast<enum_variant_constructor_t>(other);
    
    if (!self_constructor || !other_constructor) {
        return false;
    }
    
    return self_constructor->get_enum_name() == other_constructor->get_enum_name() &&
           self_constructor->get_variant_name() == other_constructor->get_variant_name();
}

auto enum_variant_constructor_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Enum variant constructors are always truthy
}

auto enum_variant_constructor_type_t::get_instance() -> std::shared_ptr<enum_variant_constructor_type_t>
{
    if (!s_instance) {
        s_instance = std::shared_ptr<enum_variant_constructor_type_t>(new enum_variant_constructor_type_t());
    }
    return s_instance;
}

// ============================================================================
// bound_enum_method_type_t Implementation
// ============================================================================

std::shared_ptr<bound_enum_method_type_t> bound_enum_method_type_t::s_instance = nullptr;

auto bound_enum_method_type_t::name() const -> std::string
{
    return "bound_enum_method";
}

auto bound_enum_method_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    auto self_method = std::dynamic_pointer_cast<bound_enum_method_t>(self);
    auto other_method = std::dynamic_pointer_cast<bound_enum_method_t>(other);
    
    if (!self_method || !other_method) {
        return false;
    }
    
    return self_method->get_enum_name() == other_method->get_enum_name() &&
           self_method->get_method_name() == other_method->get_method_name();
}

auto bound_enum_method_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Bound enum methods are always truthy
}

auto bound_enum_method_type_t::get_instance() -> std::shared_ptr<bound_enum_method_type_t>
{
    if (!s_instance) {
        s_instance = std::shared_ptr<bound_enum_method_type_t>(new bound_enum_method_type_t());
    }
    return s_instance;
}

} // namespace zephyr