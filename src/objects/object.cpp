#include "objects/object.hpp"
#include "types/type.hpp"
#include "errors.hpp"
#include <sstream>

namespace zephyr
{

auto object_t::to_string() const -> std::string
{
    std::stringstream ss;
    ss << "<object at " << std::hex << shared_from_this().get() << ">";
    return std::string(ss.str());
}

// Binary operations (will be delegated to type_t)
auto object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return type()->add(shared_from_this(), other);
}

auto object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return type()->subtract(shared_from_this(), other);
}

auto object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return type()->multiply(shared_from_this(), other);
}

auto object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return type()->divide(shared_from_this(), other);
}

auto object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return type()->modulo(shared_from_this(), other);
}

auto object_t::power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> // Added for **= operator
{
    return type()->power(shared_from_this(), other);
}

// Truthiness check
auto object_t::is_truthy() const -> bool
{
    return type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

// Negation operation
auto object_t::negate() -> std::shared_ptr<object_t>
{
    return type()->negate(shared_from_this());
}

// List operations
auto object_t::item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    return type()->item(shared_from_this(), index);
}

auto object_t::item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    type()->item(shared_from_this(), index, value);
}

// Member access operations
auto object_t::member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    return type()->member(shared_from_this(), member_name);
}

auto object_t::member(const std::string& member_name, std::shared_ptr<object_t> value) -> void
{
    type()->member(shared_from_this(), member_name, value);
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
    return type()->contains(shared_from_this(), item);
}

}
