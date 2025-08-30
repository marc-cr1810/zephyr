#include "objects/object.hpp"

namespace zephyr
{

auto object_t::to_string() const -> std::string
{
    return std::string("<object>");
}

// Binary operations (will be delegated to type_t)
auto object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object does not support addition operation");
}

auto object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object does not support subtraction operation");
}

auto object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object does not support multiplication operation");
}

auto object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object does not support division operation");
}

auto object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object does not support modulo operation");
}

auto object_t::power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> // Added for **= operator
{
    throw type_error_t("Object does not support power operation");
}

// Truthiness check
auto object_t::is_truthy() const -> bool
{
    return true; // Default: most objects are truthy
}

// Negation operation
auto object_t::negate() -> std::shared_ptr<object_t>
{
    throw type_error_t("Object does not support negation");
}

// List operations
auto object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object is not subscriptable");
}

auto object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Object does not support item assignment");
}

// Member access operations
auto object_t::get_member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    throw attribute_error_t("Object has no member '" + member_name + "'");
}

auto object_t::set_member(const std::string& member_name, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Object does not support member assignment");
}

// Function call operations
auto object_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object is not callable");
}

auto object_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    throw attribute_error_t("Object has no method '" + method_name + "'");
}

// Container operations
auto object_t::contains(std::shared_ptr<object_t> item) -> bool
{
    throw type_error_t("Object does not support containment check");
}

}
