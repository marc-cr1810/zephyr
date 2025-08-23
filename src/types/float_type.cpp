#include "types/float_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"
#include <cmath>

namespace zephyr
{

auto float_type_t::get_instance() -> std::shared_ptr<float_type_t>
{
    static auto instance = std::shared_ptr<float_type_t>(new float_type_t());
    return instance;
}

auto float_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() + other_float->get_value());
    }
    else if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() + static_cast<double>(other_int->get_value()));
    }
    throw type_error_t("Unsupported operand types for +", 0, 0, 1);
}

auto float_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() - other_float->get_value());
    }
    else if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() - static_cast<double>(other_int->get_value()));
    }
    throw type_error_t("Unsupported operand types for -", 0, 0, 1);
}

auto float_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() * other_float->get_value());
    }
    else if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() * static_cast<double>(other_int->get_value()));
    }
    throw type_error_t("Unsupported operand types for *", 0, 0, 1);
}

auto float_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        if (other_float->get_value() == 0.0)
        {
            throw zero_division_error_t("Division by zero", 0, 0, 1);
        }
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() / other_float->get_value());
    }
    else if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        if (other_int->get_value() == 0)
        {
            throw zero_division_error_t("Division by zero", 0, 0, 1);
        }
        return std::make_shared<float_object_t>(std::static_pointer_cast<float_object_t>(self)->get_value() / static_cast<double>(other_int->get_value()));
    }
    throw type_error_t("Unsupported operand types for /", 0, 0, 1);
}

auto float_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        if (other_float->get_value() == 0.0)
        {
            throw zero_division_error_t("Modulo by zero", 0, 0, 1);
        }
        return std::make_shared<float_object_t>(fmod(std::static_pointer_cast<float_object_t>(self)->get_value(), other_float->get_value()));
    }
    else if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        if (other_int->get_value() == 0)
        {
            throw zero_division_error_t("Modulo by zero", 0, 0, 1);
        }
        return std::make_shared<float_object_t>(fmod(std::static_pointer_cast<float_object_t>(self)->get_value(), static_cast<double>(other_int->get_value())));
    }
    throw type_error_t("Unsupported operand types for %", 0, 0, 1);
}

auto float_type_t::get_name() const -> std::string
{
    return "float";
}

auto float_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto float_obj = std::static_pointer_cast<float_object_t>(self);
    return float_obj->get_value() != 0.0;
}

auto float_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "float")
    {
        return false;
    }

    auto self_float = std::static_pointer_cast<float_object_t>(self);
    auto other_float = std::static_pointer_cast<float_object_t>(other);
    return self_float->get_value() == other_float->get_value();
}

auto float_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    double self_value = std::static_pointer_cast<float_object_t>(self)->get_value();
    double other_value = 0.0;

    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other))
    {
        other_value = other_float->get_value();
    }
    else if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other))
    {
        other_value = static_cast<double>(other_int->get_value());
    }
    else
    {
        throw type_error_t("Cannot compare float with " + other->get_type()->get_name(), 0, 0, 1);
    }

    if (self_value < other_value) return -1;
    if (self_value > other_value) return 1;
    return 0;
}


} // namespace zephyr
