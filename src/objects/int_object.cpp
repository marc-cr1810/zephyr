#include "zephyr/objects/int_object.hpp"
#include "zephyr/types/int_type.hpp"
#include "zephyr/errors.hpp"
#include <string>
#include <memory>
#include <limits>
#include <cmath>

namespace zephyr
{

// ========== CONSTRUCTORS ==========

int_object_t::int_object_t()
    : m_value(0), m_kind(integer_kind_e::DEFAULT_INT)
{
}

int_object_t::int_object_t(int val)
    : m_value(static_cast<int64_t>(val)), m_kind(integer_kind_e::DEFAULT_INT)
{
}

int_object_t::int_object_t(int64_t val, integer_kind_e kind)
    : m_value(val), m_kind(kind)
{
    validate_value();
}

int_object_t::int_object_t(int64_t val)
    : m_value(val), m_kind(integer_kind_utils_t::smallest_kind_for_value(val, true))
{
    // No validation needed since we chose the kind to fit the value
}

int_object_t::int_object_t(const std::string& value_str, const std::string& suffix)
{
    try {
        m_value = std::stoll(value_str);
        if (suffix.empty()) {
            m_kind = integer_kind_utils_t::smallest_kind_for_value(m_value, true);
        } else {
            m_kind = integer_kind_utils_t::parse_literal_suffix(suffix);
        }
        validate_value();
    } catch (const std::invalid_argument&) {
        throw value_error_t("Invalid integer literal: " + value_str);
    } catch (const std::out_of_range&) {
        throw overflow_error_t("Integer literal out of range: " + value_str);
    }
}

int_object_t::int_object_t(const int_object_t& other)
    : m_value(other.m_value), m_kind(other.m_kind)
{
}

auto int_object_t::operator=(const int_object_t& other) -> int_object_t&
{
    if (this != &other) {
        m_value = other.m_value;
        m_kind = other.m_kind;
    }
    return *this;
}

// ========== OBJECT INTERFACE ==========

auto int_object_t::type() const -> std::shared_ptr<type_t>
{
    // For regular int (DEFAULT_INT), return the classic int_type_t
    if (m_kind == integer_kind_e::DEFAULT_INT) {
        return int_type_t::get_instance();
    }
    
    // For sized integers, we still return int_type_t but we need to ensure
    // the type name is reported correctly. The type system is unified now.
    // We could create separate type objects, but for simplicity in the unified
    // system, we'll use int_type_t and let the type_name() method handle the naming.
    return int_type_t::get_instance();
}

auto int_object_t::to_string() const -> std::string
{
    return std::to_string(m_value);
}

// ========== BASIC VALUE ACCESS ==========

auto int_object_t::value() const -> int
{
    if (m_value < std::numeric_limits<int32_t>::min() || m_value > std::numeric_limits<int32_t>::max()) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " cannot fit in 32-bit int");
    }
    return static_cast<int>(m_value);
}

auto int_object_t::value(int val) -> void
{
    m_value = static_cast<int64_t>(val);
    if (m_kind != integer_kind_e::DEFAULT_INT && m_kind != integer_kind_e::I32) {
        validate_value(); // Ensure new value fits in current kind
    }
}

auto int_object_t::value_64() const -> int64_t
{
    return m_value;
}

auto int_object_t::value_64(int64_t val) -> void
{
    m_value = val;
    validate_value();
}

// ========== INTEGER KIND MANAGEMENT ==========

auto int_object_t::kind() const -> integer_kind_e
{
    return m_kind;
}

auto int_object_t::kind(integer_kind_e new_kind) -> void
{
    if (!integer_kind_utils_t::value_fits(m_value, new_kind)) {
        throw overflow_error_t("Value " + std::to_string(m_value) + 
                              " does not fit in " + integer_kind_utils_t::kind_to_string(new_kind));
    }
    m_kind = new_kind;
}

auto int_object_t::is_signed() const -> bool
{
    return integer_kind_utils_t::is_signed(m_kind);
}

auto int_object_t::is_unsigned() const -> bool
{
    return integer_kind_utils_t::is_unsigned(m_kind);
}

auto int_object_t::bit_size() const -> int
{
    return integer_kind_utils_t::bit_size(m_kind);
}

