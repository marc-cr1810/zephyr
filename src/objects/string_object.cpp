#include "objects/string_object.hpp"
#include "types/string_type.hpp"

namespace zephyr
{

string_object_t::string_object_t(const std::string& val)
    : m_value(val)
{
}

auto string_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return string_type_t::get_instance();
}

auto string_object_t::to_string() const -> std::string
{
    return m_value;
}

auto string_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto string_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto string_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto string_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto string_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto string_object_t::is_truthy() const -> bool
{
    return !m_value.empty();
}

auto string_object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    // Implementation would go here for string indexing
    throw std::runtime_error("String indexing not yet implemented");
}

auto string_object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    // Implementation would go here for string item setting
    throw std::runtime_error("String item setting not supported");
}

auto string_object_t::get_value() const -> const std::string&
{
    return m_value;
}

auto string_object_t::set_value(const std::string& val) -> void
{
    m_value = val;
}

auto string_object_t::check_bounds(int index) const -> void
{
    if (index < 0 || index >= static_cast<int>(m_value.length()))
    {
        throw std::runtime_error("String index out of bounds");
    }
}

auto string_object_t::normalize_index(int index) const -> int
{
    if (index < 0)
    {
        index += static_cast<int>(m_value.length());
    }
    return index;
}

} // namespace zephyr