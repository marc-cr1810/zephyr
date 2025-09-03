#include "objects/int_object.hpp"
#include "types/int_type.hpp"
#include <string>
#include <cmath> // Added for std::pow

namespace zephyr
{

int_object_t::int_object_t(int val)
    : m_value(val)
{
}

auto int_object_t::type() const -> std::shared_ptr<type_t>
{
    return int_type_t::get_instance();
}

auto int_object_t::to_string() const -> std::string
{
    return std::to_string(m_value);
}

auto int_object_t::value() const -> int
{
    return m_value;
}

auto int_object_t::value(int val) -> void
{
    m_value = val;
}

} // namespace zephyr
