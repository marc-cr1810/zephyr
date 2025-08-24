#include "objects/interface_object.hpp"
#include "types/interface_type.hpp"
#include <stdexcept>
#include "errors.hpp"

namespace zephyr
{

interface_object_t::interface_object_t(const std::string& interface_name)
    : m_interface_name(interface_name)
{
}

auto interface_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return interface_type_t::get_instance(m_interface_name);
}

auto interface_object_t::to_string() const -> std::string
{
    return "<interface " + m_interface_name + ">";
}

void interface_object_t::add_method(const function_signature_t& method)
{
    m_methods.push_back(method);
}

const std::vector<function_signature_t>& interface_object_t::get_methods() const
{
    return m_methods;
}

auto interface_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for interfaces");
}

auto interface_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for interfaces");
}

auto interface_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for interfaces");
}

auto interface_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for interfaces");
}

auto interface_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for interfaces");
}

}