auto int_object_t::byte_size() const -> int
{
    return integer_kind_utils_t::byte_size(m_kind);
}

auto int_object_t::type_name() const -> std::string
{
    if (m_kind == integer_kind_e::DEFAULT_INT) {
        return "int";
    }
    return integer_kind_utils_t::kind_to_string(m_kind);
}

auto int_object_t::is_regular_int() const -> bool
{
    return m_kind == integer_kind_e::DEFAULT_INT || m_kind == integer_kind_e::I32;
}

auto int_object_t::is_sized_integer() const -> bool
{
    return m_kind != integer_kind_e::DEFAULT_INT;
}

// ========== RANGE OPERATIONS ==========

auto int_object_t::min_value() const -> int64_t
{
    return integer_kind_utils_t::min_value(m_kind);
}

auto int_object_t::max_value() const -> int64_t
{
    return integer_kind_utils_t::max_value(m_kind);
}

auto int_object_t::fits_in_current_kind(int64_t val) const -> bool
{
    return integer_kind_utils_t::value_fits(val, m_kind);
}

auto int_object_t::fits_in_kind(int64_t val, integer_kind_e target_kind) const -> bool
{
    return integer_kind_utils_t::value_fits(val, target_kind);
}

// ========== TYPE CONVERSIONS ==========

auto int_object_t::convert_to_kind(integer_kind_e target_kind) const -> std::shared_ptr<int_object_t>
{
    if (!integer_kind_utils_t::value_fits(m_value, target_kind)) {
        throw overflow_error_t("Cannot convert " + std::to_string(m_value) + 
                              " from " + integer_kind_utils_t::kind_to_string(m_kind) +
                              " to " + integer_kind_utils_t::kind_to_string(target_kind) +
                              " without overflow");
    }
    return std::make_shared<int_object_t>(m_value, target_kind);
}

auto int_object_t::can_convert_to_kind(integer_kind_e target_kind) const -> bool
{
    return integer_kind_utils_t::can_convert_to(m_kind, target_kind) ||
           integer_kind_utils_t::value_fits(m_value, target_kind);
}

auto int_object_t::to_int() const -> int
{
    return value(); // Uses existing range checking
}

auto int_object_t::to_int64() const -> int64_t
{
    return m_value;
}

auto int_object_t::to_uint64() const -> uint64_t
{
    if (m_value < 0) {
        throw overflow_error_t("Cannot convert negative value " + std::to_string(m_value) + " to uint64_t");
    }
    return static_cast<uint64_t>(m_value);
}

// Cast helpers for extracting specific sized values
auto int_object_t::as_i8() const -> int8_t
{
    if (m_value < -128 || m_value > 127) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " out of range for i8");
    }
    return static_cast<int8_t>(m_value);
}

auto int_object_t::as_u8() const -> uint8_t
{
    if (m_value < 0 || m_value > 255) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " out of range for u8");
    }
    return static_cast<uint8_t>(m_value);
}

auto int_object_t::as_i16() const -> int16_t
{
    if (m_value < -32768 || m_value > 32767) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " out of range for i16");
    }
    return static_cast<int16_t>(m_value);
}

auto int_object_t::as_u16() const -> uint16_t
{
    if (m_value < 0 || m_value > 65535) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " out of range for u16");
    }
    return static_cast<uint16_t>(m_value);
}

auto int_object_t::as_i32() const -> int32_t
{
    if (m_value < std::numeric_limits<int32_t>::min() || m_value > std::numeric_limits<int32_t>::max()) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " out of range for i32");
    }
    return static_cast<int32_t>(m_value);
}

auto int_object_t::as_u32() const -> uint32_t
{
    if (m_value < 0 || m_value > std::numeric_limits<uint32_t>::max()) {
        throw overflow_error_t("Value " + std::to_string(m_value) + " out of range for u32");
    }
    return static_cast<uint32_t>(m_value);
}

auto int_object_t::as_i64() const -> int64_t
{
    return m_value;
}

auto int_object_t::as_u64() const -> uint64_t
{
    return to_uint64(); // Uses existing validation
}

// ========== SAFE ARITHMETIC OPERATIONS ==========

