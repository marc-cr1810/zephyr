#include "types/int_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"
#include <cmath>

namespace zephyr
{

auto int_type_t::get_instance() -> std::shared_ptr<int_type_t>
{
    static auto instance = std::shared_ptr<int_type_t>(new int_type_t());
    return instance;
}

auto int_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        return std::make_shared<int_object_t>(std::static_pointer_cast<int_object_t>(self)->get_value() + other_int->get_value());
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        return std::make_shared<float_object_t>(static_cast<double>(std::static_pointer_cast<int_object_t>(self)->get_value()) + other_float->get_value());
    }
    throw type_error_t("Unsupported operand types for +", 0, 0, 1);
}

auto int_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        return std::make_shared<int_object_t>(std::static_pointer_cast<int_object_t>(self)->get_value() - other_int->get_value());
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        return std::make_shared<float_object_t>(static_cast<double>(std::static_pointer_cast<int_object_t>(self)->get_value()) - other_float->get_value());
    }
    throw type_error_t("Unsupported operand types for -", 0, 0, 1);
}

auto int_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        return std::make_shared<int_object_t>(std::static_pointer_cast<int_object_t>(self)->get_value() * other_int->get_value());
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        return std::make_shared<float_object_t>(static_cast<double>(std::static_pointer_cast<int_object_t>(self)->get_value()) * other_float->get_value());
    }
    else if (auto other_string = std::dynamic_pointer_cast<string_object_t>(other))
    {
        // int * string: repeat the string int times
        int repeat_count = std::static_pointer_cast<int_object_t>(self)->get_value();
        if (repeat_count < 0)
        {
            return std::make_shared<string_object_t>("");
        }
        std::string result;
        for (int i = 0; i < repeat_count; ++i)
        {
            result += other_string->get_value();
        }
        return std::make_shared<string_object_t>(result);
    }
    throw type_error_t("Unsupported operand types for *", 0, 0, 1);
}

auto int_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        if (other_int->get_value() == 0)
        {
            throw zero_division_error_t("Division by zero", 0, 0, 1);
        }
        return std::make_shared<int_object_t>(std::static_pointer_cast<int_object_t>(self)->get_value() / other_int->get_value());
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        if (other_float->get_value() == 0.0)
        {
            throw zero_division_error_t("Division by zero", 0, 0, 1);
        }
        return std::make_shared<float_object_t>(static_cast<double>(std::static_pointer_cast<int_object_t>(self)->get_value()) / other_float->get_value());
    }
    throw type_error_t("Unsupported operand types for /", 0, 0, 1);
}

auto int_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        if (other_int->get_value() == 0)
        {
            throw zero_division_error_t("Modulo by zero", 0, 0, 1);
        }
        return std::make_shared<int_object_t>(std::static_pointer_cast<int_object_t>(self)->get_value() % other_int->get_value());
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        if (other_float->get_value() == 0.0)
        {
            throw zero_division_error_t("Modulo by zero", 0, 0, 1);
        }
        double left_val = static_cast<double>(std::static_pointer_cast<int_object_t>(self)->get_value());
        return std::make_shared<float_object_t>(fmod(left_val, other_float->get_value()));
    }
    throw type_error_t("Unsupported operand types for %", 0, 0, 1);
}

auto int_type_t::get_name() const -> std::string
{
    return "int";
}

auto int_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto int_obj = std::static_pointer_cast<int_object_t>(self);
    return int_obj->get_value() != 0;
}

auto int_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "int")
    {
        return false;
    }

    auto self_int = std::static_pointer_cast<int_object_t>(self);
    auto other_int = std::static_pointer_cast<int_object_t>(other);
    return self_int->get_value() == other_int->get_value();
}

auto int_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    int self_value = std::static_pointer_cast<int_object_t>(self)->get_value();

    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        int other_value = other_int->get_value();
        if (self_value < other_value) return -1;
        if (self_value > other_value) return 1;
        return 0;
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        double self_double = static_cast<double>(self_value);
        double other_value = other_float->get_value();
        if (self_double < other_value) return -1;
        if (self_double > other_value) return 1;
        return 0;
    }
    else
    {
        throw type_error_t("Cannot compare int with " + other->get_type()->get_name(), 0, 0, 1);
    }
}


} // namespace zephyr
