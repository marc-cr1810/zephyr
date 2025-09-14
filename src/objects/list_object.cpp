#include "zephyr/objects/list_object.hpp"
#include "zephyr/types/list_type.hpp"

namespace zephyr
{

list_object_t::list_object_t(const std::vector<std::shared_ptr<object_t>>& elements)
    : m_elements(elements)
{
}

list_object_t::list_object_t()
{
}

auto list_object_t::type() const -> std::shared_ptr<type_t>
{
    return list_type_t::get_instance();
}

auto list_object_t::to_string() const -> std::string
{
    std::string result = "[";
    bool first = true;

    for (const auto& element : m_elements)
    {
        if (!first)
        {
            result += ", ";
        }
        first = false;
        result += element->to_string();
    }

    result += "]";
    return result;
}

auto list_object_t::elements() const -> const std::vector<std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto list_object_t::elements_mutable() -> std::vector<std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto list_object_t::elements(const std::vector<std::shared_ptr<object_t>>& elems) -> void
{
    m_elements = elems;
}

} // namespace zephyr
