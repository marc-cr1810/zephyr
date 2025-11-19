#include "zephyr/types/int_type.hpp"
#include "zephyr/objects/objects.hpp"
#include "zephyr/errors.hpp"
#include <cmath>

namespace zephyr
{

std::shared_ptr<int_type_t> int_type_t::s_instance = nullptr;

auto int_type_t::get_instance() -> std::shared_ptr<int_type_t>
{
    if (!s_instance) {
        s_instance = std::shared_ptr<int_type_t>(new int_type_t());
    }
    return s_instance;
}

auto int_type_t::name() const -> std::string
{
    return "int";
}

auto int_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return perform_mixed_arithmetic(self, other, "addition", 
        [](int64_t left, int64_t right) { return left + right; });
}

auto int_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return perform_mixed_arithmetic(self, other, "subtraction",
        [](int64_t left, int64_t right) { return left - right; });
}

auto int_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    // Handle string multiplication (int * string)
    if (auto other_string = std::dynamic_pointer_cast<string_object_t>(other)) {
        int repeat_count = std::static_pointer_cast<int_object_t>(self)->value();
        if (repeat_count < 0) {
            return std::make_shared<string_object_t>("");
        }
        std::string result;
        for (int i = 0; i < repeat_count; ++i) {
            result += other_string->value();
        }
        return std::make_shared<string_object_t>(result);
    }
    
    return perform_mixed_arithmetic(self, other, "multiplication",
        [](int64_t left, int64_t right) { return left * right; });
}

auto int_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    // Handle float division - always promote to float for division
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other)) {
        if (other_float->value() == 0.0) {
            throw zero_division_error_t("Division by zero");
        }
        auto self_int = std::static_pointer_cast<int_object_t>(self);
        return std::make_shared<float_object_t>(static_cast<double>(self_int->value()) / other_float->value());
    }
    
    // For integer division, check if we need to promote to float
    if (is_integer_compatible(other)) {
        auto [self_val, self_kind] = extract_integer_value(self);
        auto [other_val, other_kind] = extract_integer_value(other);
        
        if (other_val == 0) {
            throw zero_division_error_t("Division by zero");
        }
        
        // Check if division results in a whole number
        if (self_val % other_val == 0) {
            // Integer division with no remainder - keep as integer
            int64_t result = self_val / other_val;
            auto result_type = determine_result_type(self, other);
            return create_result_object(result, result_type);
        } else {
            // Fractional result - promote to float
            return std::make_shared<float_object_t>(static_cast<double>(self_val) / static_cast<double>(other_val));
        }
    }
    
    throw type_error_t("Unsupported operand types for /: int and " + other->type()->name());
}

auto int_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    // Handle float modulo
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other)) {
        if (other_float->value() == 0.0) {
            throw zero_division_error_t("Modulo by zero");
        }
        auto self_int = std::static_pointer_cast<int_object_t>(self);
        double left_val = static_cast<double>(self_int->value());
        return std::make_shared<float_object_t>(fmod(left_val, other_float->value()));
    }
    
    return perform_mixed_arithmetic(self, other, "modulo",
        [](int64_t left, int64_t right) {
            if (right == 0) {
                throw zero_division_error_t("Modulo by zero");
            }
            return left % right;
        });
}

/**
 * Power operation for integer types with proper type promotion.
 * 
 * This implementation ensures that integer power operations promote to larger
 * integer types when needed, rather than incorrectly promoting to float for
 * overflow cases. The promotion hierarchy is:
 * 
 * 1. Negative exponents → float (mathematically correct for fractional results)
 * 2. Results fitting in int32 → int (backward compatibility)
 * 3. Large integer results → i64 (preserves integer semantics)
 * 4. Results too large for int64 → float (mathematical necessity)
 * 
 * This fixes the previous behavior where large integer results like 10^12
 * would incorrectly become float instead of i64.
 */
