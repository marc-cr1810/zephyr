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
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

auto list_object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    return get_type()->get_item(shared_from_this(), index);
}

auto list_object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    get_type()->set_item(shared_from_this(), index, value);
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



} // namespace zephyr
