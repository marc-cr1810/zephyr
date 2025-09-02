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