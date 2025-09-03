#include "objects/object.hpp"
#include "types/type.hpp"
#include "errors.hpp"

namespace zephyr
{

auto object_t::to_string() const -> std::string
{
    return std::string("<object>");
}

// Binary operations (will be delegated to type_t)
auto object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto object_t::power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> // Added for **= operator
{
    return get_type()->power(shared_from_this(), other);
}

// Truthiness check
auto object_t::is_truthy() const -> bool
{
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

// Negation operation
auto object_t::negate() -> std::shared_ptr<object_t>
{
    return get_type()->negate(shared_from_this());
}

// List operations
auto object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    return get_type()->get_item(shared_from_this(), index);
}

auto object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    get_type()->set_item(shared_from_this(), index, value);
}

// Member access operations
auto object_t::get_member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    return get_type()->get_member(shared_from_this(), member_name);
}

auto object_t::set_member(const std::string& member_name, std::shared_ptr<object_t> value) -> void
{
    get_type()->set_member(shared_from_this(), member_name, value);
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
    return get_type()->contains(shared_from_this(), item);
}

}
