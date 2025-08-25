#include "types/class_type.hpp"
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

auto class_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("addition");
    return nullptr;
}

auto class_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("subtraction");
    return nullptr;
}

auto class_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("multiplication");
    return nullptr;
}

auto class_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("division");
    return nullptr;
}

auto class_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("modulo");
    return nullptr;
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
    auto instance = std::dynamic_pointer_cast<class_instance_t>(self);
    if (instance)
    {
        return instance->get_member(name);
    }
    throw type_error_t("Cannot access member on non-class instance");
}

auto class_type_t::set_member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
{
    auto instance = std::dynamic_pointer_cast<class_instance_t>(self);
    if (instance)
    {
        instance->set_member(name, value);
        return;
    }
    throw type_error_t("Cannot set member on non-class instance");
}

auto class_type_t::throw_unsupported_operation(const std::string& operation) const -> void
{
    throw type_error_t("Unsupported operation '" + operation + "' for class type '" + m_class_name + "'");
}

}
