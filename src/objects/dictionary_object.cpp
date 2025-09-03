#include "objects/dictionary_object.hpp"
#include "types/dictionary_type.hpp"
#include <sstream>

namespace zephyr
{

dictionary_object_t::dictionary_object_t(std::map<std::string, std::shared_ptr<object_t>> elems)
    : m_elements(std::move(elems))
{
}

dictionary_object_t::dictionary_object_t()
{
}

auto dictionary_object_t::type() const -> std::shared_ptr<type_t>
{
    return dictionary_type_t::get_instance();
}

auto dictionary_object_t::to_string() const -> std::string
{
    std::stringstream ss;
    ss << "{";
    bool first = true;

    for (const auto& pair : m_elements)
    {
        if (!first)
        {
            ss << ", ";
        }
        ss << pair.first << ": " << pair.second->to_string();
        first = false;
    }

    ss << "}";
    return ss.str();
}

auto dictionary_object_t::elements() const -> const std::map<std::string, std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto dictionary_object_t::elements_mutable() -> std::map<std::string, std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto dictionary_object_t::elements(const std::map<std::string, std::shared_ptr<object_t>>& elems) -> void
{
    m_elements = elems;
}

} // namespace zephyr
