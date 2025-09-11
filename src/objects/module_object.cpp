#include "objects/module_object.hpp"
#include "types/module_type.hpp"
#include "module_loader.hpp"
#include "errors.hpp"

namespace zephyr
{

module_object_t::module_object_t(std::shared_ptr<module_t> module, const std::string& module_name)
    : m_module(module), m_module_name(module_name)
{
}

auto module_object_t::type() const -> std::shared_ptr<type_t>
{
    return module_type_t::get_instance();
}

auto module_object_t::to_string() const -> std::string
{
    return "<module '" + m_module_name + "'>";
}

auto module_object_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Get the exported function from the module
    auto export_value = m_module->get_export(method_name);
    if (!export_value)
    {
        throw attribute_error_t("Module '" + m_module_name + "' has no function '" + method_name + "'");
    }
    
    // Call the function with the provided arguments
    return export_value->call(args);
}

auto module_object_t::member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    auto export_value = m_module->get_export(member_name);
    if (!export_value)
    {
        throw attribute_error_t("Module '" + m_module_name + "' has no export '" + member_name + "'");
    }
    return export_value;
}

auto module_object_t::has_member(const std::string& name) const -> bool
{
    return m_module->get_export(name) != nullptr;
}

auto module_object_t::get_module() const -> std::shared_ptr<module_t>
{
    return m_module;
}

auto module_object_t::get_module_name() const -> const std::string&
{
    return m_module_name;
}

} // namespace zephyr