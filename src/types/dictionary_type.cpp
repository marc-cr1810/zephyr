#include "types/dictionary_type.hpp"
#include "objects/objects.hpp"
#include <stdexcept>
#include "errors.hpp"

namespace zephyr
{

auto dictionary_type_t::get_instance() -> std::shared_ptr<dictionary_type_t>
{
    static auto instance = std::shared_ptr<dictionary_type_t>(new dictionary_type_t());
    return instance;
}

auto dictionary_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for dictionaries", 0, 0, 1);
}

auto dictionary_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for dictionaries", 0, 0, 1);
}

auto dictionary_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Operation not supported for dictionaries", 0, 0, 1);
}

auto dictionary_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for dictionaries", 0, 0, 1);
}

auto dictionary_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for dictionaries", 0, 0, 1);
}

auto dictionary_type_t::get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    if (auto index_string = std::dynamic_pointer_cast<string_object_t>(index))
    {
        const auto& elements = self_dict->get_elements();
        if (elements.count(index_string->get_value()))
        {
            return elements.at(index_string->get_value());
        }
        else
        {
            throw key_error_t("Key '" + index_string->get_value() + "' not found in dictionary.", 0, 0, 1);
        }
    }
    throw type_error_t("Dictionary key must be a string.", 0, 0, 1);
}

auto dictionary_type_t::set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    if (auto index_string = std::dynamic_pointer_cast<string_object_t>(index))
    {
        auto& elements = self_dict->get_elements_mutable();
        elements[index_string->get_value()] = value;
    }
    else
    {
        throw type_error_t("Dictionary key must be a string.", 0, 0, 1);
    }
}

auto dictionary_type_t::get_name() const -> std::string
{
    return "dictionary";
}

auto dictionary_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto dict_obj = std::static_pointer_cast<dictionary_object_t>(self);
    return !dict_obj->get_elements().empty();
}

auto dictionary_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "dictionary")
    {
        return false;
    }

    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    auto other_dict = std::static_pointer_cast<dictionary_object_t>(other);

    const auto& self_elements = self_dict->get_elements();
    const auto& other_elements = other_dict->get_elements();

    return self_elements.size() == other_elements.size();
}

auto dictionary_type_t::is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    const auto& elements = self_dict->get_elements();
    return elements.count(name) > 0;
}

auto dictionary_type_t::get_member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    const auto& elements = self_dict->get_elements();
    if (elements.count(name))
    {
        return elements.at(name);
    }
    throw attribute_error_t("Dictionary has no member '" + name + "'", 0, 0, 1);
}

auto dictionary_type_t::set_member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    auto& elements = self_dict->get_elements_mutable();
    elements[name] = value;
}

auto dictionary_type_t::contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    const auto& elements = self_dict->get_elements();

    // For dictionaries, 'in' checks for key membership
    if (auto key_string = std::dynamic_pointer_cast<string_object_t>(item))
    {
        return elements.count(key_string->get_value()) > 0;
    }
    return false;
}

auto dictionary_type_t::get_length(std::shared_ptr<object_t> self) -> int
{
    auto self_dict = std::static_pointer_cast<dictionary_object_t>(self);
    return static_cast<int>(self_dict->get_elements().size());
}


} // namespace zephyr