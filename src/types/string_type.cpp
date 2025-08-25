#include "types/string_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"

namespace zephyr
{

auto string_type_t::get_instance() -> std::shared_ptr<string_type_t>
{
    static auto instance = std::shared_ptr<string_type_t>(new string_type_t());
    return instance;
}

auto string_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_string = std::dynamic_pointer_cast<string_object_t>(other))
    {
        return std::make_shared<string_object_t>(std::static_pointer_cast<string_object_t>(self)->get_value() + other_string->get_value());
    }

    // Handle none values in string concatenation
    if (!other || other->get_type()->get_name() == "none")
    {
        return std::make_shared<string_object_t>(std::static_pointer_cast<string_object_t>(self)->get_value() + "none");
    }

    // Try to convert other type to string for concatenation
    try {
        return std::make_shared<string_object_t>(std::static_pointer_cast<string_object_t>(self)->get_value() + other->to_string());
    } catch (...) {
        std::string self_type = self ? self->get_type()->get_name() : "null";
        std::string other_type = other ? other->get_type()->get_name() : "null";
        throw type_error_t("Unsupported operand types for +: " + self_type + " + " + other_type);
    }
}

auto string_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for strings");
}

auto string_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        // string * int: repeat the string int times
        int repeat_count = other_int->get_value();
        if (repeat_count < 0)
        {
            return std::make_shared<string_object_t>("");
        }
        std::string result;
        std::string str_value = std::static_pointer_cast<string_object_t>(self)->get_value();
        for (int i = 0; i < repeat_count; ++i)
        {
            result += str_value;
        }
        return std::make_shared<string_object_t>(result);
    }
    throw type_error_t("Unsupported operand types for *");
}

auto string_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for strings");
}

auto string_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for strings");
}

auto string_type_t::get_name() const -> std::string
{
    return "string";
}

auto string_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto str_obj = std::static_pointer_cast<string_object_t>(self);
    return !str_obj->get_value().empty();
}

auto string_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "string")
    {
        return false;
    }

    auto self_str = std::static_pointer_cast<string_object_t>(self);
    auto other_str = std::static_pointer_cast<string_object_t>(other);
    return self_str->get_value() == other_str->get_value();
}

auto string_type_t::get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    if (auto index_int = std::dynamic_pointer_cast<int_object_t>(index))
    {
        int idx = index_int->get_value();
        const std::string& str_value = self_str->get_value();
        if (idx < 0 || idx >= static_cast<int>(str_value.length()))
        {
            throw index_error_t("String index out of bounds");
        }
        return std::make_shared<string_object_t>(std::string(1, str_value[idx]));
    }
    throw type_error_t("String index must be an integer");
}

auto string_type_t::set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Strings are immutable and do not support item assignment");
}

auto string_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    if (other->get_type()->get_name() != "string")
    {
        throw type_error_t("Cannot compare string with " + other->get_type()->get_name());
    }

    auto self_str = std::static_pointer_cast<string_object_t>(self);
    auto other_str = std::static_pointer_cast<string_object_t>(other);

    const std::string& self_value = self_str->get_value();
    const std::string& other_value = other_str->get_value();

    if (self_value < other_value) return -1;
    if (self_value > other_value) return 1;
    return 0;
}

auto string_type_t::contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    if (auto item_str = std::dynamic_pointer_cast<string_object_t>(item))
    {
        const std::string& self_value = self_str->get_value();
        const std::string& item_value = item_str->get_value();
        return self_value.find(item_value) != std::string::npos;
    }
    return false;
}

auto string_type_t::get_length(std::shared_ptr<object_t> self) -> int
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    return static_cast<int>(self_str->get_value().length());
}


} // namespace zephyr
