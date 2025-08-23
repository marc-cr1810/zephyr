#include "objects/class_instance_object.hpp"
#include "objects/class_object.hpp"
#include "objects/none_object.hpp"
#include "types/class_type.hpp"
#include "errors.hpp"

namespace zephyr
{

class_instance_t::class_instance_t(std::shared_ptr<class_object_t> class_obj)
    : m_class_obj(class_obj)
{
    initialize_default_members();
}

auto class_instance_t::get_type() const -> std::shared_ptr<type_t>
{
    return class_type_t::get_instance(m_class_obj->m_class_name);
}

auto class_instance_t::to_string() const -> std::string
{
    return "<instance of " + m_class_obj->m_class_name + ">";
}

auto class_instance_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for class instances", 0, 0, 1);
}

auto class_instance_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for class instances", 0, 0, 1);
}

auto class_instance_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for class instances", 0, 0, 1);
}

auto class_instance_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for class instances", 0, 0, 1);
}

auto class_instance_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for class instances", 0, 0, 1);
}

auto class_instance_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Method execution should be handled by the interpreter
    // This method should not be called directly - similar to function_object_t
    throw type_error_t("Method call should be handled by interpreter", 0, 0, 1);
}

auto class_instance_t::has_member(const std::string& name) const -> bool
{
    return m_members.find(name) != m_members.end();
}

auto class_instance_t::get_member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    validate_member_access(member_name);
    
    auto it = m_members.find(member_name);
    if (it != m_members.end())
    {
        return it->second;
    }
    
    throw attribute_error_t("Member '" + member_name + "' not found in class instance", 0, 0, 1);
}

auto class_instance_t::set_member(const std::string& member_name, std::shared_ptr<object_t> value) -> void
{
    validate_member_assignment(member_name);
    
    // Check for type validation if member has explicit type
    if (m_class_obj)
    {
        for (const auto& member_var : m_class_obj->m_member_variables)
        {
            if (member_var.name == member_name && !member_var.type_name.empty())
            {
                // Member has explicit type - validate assignment
                if (value->get_type()->get_name() != "none")
                {
                    std::string expected_type = member_var.type_name;
                    std::string actual_type = value->get_type()->get_name();
                    
                    // Normalize type names for common aliases
                    if (expected_type == "int" && actual_type == "number") {
                        actual_type = "int";
                    } else if (expected_type == "float" && actual_type == "number") {
                        actual_type = "float";
                    } else if (expected_type == "string" && actual_type == "string_literal") {
                        actual_type = "string";
                    }
                    
                    if (actual_type != expected_type)
                    {
                        throw type_error_t("Type mismatch for member '" + member_name + 
                                               "': expected " + expected_type + 
                                               ", got " + actual_type, 0, 0, 1);
                    }
                }
                break;
            }
        }
    }
    
    m_members[member_name] = value;
}

auto class_instance_t::has_method(const std::string& name) const -> bool
{
    return m_class_obj->has_method(name);
}

auto class_instance_t::get_method(const std::string& name) const -> std::shared_ptr<function_definition_t>
{
    return m_class_obj->get_method(name);
}

auto class_instance_t::mark_member_const(const std::string& name) -> void
{
    m_const_members.insert(name);
}

auto class_instance_t::is_member_const(const std::string& name) const -> bool
{
    return m_const_members.find(name) != m_const_members.end();
}

auto class_instance_t::validate_member_access(const std::string& name) const -> void
{
    if (name.empty())
    {
        throw value_error_t("Member name cannot be empty", 0, 0, 1);
    }
}

auto class_instance_t::validate_member_assignment(const std::string& name) const -> void
{
    validate_member_access(name);
    
    if (is_member_const(name))
    {
        throw type_error_t("Cannot modify const member '" + name + "'", 0, 0, 1);
    }
}

auto class_instance_t::initialize_default_members() -> void
{
    if (!m_class_obj)
    {
        return;
    }
    
    for (const auto& member_var : m_class_obj->m_member_variables)
    {
        if (member_var.has_default)
        {
            if (member_var.default_value)
            {
                m_members[member_var.name] = member_var.default_value;
            }
            else
            {
                m_members[member_var.name] = std::make_shared<none_object_t>();
            }
        }
        else
        {
            m_members[member_var.name] = std::make_shared<none_object_t>();
        }
        
        if (member_var.is_const)
        {
            mark_member_const(member_var.name);
        }
    }
}

auto class_instance_t::format_members() const -> std::string
{
    std::string result = "{";
    bool first = true;
    
    for (const auto& pair : m_members)
    {
        if (!first)
        {
            result += ", ";
        }
        first = false;
        
        result += pair.first + ": " + pair.second->to_string();
        
        if (is_member_const(pair.first))
        {
            result += " (const)";
        }
    }
    
    result += "}";
    return result;
}

} // namespace zephyr
