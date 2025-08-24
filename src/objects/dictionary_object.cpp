#include "objects/dictionary_object.hpp"
#include "objects/string_object.hpp"
#include "objects/int_object.hpp"
#include "objects/none_object.hpp"
#include "types/dictionary_type.hpp"
#include <sstream>
#include <stdexcept>
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
    return !m_elements.empty();
}

auto dictionary_object_t::get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    auto key_str = key_to_string(index);
    
    if (has_key(key_str))
    {
        return m_elements[key_str];
    }
    else
    {
        throw key_error_t("Key '" + key_str + "' not found in dictionary");
    }
}

auto dictionary_object_t::set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    auto key_str = key_to_string(index);
    m_elements[key_str] = value;
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

auto dictionary_object_t::key_to_string(std::shared_ptr<object_t> key) const -> std::string
{
    if (!key)
    {
        throw type_error_t("Dictionary key cannot be null");
    }
    
    auto type_name = key->get_type()->get_name();
    
    if (type_name == "string")
    {
        auto str_obj = std::static_pointer_cast<string_object_t>(key);
        return str_obj->get_value();
    }
    else if (type_name == "int")
    {
        auto int_obj = std::static_pointer_cast<int_object_t>(key);
        return std::to_string(int_obj->get_value());
    }
    else
    {
        // For other types, use their string representation
        return key->to_string();
    }
}

auto dictionary_object_t::has_key(const std::string& key) const -> bool
{
    return m_elements.find(key) != m_elements.end();
}

auto dictionary_object_t::get_keys() const -> std::vector<std::string>
{
    std::vector<std::string> keys;
    keys.reserve(m_elements.size());
    
    for (const auto& pair : m_elements)
    {
        keys.push_back(pair.first);
    }
    
    return keys;
}

auto dictionary_object_t::get_values() const -> std::vector<std::shared_ptr<object_t>>
{
    std::vector<std::shared_ptr<object_t>> values;
    values.reserve(m_elements.size());
    
    for (const auto& pair : m_elements)
    {
        values.push_back(pair.second);
    }
    
    return values;
}

} // namespace zephyr