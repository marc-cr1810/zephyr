#include "objects/int_object.hpp"
#include "objects/float_object.hpp"
#include "types/int_type.hpp"
#include <string>
#include "errors.hpp"

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
    check_division_by_zero(other);
    return get_type()->divide(shared_from_this(), other);
}

auto int_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    check_division_by_zero(other);
    return get_type()->modulo(shared_from_this(), other);
}

auto int_object_t::is_truthy() const -> bool
{
    return m_value != 0;
}

auto int_object_t::get_value() const -> int
{
    return m_value;
}

auto int_object_t::set_value(int val) -> void
{
    m_value = val;
}

auto int_object_t::check_division_by_zero(std::shared_ptr<object_t> other) const -> void
{
    if (!other)
    {
        throw value_error_t("Cannot divide by null object");
    }

    auto type_name = other->get_type()->get_name();

    if (type_name == "int")
    {
        auto int_obj = std::static_pointer_cast<int_object_t>(other);
        if (int_obj->get_value() == 0)
        {
            throw zero_division_error_t("Division by zero");
        }
    }
    else if (type_name == "float")
    {
        auto float_obj = std::static_pointer_cast<float_object_t>(other);
        if (float_obj->get_value() == 0.0)
        {
            throw zero_division_error_t("Division by zero");
        }
    }
}

} // namespace zephyr
