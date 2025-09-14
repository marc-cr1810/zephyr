#include "zephyr/types/string_type.hpp"
#include "zephyr/objects/objects.hpp"
#include "zephyr/errors.hpp"
#include <algorithm>

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
        return std::make_shared<string_object_t>(std::static_pointer_cast<string_object_t>(self)->value() + other_string->value());
    }

    // Handle none values in string concatenation
    if (!other || other->type()->name() == "none")
    {
        return std::make_shared<string_object_t>(std::static_pointer_cast<string_object_t>(self)->value() + "none");
    }

    // Try to convert other type to string for concatenation
    try {
        return std::make_shared<string_object_t>(std::static_pointer_cast<string_object_t>(self)->value() + other->to_string());
    } catch (...) {
        std::string self_type = self ? self->type()->name() : "null";
        std::string other_type = other ? other->type()->name() : "null";
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
        int repeat_count = other_int->value();
        if (repeat_count < 0)
        {
            return std::make_shared<string_object_t>("");
        }
        std::string result;
        std::string str_value = std::static_pointer_cast<string_object_t>(self)->value();
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

auto string_type_t::name() const -> std::string
{
    return "string";
}

auto string_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto str_obj = std::static_pointer_cast<string_object_t>(self);
    return !str_obj->value().empty();
}

auto string_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->type()->name() != "string")
    {
        return false;
    }

    auto self_str = std::static_pointer_cast<string_object_t>(self);
    auto other_str = std::static_pointer_cast<string_object_t>(other);
    return self_str->value() == other_str->value();
}

auto string_type_t::item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    if (auto index_int = std::dynamic_pointer_cast<int_object_t>(index))
    {
        int idx = index_int->value();
        const std::string& str_value = self_str->value();

        if (idx < 0)
        {
            idx += static_cast<int>(str_value.length());
        }

        if (idx < 0 || idx >= static_cast<int>(str_value.length()))
        {
            throw index_error_t("String index out of bounds");
        }
        return std::make_shared<string_object_t>(std::string(1, str_value[idx]));
    }
    throw type_error_t("String index must be an integer");
}

auto string_type_t::item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Strings are immutable and do not support item assignment");
}

auto string_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    if (other->type()->name() != "string")
    {
        throw type_error_t("Cannot compare string with " + other->type()->name());
    }

    auto self_str = std::static_pointer_cast<string_object_t>(self);
    auto other_str = std::static_pointer_cast<string_object_t>(other);

    const std::string& self_value = self_str->value();
    const std::string& other_value = other_str->value();

    if (self_value < other_value) return -1;
    if (self_value > other_value) return 1;
    return 0;
}

auto string_type_t::contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    if (auto item_str = std::dynamic_pointer_cast<string_object_t>(item))
    {
        const std::string& self_value = self_str->value();
        const std::string& item_value = item_str->value();
        return self_value.find(item_value) != std::string::npos;
    }
    return false;
}

auto string_type_t::length(std::shared_ptr<object_t> self) -> int
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    return static_cast<int>(self_str->value().length());
}

auto string_type_t::slice(std::shared_ptr<object_t> self, 
                          std::shared_ptr<object_t> start, 
                          std::shared_ptr<object_t> end, 
                          std::shared_ptr<object_t> step) -> std::shared_ptr<object_t>
{
    auto self_str = std::static_pointer_cast<string_object_t>(self);
    const std::string& str_value = self_str->value();
    int str_length = static_cast<int>(str_value.length());
    
    // Parse start index (default to 0)
    int start_idx = 0;
    if (start && start->type()->name() != "none") {
        if (auto start_int = std::dynamic_pointer_cast<int_object_t>(start)) {
            start_idx = start_int->value();
            if (start_idx < 0) {
                start_idx += str_length;
            }
            start_idx = std::max(0, std::min(start_idx, str_length));
        } else {
            throw type_error_t("Slice indices must be integers or None");
        }
    }
    
    // Parse end index (default to length)
    int end_idx = str_length;
    if (end && end->type()->name() != "none") {
        if (auto end_int = std::dynamic_pointer_cast<int_object_t>(end)) {
            end_idx = end_int->value();
            if (end_idx < 0) {
                end_idx += str_length;
            }
            end_idx = std::max(0, std::min(end_idx, str_length));
        } else {
            throw type_error_t("Slice indices must be integers or None");
        }
    }
    
    // Parse step (default to 1)
    int step_val = 1;
    if (step && step->type()->name() != "none") {
        if (auto step_int = std::dynamic_pointer_cast<int_object_t>(step)) {
            step_val = step_int->value();
            if (step_val == 0) {
                throw value_error_t("Slice step cannot be zero");
            }
        } else {
            throw type_error_t("Slice step must be an integer");
        }
    }
    
    // Build the result string
    std::string result;
    if (step_val > 0) {
        // Forward iteration
        for (int i = start_idx; i < end_idx; i += step_val) {
            result += str_value[i];
        }
    } else {
        // Backward iteration
        // When step is negative, swap the default start and end
        if (!start && !end) {
            start_idx = str_length - 1;
            end_idx = -1;
        } else if (!start) {
            start_idx = str_length - 1;
        } else if (!end) {
            end_idx = -1;
        }
        
        // Swap if necessary for negative step
        if (start_idx < end_idx && step_val < 0) {
            std::swap(start_idx, end_idx);
            end_idx--;
        }
        
        for (int i = start_idx; i > end_idx; i += step_val) {
            if (i >= 0 && i < str_length) {
                result += str_value[i];
            }
        }
    }
    
    return std::make_shared<string_object_t>(result);
}

} // namespace zephyr
