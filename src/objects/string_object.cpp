#include "objects/string_object.hpp"
#include "types/string_type.hpp"
#include "errors.hpp"
#include "objects/int_object.hpp" // Added for int_object_t

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
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

auto string_object_t::get_item(std::shared_ptr<object_t> index_obj) -> std::shared_ptr<object_t>
{
    return get_type()->get_item(shared_from_this(), index_obj);
}

auto string_object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    get_type()->set_item(shared_from_this(), index, value);
}

auto string_object_t::get_value() const -> const std::string&
{
    return m_value;
}

auto string_object_t::set_value(const std::string& val) -> void
{
    m_value = val;
}

} // namespace zephyr