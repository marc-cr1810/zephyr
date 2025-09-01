#include "objects/float_object.hpp"
#include "objects/int_object.hpp"
#include "types/float_type.hpp"
#include <string>
#include <sstream>
#include <iomanip>
#include "errors.hpp"
#include <cmath> // Added for std::pow

namespace zephyr
{

float_object_t::float_object_t(double val)
    : m_value(val)
{
}

auto float_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return float_type_t::get_instance();
}

auto float_object_t::to_string() const -> std::string
{
    return format_float_string();
}

auto float_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto float_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto float_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto float_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto float_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto float_object_t::power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->power(shared_from_this(), other);
}

auto float_object_t::is_truthy() const -> bool
{
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
}

auto float_object_t::get_value() const -> double
{
    return m_value;
}

auto float_object_t::set_value(double val) -> void
{
    m_value = val;
}



auto float_object_t::format_float_string() const -> std::string
{
    // Check if the float is actually a whole number
    if (m_value == static_cast<long long>(m_value))
    {
        // If it's a whole number, display without decimal places
        return std::to_string(static_cast<long long>(m_value)) + ".0";
    }
    else
    {
        // Use stringstream for better formatting control
        std::ostringstream oss;
        oss << std::setprecision(15) << m_value;
        std::string result = oss.str();
        
        // Remove trailing zeros after decimal point
        if (result.find('.') != std::string::npos)
        {
            result = result.substr(0, result.find_last_not_of('0') + 1);
            if (result.back() == '.')
            {
                result += '0';
            }
        }
        
        return result;
    }
}

} // namespace zephyr