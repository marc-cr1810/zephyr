#include "objects/boolean_object.hpp"
#include "types/boolean_type.hpp"

namespace zephyr
{

std::shared_ptr<boolean_object_t> boolean_object_t::s_true_instance = nullptr;
std::shared_ptr<boolean_object_t> boolean_object_t::s_false_instance = nullptr;

boolean_object_t::boolean_object_t(bool val)
    : m_value(val)
{
}

auto boolean_object_t::get_type() const -> std::shared_ptr<type_t> 
{
    return boolean_type_t::get_instance();
}

auto boolean_object_t::to_string() const -> std::string 
{
    return m_value ? "true" : "false";
}

auto boolean_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> 
{
    return get_type()->add(shared_from_this(), other);
}

auto boolean_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> 
{
    return get_type()->subtract(shared_from_this(), other);
}

auto boolean_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> 
{
    return get_type()->multiply(shared_from_this(), other);
}

auto boolean_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> 
{
    return get_type()->divide(shared_from_this(), other);
}

auto boolean_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> 
{
    return get_type()->modulo(shared_from_this(), other);
}

auto boolean_object_t::is_truthy() const -> bool
{
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

auto boolean_object_t::get_true() -> std::shared_ptr<boolean_object_t>
{
    if (!s_true_instance) {
        s_true_instance = std::shared_ptr<boolean_object_t>(new boolean_object_t(true));
    }
    return s_true_instance;
}

auto boolean_object_t::get_false() -> std::shared_ptr<boolean_object_t>
{
    if (!s_false_instance) {
        s_false_instance = std::shared_ptr<boolean_object_t>(new boolean_object_t(false));
    }
    return s_false_instance;
}

auto boolean_object_t::get_instance(bool value) -> std::shared_ptr<boolean_object_t>
{
    return value ? get_true() : get_false();
}

} // namespace zephyr