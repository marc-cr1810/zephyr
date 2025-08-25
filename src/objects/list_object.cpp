#include "objects/list_object.hpp"
#include "objects/int_object.hpp"
#include "objects/none_object.hpp"
#include "types/list_type.hpp"
#include "errors.hpp"

namespace zephyr
{

list_object_t::list_object_t(const std::vector<std::shared_ptr<object_t>>& elements)
    : m_elements(elements)
{
}

list_object_t::list_object_t()
{
}

auto list_object_t::get_type() const -> std::shared_ptr<type_t>
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

auto list_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto list_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto list_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto list_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto list_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto list_object_t::is_truthy() const -> bool
{
    return !m_elements.empty();
}

auto list_object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    if (!index)
    {
        throw type_error_t("List index cannot be null");
    }

    auto int_index = std::dynamic_pointer_cast<int_object_t>(index);
    if (!int_index)
    {
        throw type_error_t("List indices must be integers, not '" + index->get_type()->get_name() + "'");
    }

    int idx = normalize_index(int_index->get_value());
    check_bounds(idx);

    return m_elements[idx];
}

auto list_object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    if (!index)
    {
        throw type_error_t("List index cannot be null");
    }

    if (!value)
    {
        throw value_error_t("List value cannot be null");
    }

    auto int_index = std::dynamic_pointer_cast<int_object_t>(index);
    if (!int_index)
    {
        throw type_error_t("List indices must be integers, not '" + index->get_type()->get_name() + "'");
    }

    int idx = normalize_index(int_index->get_value());
    check_bounds(idx);

    m_elements[idx] = value;
}

auto list_object_t::get_elements() const -> const std::vector<std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto list_object_t::get_elements_mutable() -> std::vector<std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto list_object_t::set_elements(const std::vector<std::shared_ptr<object_t>>& elems) -> void
{
    m_elements = elems;
}

auto list_object_t::check_bounds(int index) const -> void
{
    if (index < 0 || index >= static_cast<int>(m_elements.size()))
    {
        throw index_error_t("List index " + std::to_string(index) + " out of range [0, " +
                                std::to_string(m_elements.size()) + ")");
    }
}

auto list_object_t::normalize_index(int index) const -> int
{
    if (index < 0)
    {
        // Handle negative indices (Python-style)
        index += static_cast<int>(m_elements.size());
    }
    return index;
}

auto list_object_t::append_element(std::shared_ptr<object_t> element) -> void
{
    if (!element)
    {
        element = std::make_shared<none_object_t>();
    }
    m_elements.push_back(element);
}

auto list_object_t::pop_element(int index) -> std::shared_ptr<object_t>
{
    if (m_elements.empty())
    {
        throw index_error_t("Cannot pop from empty list");
    }

    int idx = normalize_index(index);
    check_bounds(idx);

    auto result = m_elements[idx];
    m_elements.erase(m_elements.begin() + idx);
    return result;
}

} // namespace zephyr