auto int_type_t::power(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    auto self_int = std::static_pointer_cast<int_object_t>(self);
    
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other)) {
        int64_t base = self_int->value_64();
        int64_t exponent = other_int->value_64();
        
        // Handle negative exponents by promoting to float
        if (exponent < 0) {
            return std::make_shared<float_object_t>(std::pow(static_cast<double>(base), static_cast<double>(exponent)));
        }
        
        // Calculate power using double precision to avoid overflow during calculation
        double result_double = std::pow(static_cast<double>(base), static_cast<double>(exponent));
        
        // Check if result is too large for int64_t or has fractional part
        if (result_double > static_cast<double>(std::numeric_limits<int64_t>::max()) || 
            result_double != std::floor(result_double)) {
            return std::make_shared<float_object_t>(result_double);
        }
        
        int64_t result = static_cast<int64_t>(result_double);
        
        // Check if result fits in int32, otherwise promote to i64 (not float!)
        if (result >= std::numeric_limits<int32_t>::min() && result <= std::numeric_limits<int32_t>::max()) {
            return std::make_shared<int_object_t>(static_cast<int>(result));
        } else {
            return int_object_t::create_i64(result);  // Promote to i64, preserving integer semantics
        }
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other)) {
        return std::make_shared<float_object_t>(std::pow(static_cast<double>(self_int->value()), other_float->value()));
    }

    
    throw type_error_t("Unsupported operand type for power: " + other->type()->name());
}

auto int_type_t::negate(std::shared_ptr<object_t> self) -> std::shared_ptr<object_t>
{
    auto int_obj = std::static_pointer_cast<int_object_t>(self);
    int64_t value = int_obj->value_64();
    
    // Use the unified integer's safe negate method
    try {
        int64_t result = int_obj->safe_negate();
        
        // If original was regular int and result fits, keep as regular int
        if (int_obj->kind() == integer_kind_e::DEFAULT_INT && 
            result >= std::numeric_limits<int32_t>::min() && 
            result <= std::numeric_limits<int32_t>::max()) {
            return std::make_shared<int_object_t>(static_cast<int>(result));
        }
        
        // Otherwise create with appropriate kind
        return std::make_shared<int_object_t>(result, int_obj->kind());
    } catch (const overflow_error_t&) {
        // If overflow occurs, promote to i64
        return int_object_t::create_i64(-value);
    }
}

auto int_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto int_obj = std::static_pointer_cast<int_object_t>(self);
    return int_obj->value() != 0;
}

auto int_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    auto self_int = std::static_pointer_cast<int_object_t>(self);
    int64_t self_value = self_int->value_64();
    
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other)) {
        return self_value == other_int->value_64();
    }
    
    return false;
}

auto int_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    auto self_int = std::static_pointer_cast<int_object_t>(self);
    int64_t self_value = self_int->value_64();

    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other)) {
        int64_t other_value = other_int->value_64();
        if (self_value < other_value) return -1;
        if (self_value > other_value) return 1;
        return 0;
    }
    else if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other)) {
        double self_double = static_cast<double>(self_value);
        double other_value = other_float->value();
        if (self_double < other_value) return -1;
        if (self_double > other_value) return 1;
        return 0;
    }
    
    throw type_error_t("Cannot compare int with " + other->type()->name());
}

// Type information methods
auto int_type_t::is_signed() const -> bool
{
    return true;
}

auto int_type_t::is_unsigned() const -> bool
{
    return false;
}

auto int_type_t::bit_size() const -> int
{
    return 32;
}

auto int_type_t::byte_size() const -> int
{
    return 4;
}

auto int_type_t::integer_kind() const -> integer_kind_e
{
    return integer_kind_e::DEFAULT_INT;  // Maps to I32 but keeps identity as DEFAULT_INT
}

// Compatibility methods
auto int_type_t::is_compatible_with_sized_int() const -> bool
{
    return true;
}

auto int_type_t::can_promote_with(std::shared_ptr<object_t> other) const -> bool
{
    return is_integer_compatible(other);
}

// Private helper methods
auto int_type_t::perform_mixed_arithmetic(
    std::shared_ptr<object_t> self, 
    std::shared_ptr<object_t> other,
    const std::string& operation_name,
    std::function<int64_t(int64_t, int64_t)> operation
) -> std::shared_ptr<object_t>
{
    // Handle float promotion
    if (auto other_float = std::dynamic_pointer_cast<float_object_t>(other)) {
        auto self_int = std::static_pointer_cast<int_object_t>(self);
        double left_val = static_cast<double>(self_int->value());
        double right_val = other_float->value();
        
        // Perform operation in floating point
        double result = operation(static_cast<int64_t>(left_val), static_cast<int64_t>(right_val));
        return std::make_shared<float_object_t>(result);
    }
    
    // Handle integer arithmetic (int + int or int + sized_int)
    if (is_integer_compatible(other)) {
        auto [self_val, self_kind] = extract_integer_value(self);
        auto [other_val, other_kind] = extract_integer_value(other);
        
        try {
            int64_t result = operation(self_val, other_val);
            auto result_type = determine_result_type(self, other);
            return create_result_object(result, result_type);
        } catch (const std::exception& e) {
            throw overflow_error_t("Arithmetic overflow in " + operation_name + ": " + e.what());
        }
    }
    
    throw type_error_t("Unsupported operand types for " + operation_name + ": int and " + other->type()->name());
}

