#include "types/class_type.hpp"
#include "objects/class_object.hpp"
#include "objects/object.hpp"
#include "errors.hpp"
#include "objects/class_instance_object.hpp"

namespace zephyr
{

// Static member initialization
std::map<std::string, std::shared_ptr<class_type_t>> class_type_t::s_instances;

class_type_t::class_type_t(const std::string& class_name)
    : m_class_name(class_name)
{
}

auto class_type_t::get_instance(const std::string& class_name) -> std::shared_ptr<class_type_t>
{
    auto it = s_instances.find(class_name);
    if (it != s_instances.end()) {
        return it->second;
    }

    auto instance = std::shared_ptr<class_type_t>(new class_type_t(class_name));
    s_instances[class_name] = instance;
    return instance;
}

auto class_type_t::get_name() const -> std::string
{
    return m_class_name;
}



auto class_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Class instances are always truthy
}

auto class_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != m_class_name)
    {
        return false;
    }

    // Instance equality is pointer equality
    return self.get() == other.get();
}

auto class_type_t::get_member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    auto instance = std::static_pointer_cast<class_instance_t>(self);
    
    if (name.empty())
    {
        throw value_error_t("Member name cannot be empty");
    }

    auto it = instance->m_members.find(name);
    if (it != instance->m_members.end())
    {
        return it->second;
    }

    throw attribute_error_t("Member '" + name + "' not found in class instance");
}

auto class_type_t::set_member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
{
    auto instance = std::static_pointer_cast<class_instance_t>(self);

    if (name.empty())
    {
        throw value_error_t("Member name cannot be empty");
    }

    if (instance->is_member_const(name))
    {
        throw type_error_t("Cannot modify const member '" + name + "'");
    }

    // Check for type validation if member has explicit type
    if (instance->m_class_obj)
    {
        for (const auto& member_var : instance->m_class_obj->m_member_variables)
        {
            if (member_var.name == name && !member_var.type_name.empty())
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
                        throw type_error_t("Type mismatch for member '" + name +
                                               "': expected " + expected_type +
                                               ", got " + actual_type);
                    }
                }
                break;
            }
        }
    }

    instance->m_members[name] = value;
}



}
