#include "objects/string_object.hpp"
#include "types/string_type.hpp"

namespace zephyr
{

string_object_t::string_object_t(const std::string& val)
    : m_value(val)
{
}

auto string_object_t::type() const -> std::shared_ptr<type_t>
{
    return string_type_t::get_instance();
}

auto string_object_t::to_string() const -> std::string
{
    return m_value;
}

auto string_object_t::value() const -> const std::string&
{
    return m_value;
}

auto string_object_t::value(const std::string& val) -> void
{
    m_value = val;
}

} // namespace zephyr