auto int_type_t::determine_result_type(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) const -> std::shared_ptr<type_t>
{
    // If other is a different integer kind, use type promotion rules
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other)) {
        integer_kind_e other_kind = other_int->kind();
        integer_kind_e result_kind = integer_kind_utils_t::promote_kinds(integer_kind_e::DEFAULT_INT, other_kind);
        
        // Always return int_type_t since we have unified the system
        // The actual result object will have the correct kind
        return get_instance();
    }
    
    // Default to int type for all integer operations
    return get_instance();
}

auto int_type_t::create_result_object(int64_t value, std::shared_ptr<type_t> result_type) const -> std::shared_ptr<object_t>
{
    // Always create int_object_t since we have a unified system
    // Determine the appropriate kind based on the value and context
    integer_kind_e target_kind = integer_kind_e::DEFAULT_INT;
    
    // Check if value fits in regular int
    if (value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max()) {
        target_kind = integer_kind_e::DEFAULT_INT;
    } else {
        // Use i64 for larger values
        target_kind = integer_kind_e::I64;
    }
    
    return std::make_shared<int_object_t>(value, target_kind);
}

auto int_type_t::extract_integer_value(std::shared_ptr<object_t> obj) const -> std::pair<int64_t, integer_kind_e>
{
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return {int_obj->value_64(), int_obj->kind()};
    }
    
    throw type_error_t("Object is not an integer type");
}

auto int_type_t::is_integer_compatible(std::shared_ptr<object_t> obj) const -> bool
{
    return std::dynamic_pointer_cast<int_object_t>(obj) != nullptr;
}

auto int_type_t::check_division_by_zero(std::shared_ptr<object_t> other) const -> void
{
    if (auto other_int = std::dynamic_pointer_cast<int_object_t>(other)) {
        if (other_int->value_64() == 0) {
            throw zero_division_error_t("Division by zero");
        }
    }
}

auto int_type_t::perform_integer_division(int64_t left, int64_t right) const -> std::shared_ptr<object_t>
{
    if (right == 0) {
        throw zero_division_error_t("Division by zero");
    }
    
    int64_t result = left / right;
    
    // Keep result as int if it fits
    if (result >= std::numeric_limits<int32_t>::min() && result <= std::numeric_limits<int32_t>::max()) {
        return std::make_shared<int_object_t>(static_cast<int>(result));
    } else {
        return int_object_t::create_i64(result);
    }
}

auto int_type_t::perform_integer_modulo(int64_t left, int64_t right) const -> std::shared_ptr<object_t>
{
    if (right == 0) {
        throw zero_division_error_t("Modulo by zero");
    }
    
    int64_t result = left % right;
    
    // Keep result as int if it fits
    if (result >= std::numeric_limits<int32_t>::min() && result <= std::numeric_limits<int32_t>::max()) {
        return std::make_shared<int_object_t>(static_cast<int>(result));
    } else {
        return int_object_t::create_i64(result);
    }
}

auto int_type_t::perform_integer_power(int64_t base, int64_t exponent) const -> std::shared_ptr<object_t>
{
    if (exponent < 0) {
        // Negative exponent - return float
        return std::make_shared<float_object_t>(std::pow(static_cast<double>(base), static_cast<double>(exponent)));
    }
    
    int64_t result = static_cast<int64_t>(std::pow(base, exponent));
    
    // Keep result as int if it fits
    if (result >= std::numeric_limits<int32_t>::min() && result <= std::numeric_limits<int32_t>::max()) {
        return std::make_shared<int_object_t>(static_cast<int>(result));
    } else {
        return int_object_t::create_i64(result);
    }
}

} // namespace zephyr