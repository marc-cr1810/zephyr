#include "types/interface_type.hpp"
#include "objects/objects.hpp"
#include <stdexcept>
#include "errors.hpp"

namespace zephyr
{

// Static member initialization
std::map<std::string, std::shared_ptr<interface_type_t>> interface_type_t::s_instances;

interface_type_t::interface_type_t(const std::string& interface_name)
    : m_interface_name(interface_name)
{
}

auto interface_type_t::get_instance(const std::string& interface_name) -> std::shared_ptr<interface_type_t>
{
    auto it = s_instances.find(interface_name);
    if (it != s_instances.end()) {
        return it->second;
    }

    auto instance = std::shared_ptr<interface_type_t>(new interface_type_t(interface_name));
    s_instances[interface_name] = instance;
    return instance;
}

auto interface_type_t::get_name() const -> std::string
{
    return m_interface_name;
}

auto interface_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Interface objects are always truthy
}

auto interface_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != m_interface_name)
    {
        return false;
    }

    // Instance equality is pointer equality
    return self.get() == other.get();
}

}
