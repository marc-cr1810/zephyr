#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include "zephyr/objects/function_object.hpp"
#include "zephyr/types/function_type.hpp"
#include "zephyr/ast.hpp"
#include "zephyr/errors.hpp"

namespace zephyr
{

class_object_t::class_object_t(const std::string& class_name)
    : m_class_name(class_name)
{
}

auto class_object_t::type() const -> std::shared_ptr<type_t>
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
    throw type_error_t("Class instantiation should be handled by interpreter");
}

void class_object_t::add_interface(const std::string& interface_name)
{
    m_interfaces.push_back(interface_name);
}

const std::vector<std::string>& class_object_t::interfaces() const
{
    return m_interfaces;
}

auto class_object_t::add_method(const std::string& method_name, std::shared_ptr<function_definition_t> method) -> void
{
    validate_method_name(method_name);

    if (method)
    {
        // Keep for backward compatibility - store first method with this name
        if (m_methods.find(method_name) == m_methods.end()) {
            m_methods[method_name] = method;
        }
        
        // Only add non-abstract methods to the overload resolver
        // Abstract methods define the interface but cannot be called
        if (!method->is_abstract)
        {
            // Create function object and add to overload resolver
            auto func_obj = std::make_shared<function_object_t>(
                method->parameters, 
                std::unique_ptr<block_t>(static_cast<block_t*>(method->body->clone().release())), 
                method->return_type_name, 
                method->explicit_return_type, 
                method->async
            );
            
            m_method_resolver.add_overload(method_name, method->parameters, func_obj);
        }
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
            throw attribute_error_t("Member variable '" + var_info.name + "' already defined in class '" + m_class_name + "'");
        }
    }

    m_member_variables.push_back(var_info);
}

auto class_object_t::has_method(const std::string& method_name) const -> bool
{
    // Check this class's methods
    if (m_method_resolver.has_function(method_name)) {
        return true;
    }
    
    // Check parent class's methods
    if (m_parent_class) {
        return m_parent_class->has_method(method_name);
    }
    
    return false;
}

auto class_object_t::method(const std::string& method_name) const -> std::shared_ptr<function_definition_t>
{
    // Check this class's methods
    auto it = m_methods.find(method_name);
    if (it != m_methods.end())
    {
        return it->second;
    }
    
    // Check parent class's methods
    if (m_parent_class)
    {
        return m_parent_class->method(method_name);
    }
    
    throw attribute_error_t("Method '" + method_name + "' not found in class '" + m_class_name + "'");
}

auto class_object_t::resolve_method_call(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> overload_resolution_result_t
{
    // Try to resolve in this class first
    auto result = m_method_resolver.resolve_call(method_name, args);
    if (result.found_match) {
        return result;
    }
    
    // If not found and we have a parent class, try there
    if (m_parent_class) {
        return m_parent_class->resolve_method_call(method_name, args);
    }
    
    return result;
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
    throw attribute_error_t("Member variable '" + var_name + "' not found in class '" + m_class_name + "'");
}

auto class_object_t::member_variable_info(const std::string& var_name) const -> member_variable_info_t
{
    for (const auto& var : m_member_variables)
    {
        if (var.name == var_name)
        {
            return var;
        }
    }
    throw attribute_error_t("Member variable '" + var_name + "' not found in class '" + m_class_name + "'");
}

auto class_object_t::set_parent_class(std::shared_ptr<class_object_t> parent) -> void
{
    m_parent_class = parent;
}

auto class_object_t::parent_class() const -> std::shared_ptr<class_object_t>
{
    return m_parent_class;
}

auto class_object_t::has_method_including_parent(const std::string& method_name) const -> bool
{
    if (has_method(method_name))
    {
        return true;
    }
    
    if (m_parent_class)
    {
        return m_parent_class->has_method_including_parent(method_name);
    }
    
    return false;
}

auto class_object_t::method_including_parent(const std::string& method_name) const -> std::shared_ptr<function_definition_t>
{
    if (has_method(method_name))
    {
        return method(method_name);
    }
    
    if (m_parent_class)
    {
        return m_parent_class->method_including_parent(method_name);
    }
    
    throw attribute_error_t("Method '" + method_name + "' not found in class '" + m_class_name + "' or its parent classes");
}

auto class_object_t::has_member_variable_including_parent(const std::string& var_name) const -> bool
{
    for (const auto& var : m_member_variables)
    {
        if (var.name == var_name)
        {
            return true;
        }
    }
    
    if (m_parent_class)
    {
        return m_parent_class->has_member_variable_including_parent(var_name);
    }
    
    return false;
}

auto class_object_t::member_variable_info_including_parent(const std::string& var_name) const -> member_variable_info_t
{
    for (const auto& var : m_member_variables)
    {
        if (var.name == var_name)
        {
            return var;
        }
    }
    
    if (m_parent_class)
    {
        return m_parent_class->member_variable_info_including_parent(var_name);
    }
    
    throw attribute_error_t("Member variable '" + var_name + "' not found in class '" + m_class_name + "' or its parent classes");
}

auto class_object_t::validate_method_name(const std::string& method_name) const -> void
{
    if (method_name.empty())
    {
        throw value_error_t("Method name cannot be empty in class '" + m_class_name + "'");
    }

    // Remove the check for existing methods to allow overloading
    // Method overloading is now handled by the method resolver
}

auto class_object_t::validate_member_variable_name(const std::string& var_name) const -> void
{
    if (var_name.empty())
    {
        throw value_error_t("Member variable name cannot be empty in class '" + m_class_name + "'");
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
