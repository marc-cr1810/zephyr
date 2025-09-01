#include "objects/dictionary_object.hpp"
#include "objects/string_object.hpp"
#include "objects/int_object.hpp"
#include "types/dictionary_type.hpp"
#include <sstream>
#include "errors.hpp"

namespace zephyr
{

dictionary_object_t::dictionary_object_t(std::map<std::string, std::shared_ptr<object_t>> elems)
    : m_elements(std::move(elems))
{
}

dictionary_object_t::dictionary_object_t()
{
}

auto dictionary_object_t::get_type() const -> std::shared_ptr<type_t>
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

auto dictionary_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto dictionary_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto dictionary_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto dictionary_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto dictionary_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto dictionary_object_t::is_truthy() const -> bool
{
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

auto dictionary_object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    return get_type()->get_item(shared_from_this(), index);
}

auto dictionary_object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    get_type()->set_item(shared_from_this(), index, value);
}

auto dictionary_object_t::get_elements() const -> const std::map<std::string, std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto dictionary_object_t::get_elements_mutable() -> std::map<std::string, std::shared_ptr<object_t>>&
{
    return m_elements;
}

auto dictionary_object_t::set_elements(const std::map<std::string, std::shared_ptr<object_t>>& elems) -> void
{
    m_elements = elems;
}



} // namespace zephyr
