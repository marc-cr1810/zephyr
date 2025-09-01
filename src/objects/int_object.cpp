#include "objects/int_object.hpp"
#include "objects/float_object.hpp"
#include "types/int_type.hpp"
#include <string>
#include "errors.hpp"
#include <cmath> // Added for std::pow

namespace zephyr
{

int_object_t::int_object_t(int val)
    : m_value(val)
{
}

auto int_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return int_type_t::get_instance();
}

auto int_object_t::to_string() const -> std::string
{
    return std::to_string(m_value);
}

auto int_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto int_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto int_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto int_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto int_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto int_object_t::power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->power(shared_from_this(), other);
}

auto int_object_t::is_truthy() const -> bool
{
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

auto int_object_t::get_value() const -> int
{
    return m_value;
}

auto int_object_t::set_value(int val) -> void
{
    m_value = val;
}



} // namespace zephyr
