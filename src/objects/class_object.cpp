#include "objects/class_object.hpp"
#include "objects/class_instance_object.hpp"
#include "types/function_type.hpp"
#include "ast.hpp"

namespace zephyr
{

class_object_t::class_object_t(const std::string& class_name)
    : m_class_name(class_name)
{
}

auto class_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return function_type_t::get_instance();  // Reuse function type for now
}

auto class_object_t::to_string() const -> std::string
{
    return "<class " + m_class_name + ">";
}

auto class_object_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Class instantiation should be handled by the interpreter
    // This method should not be called directly - similar to function_object_t
    throw std::runtime_error("Class instantiation should be handled by interpreter");
}

auto class_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for classes");
}

auto class_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for classes");
}

auto class_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for classes");
}

auto class_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for classes");
}

auto class_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for classes");
}

void class_object_t::add_interface(const std::string& interface_name)
{
    m_interfaces.push_back(interface_name);
}

const std::vector<std::string>& class_object_t::get_interfaces() const
{
    return m_interfaces;
}

auto class_object_t::add_method(const std::string& method_name, std::shared_ptr<function_definition_t> method) -> void
{
    validate_method_name(method_name);
    
    if (method)
    {
        m_methods[method_name] = method;
    }
}

auto class_object_t::add_member_variable(const member_variable_info_t& var_info) -> void
{
    validate_member_variable_name(var_info.name);
    
    // Check if member variable already exists
    for (const auto& existing : m_member_variables)
    {
        if (existing.name == var_info.name)
        {
            throw std::runtime_error("Member variable '" + var_info.name + "' already defined in class '" + m_class_name + "'");
        }
    }
    
    m_member_variables.push_back(var_info);
}

auto class_object_t::has_method(const std::string& method_name) const -> bool
{
    return m_methods.find(method_name) != m_methods.end();
}

auto class_object_t::get_method(const std::string& method_name) const -> std::shared_ptr<function_definition_t>
{
    auto it = m_methods.find(method_name);
    if (it != m_methods.end())
    {
        return it->second;
    }
    return nullptr;
}

auto class_object_t::has_member_variable(const std::string& var_name) const -> bool
{
    for (const auto& var : m_member_variables)
    {
        if (var.name == var_name)
        {
            return true;
        }
    }
    return false;
}

auto class_object_t::get_member_variable_info(const std::string& var_name) const -> member_variable_info_t
{
    for (const auto& var : m_member_variables)
    {
        if (var.name == var_name)
        {
            return var;
        }
    }
    throw std::runtime_error("Member variable '" + var_name + "' not found in class '" + m_class_name + "'");
}

auto class_object_t::validate_method_name(const std::string& method_name) const -> void
{
    if (method_name.empty())
    {
        throw std::runtime_error("Method name cannot be empty in class '" + m_class_name + "'");
    }
    
    if (m_methods.find(method_name) != m_methods.end())
    {
        throw std::runtime_error("Method '" + method_name + "' already defined in class '" + m_class_name + "'");
    }
}

auto class_object_t::validate_member_variable_name(const std::string& var_name) const -> void
{
    if (var_name.empty())
    {
        throw std::runtime_error("Member variable name cannot be empty in class '" + m_class_name + "'");
    }
}

auto class_object_t::format_methods() const -> std::string
{
    std::string result = "[";
    bool first = true;
    
    for (const auto& pair : m_methods)
    {
        if (!first)
        {
            result += ", ";
        }
        first = false;
        result += pair.first;
    }
    
    result += "]";
    return result;
}

auto class_object_t::format_member_variables() const -> std::string
{
    std::string result = "[";
    bool first = true;
    
    for (const auto& var : m_member_variables)
    {
        if (!first)
        {
            result += ", ";
        }
        first = false;
        
        result += var.name;
        if (!var.type_name.empty())
        {
            result += ": " + var.type_name;
        }
        if (var.is_const)
        {
            result += " (const)";
        }
    }
    
    result += "]";
    return result;
}

} // namespace zephyr