auto int_object_t::safe_add(int64_t other_value, integer_kind_e result_kind) const -> int64_t
{
    // Check for overflow in int64_t arithmetic first
    if (m_value > 0 && other_value > 0 && m_value > INT64_MAX - other_value) {
        throw overflow_error_t("Addition overflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    if (m_value < 0 && other_value < 0 && m_value < INT64_MIN - other_value) {
        throw overflow_error_t("Addition underflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    
    int64_t result = m_value + other_value;
    check_arithmetic_overflow(result, result_kind, "addition");
    return result;
}

auto int_object_t::safe_subtract(int64_t other_value, integer_kind_e result_kind) const -> int64_t
{
    // Check for overflow in int64_t arithmetic first
    if (m_value >= 0 && other_value < 0 && m_value > INT64_MAX + other_value) {
        throw overflow_error_t("Subtraction overflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    if (m_value < 0 && other_value > 0 && m_value < INT64_MIN + other_value) {
        throw overflow_error_t("Subtraction underflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    
    int64_t result = m_value - other_value;
    check_arithmetic_overflow(result, result_kind, "subtraction");
    return result;
}

auto int_object_t::safe_multiply(int64_t other_value, integer_kind_e result_kind) const -> int64_t
{
    // Handle zero cases
    if (m_value == 0 || other_value == 0) {
        return 0;
    }
    
    // Check for overflow
    if (m_value > 0 && other_value > 0 && m_value > INT64_MAX / other_value) {
        throw overflow_error_t("Multiplication overflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    if (m_value < 0 && other_value < 0 && m_value < INT64_MAX / other_value) {
        throw overflow_error_t("Multiplication overflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    if (m_value > 0 && other_value < 0 && other_value < INT64_MIN / m_value) {
        throw overflow_error_t("Multiplication underflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    if (m_value < 0 && other_value > 0 && m_value < INT64_MIN / other_value) {
        throw overflow_error_t("Multiplication underflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    
    int64_t result = m_value * other_value;
    check_arithmetic_overflow(result, result_kind, "multiplication");
    return result;
}

auto int_object_t::safe_divide(int64_t other_value, integer_kind_e result_kind) const -> int64_t
{
    if (other_value == 0) {
        throw zero_division_error_t("Division by zero");
    }
    
    // Check for the special case of INT64_MIN / -1 which overflows
    if (m_value == INT64_MIN && other_value == -1) {
        throw overflow_error_t("Division overflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    
    int64_t result = m_value / other_value;
    check_arithmetic_overflow(result, result_kind, "division");
    return result;
}

auto int_object_t::safe_modulo(int64_t other_value, integer_kind_e result_kind) const -> int64_t
{
    if (other_value == 0) {
        throw zero_division_error_t("Modulo by zero");
    }
    
    int64_t result = m_value % other_value;
    check_arithmetic_overflow(result, result_kind, "modulo");
    return result;
}

auto int_object_t::safe_negate(integer_kind_e result_kind) const -> int64_t
{
    // Check for negation overflow (only happens with most negative value)
    if (m_value == INT64_MIN) {
        throw overflow_error_t("Negation overflow in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
    
    int64_t result = -m_value;
    check_arithmetic_overflow(result, result_kind, "negation");
    return result;
}

// Convenient versions that maintain current kind
auto int_object_t::safe_add(int64_t other_value) const -> int64_t
{
    return safe_add(other_value, m_kind);
}

auto int_object_t::safe_subtract(int64_t other_value) const -> int64_t
{
    return safe_subtract(other_value, m_kind);
}

auto int_object_t::safe_multiply(int64_t other_value) const -> int64_t
{
    return safe_multiply(other_value, m_kind);
}

auto int_object_t::safe_divide(int64_t other_value) const -> int64_t
{
    return safe_divide(other_value, m_kind);
}

auto int_object_t::safe_modulo(int64_t other_value) const -> int64_t
{
    return safe_modulo(other_value, m_kind);
}

auto int_object_t::safe_negate() const -> int64_t
{
    return safe_negate(m_kind);
}

// ========== BITWISE OPERATIONS ==========

auto int_object_t::bitwise_and(int64_t other_value) const -> int64_t
{
    int64_t result = m_value & other_value;
    return apply_kind_mask(result);
}

auto int_object_t::bitwise_or(int64_t other_value) const -> int64_t
{
    int64_t result = m_value | other_value;
    return apply_kind_mask(result);
}

auto int_object_t::bitwise_xor(int64_t other_value) const -> int64_t
{
    int64_t result = m_value ^ other_value;
    return apply_kind_mask(result);
}

auto int_object_t::bitwise_not() const -> int64_t
{
    int64_t result = ~m_value;
    return apply_kind_mask(result);
}

auto int_object_t::left_shift(int shift_amount) const -> int64_t
{
    if (shift_amount < 0 || shift_amount >= bit_size()) {
        throw value_error_t("Shift amount " + std::to_string(shift_amount) + " out of range for " + type_name());
    }
    
    int64_t result = m_value << shift_amount;
    return apply_kind_mask(result);
}

auto int_object_t::right_shift(int shift_amount) const -> int64_t
{
    if (shift_amount < 0 || shift_amount >= bit_size()) {
        throw value_error_t("Shift amount " + std::to_string(shift_amount) + " out of range for " + type_name());
    }
    
    int64_t result = m_value >> shift_amount;
    return apply_kind_mask(result);
}

// ========== COMPATIBILITY HELPERS ==========

auto int_object_t::is_compatible_with(std::shared_ptr<object_t> other) const -> bool
{
    return int_object_utils::is_integer_object(other);
}

auto int_object_t::extract_integer_value(std::shared_ptr<object_t> other) const -> std::pair<int64_t, integer_kind_e>
{
    return int_object_utils::extract_integer_info(other);
}

auto int_object_t::get_promotion_kind(integer_kind_e other_kind) const -> integer_kind_e
{
    return integer_kind_utils_t::promote_kinds(m_kind, other_kind);
}

auto int_object_t::get_promotion_kind(std::shared_ptr<int_object_t> other) const -> integer_kind_e
{
    return get_promotion_kind(other->kind());
}

// ========== STATIC FACTORY METHODS ==========

auto int_object_t::create_i8(int8_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::I8);
}

auto int_object_t::create_i16(int16_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::I16);
}

auto int_object_t::create_i32(int32_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::I32);
}

auto int_object_t::create_i64(int64_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(value, integer_kind_e::I64);
}

auto int_object_t::create_u8(uint8_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::U8);
}

auto int_object_t::create_u16(uint16_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::U16);
}

auto int_object_t::create_u32(uint32_t value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::U32);
}

auto int_object_t::create_u64(uint64_t value) -> std::shared_ptr<int_object_t>
{
    if (value > static_cast<uint64_t>(INT64_MAX)) {
        throw overflow_error_t("u64 value " + std::to_string(value) + " too large to represent");
    }
    return std::make_shared<int_object_t>(static_cast<int64_t>(value), integer_kind_e::U64);
}

auto int_object_t::create_int(int value) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(value);
}

auto int_object_t::create_best_fit(int64_t value, bool prefer_signed) -> std::shared_ptr<int_object_t>
{
    integer_kind_e best_kind = integer_kind_utils_t::smallest_kind_for_value(value, prefer_signed);
    return std::make_shared<int_object_t>(value, best_kind);
}

auto int_object_t::from_string(const std::string& str, const std::string& suffix) -> std::shared_ptr<int_object_t>
{
    return std::make_shared<int_object_t>(str, suffix);
}

// ========== VALIDATION AND UTILITY ==========

auto int_object_t::validate_value() const -> void
{
    if (!integer_kind_utils_t::value_fits(m_value, m_kind)) {
        throw overflow_error_t("Value " + std::to_string(m_value) + 
                              " does not fit in " + integer_kind_utils_t::kind_to_string(m_kind));
    }
}

auto int_object_t::validate_kind(integer_kind_e target_kind) const -> void
{
    if (!integer_kind_utils_t::value_fits(m_value, target_kind)) {
        throw overflow_error_t("Value " + std::to_string(m_value) + 
                              " does not fit in " + integer_kind_utils_t::kind_to_string(target_kind));
    }
}

auto int_object_t::clamp_to_current_kind() -> void
{
    m_value = integer_kind_utils_t::clamp_to_range(m_value, m_kind);
}

auto int_object_t::clamp_to_kind(integer_kind_e target_kind) -> int64_t
{
    return integer_kind_utils_t::clamp_to_range(m_value, target_kind);
}

// ========== PRIVATE HELPERS ==========

auto int_object_t::check_arithmetic_overflow(int64_t result, integer_kind_e result_kind, const std::string& operation) const -> void
{
    if (!integer_kind_utils_t::value_fits(result, result_kind)) {
        throw overflow_error_t("Result " + std::to_string(result) + 
                              " from " + operation + 
                              " does not fit in " + integer_kind_utils_t::kind_to_string(result_kind));
    }
}

auto int_object_t::apply_kind_mask(int64_t value) const -> int64_t
{
    return apply_kind_mask(value, m_kind);
}

auto int_object_t::apply_kind_mask(int64_t value, integer_kind_e kind) const -> int64_t
{
    switch (kind) {
        case integer_kind_e::I8:
        case integer_kind_e::U8:
            return value & 0xFF;
        case integer_kind_e::I16:
        case integer_kind_e::U16:
            return value & 0xFFFF;
        case integer_kind_e::I32:
        case integer_kind_e::U32:
            return value & 0xFFFFFFFF;
        case integer_kind_e::I64:
        case integer_kind_e::U64:
            return value; // No masking needed for 64-bit
        default:
            return value;
    }
}

auto int_object_t::normalize_kind() const -> integer_kind_e
{
    return integer_kind_utils_t::normalize_kind(m_kind);
}

auto int_object_t::is_kind_compatible(integer_kind_e from, integer_kind_e to) const -> bool
{
    return integer_kind_utils_t::can_convert_to(from, to);
}

// ========== UTILITY FUNCTIONS ==========

namespace int_object_utils {

auto is_integer_object(std::shared_ptr<object_t> obj) -> bool
{
    return std::dynamic_pointer_cast<int_object_t>(obj) != nullptr;
}

auto extract_integer_info(std::shared_ptr<object_t> obj) -> std::pair<int64_t, integer_kind_e>
{
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return {int_obj->value_64(), int_obj->kind()};
    } else {
        throw type_error_t("Object is not an integer type");
    }
}

auto extract_integer_value(std::shared_ptr<object_t> obj) -> int64_t
{
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return int_obj->value_64();
    } else {
        throw type_error_t("Object is not an integer type");
    }
}

auto extract_integer_kind(std::shared_ptr<object_t> obj) -> integer_kind_e
{
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return int_obj->kind();
    } else {
        throw type_error_t("Object is not an integer type");
    }
}

auto convert_to_int_object(std::shared_ptr<object_t> obj) -> std::shared_ptr<int_object_t>
{
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return int_obj;
    } else {
        throw type_error_t("Object cannot be converted to int_object_t");
    }
}

auto convert_to_kind(std::shared_ptr<object_t> obj, integer_kind_e target_kind) -> std::shared_ptr<int_object_t>
{
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return int_obj->convert_to_kind(target_kind);
    } else {
        throw type_error_t("Object cannot be converted to specified integer kind");
    }
}

auto are_arithmetic_compatible(std::shared_ptr<object_t> left, std::shared_ptr<object_t> right) -> bool
{
    return is_integer_object(left) && is_integer_object(right);
}

auto get_arithmetic_result_kind(std::shared_ptr<object_t> left, std::shared_ptr<object_t> right) -> integer_kind_e
{
    auto [left_val, left_kind] = extract_integer_info(left);
    auto [right_val, right_kind] = extract_integer_info(right);
    
    return integer_kind_utils_t::promote_kinds(left_kind, right_kind);
}

auto fits_in_kind(int64_t value, integer_kind_e kind) -> bool
{
    return integer_kind_utils_t::value_fits(value, kind);
}

auto get_smallest_kind_for_value(int64_t value, bool prefer_signed) -> integer_kind_e
{
    return integer_kind_utils_t::smallest_kind_for_value(value, prefer_signed);
}

auto create_from_literal(const std::string& literal_text) -> std::shared_ptr<int_object_t>
{
    return int_object_t::from_string(literal_text);
}

auto create_from_suffix(int64_t value, const std::string& suffix) -> std::shared_ptr<int_object_t>
{
    integer_kind_e kind = integer_kind_utils_t::parse_literal_suffix(suffix);
    return std::make_shared<int_object_t>(value, kind);
}

} // namespace int_object_utils

} // namespace zephyr