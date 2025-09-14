#include "zephyr/objects/class_instance_object.hpp"
#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/types/class_type.hpp"
#include "zephyr/errors.hpp"
#include <sstream>

namespace zephyr
{

class_instance_t::class_instance_t(std::shared_ptr<class_object_t> class_obj)
    : m_class_obj(class_obj)
{
    initialize_default_members();
}

auto class_instance_t::type() const -> std::shared_ptr<type_t>
{
    return class_type_t::get_instance(m_class_obj->m_class_name);
}

auto class_instance_t::to_string() const -> std::string
{
    std::stringstream ss;
    ss << "<" << m_class_obj->m_class_name << " object at " << std::hex << shared_from_this().get() << ">";
    return std::string(ss.str());
}

auto class_instance_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Method execution should be handled by the interpreter
    // This method should not be called directly - similar to function_object_t
    throw type_error_t("Method call should be handled by interpreter");
}

auto class_instance_t::has_member(const std::string& name) const -> bool
{
    return m_members.find(name) != m_members.end();
}

auto class_instance_t::member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    return type()->member(shared_from_this(), member_name);
}

auto class_instance_t::member(const std::string& member_name, std::shared_ptr<object_t> value) -> void
{
    type()->member(shared_from_this(), member_name, value);
}

auto class_instance_t::has_method(const std::string& name) const -> bool
{
    return m_class_obj->has_method(name);
}

auto class_instance_t::method(const std::string& name) const -> std::shared_ptr<function_definition_t>
{
    return m_class_obj->method(name);
}

auto class_instance_t::mark_member_const(const std::string& name) -> void
{
    m_const_members.insert(name);
}

auto class_instance_t::is_member_const(const std::string& name) const -> bool
{
    return m_const_members.find(name) != m_const_members.end();
}

auto class_instance_t::initialize_default_members() -> void
{
    if (!m_class_obj)
    {
        return;
    }

    // Initialize parent class member variables first (if any)
    auto parent_class = m_class_obj->parent_class();
    if (parent_class)
    {
        for (const auto& member_var : parent_class->m_member_variables)
        {
            // Check if this member is overridden in the child class
            bool overridden = false;
            for (const auto& child_var : m_class_obj->m_member_variables)
            {
                if (child_var.name == member_var.name)
                {
                    overridden = true;
                    break;
                }
            }
            
            if (!overridden)
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
    }

    // Initialize class's own member variables
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

} // namespace zephyr
