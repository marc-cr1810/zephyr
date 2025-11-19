#pragma once

#include "object.hpp"
#include "zephyr/integer_kinds.hpp"
#include <cstdint>

namespace zephyr
{

/**
 * int_object_t: The unified integer object type
 * 
 * This single class handles all integer types in the Zephyr language:
 * - Regular 'int' (32-bit signed, backward compatible)
 * - Sized integers: i8, i16, i32, i64, u8, u16, u32, u64
 * 
 * Internally uses int64_t for storage and tracks the specific integer kind.
 * Maintains full backward compatibility with existing int usage while
 * providing all sized integer features in one unified type.
 */
class int_object_t : public object_t
{
public:
    // ========== CONSTRUCTORS ==========
    
    // Default constructor (creates regular int with value 0)
    int_object_t();
    
    // Create regular int from int value (backward compatibility)
    explicit int_object_t(int val);
    
    // Create from int64_t with specific kind
    explicit int_object_t(int64_t val, integer_kind_e kind);
    
    // Create from int64_t (auto-detects appropriate kind)
    explicit int_object_t(int64_t val);
    
    // Create from string with optional suffix (for parsing literals)
    explicit int_object_t(const std::string& value_str, const std::string& suffix = "");
    
    // Copy constructor
    int_object_t(const int_object_t& other);
    
    // Assignment operator
    auto operator=(const int_object_t& other) -> int_object_t&;

    // ========== OBJECT INTERFACE ==========
    
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // ========== BASIC VALUE ACCESS (BACKWARD COMPATIBLE) ==========
    
    // Original int API - maintains backward compatibility
    auto value() const -> int;  // Returns int32_t value (with range checking)
    auto value(int val) -> void;  // Sets from int32_t
    
    // Extended value access
    auto value_64() const -> int64_t;  // Access full 64-bit value
    auto value_64(int64_t val) -> void;  // Set from 64-bit value (with kind validation)

    // ========== INTEGER KIND MANAGEMENT ==========
    
    auto kind() const -> integer_kind_e;
    auto kind(integer_kind_e new_kind) -> void;  // Changes kind (with value validation)
    
    // Type information
    auto is_signed() const -> bool;
    auto is_unsigned() const -> bool;
    auto bit_size() const -> int;
    auto byte_size() const -> int;
    auto type_name() const -> std::string;  // Returns "int", "i8", "u32", etc.
    
    // Check if this is a regular int (DEFAULT_INT/I32) vs sized integer
    auto is_regular_int() const -> bool;
    auto is_sized_integer() const -> bool;

    // ========== RANGE OPERATIONS ==========
    
    auto min_value() const -> int64_t;
    auto max_value() const -> int64_t;
    auto fits_in_current_kind(int64_t val) const -> bool;
    auto fits_in_kind(int64_t val, integer_kind_e target_kind) const -> bool;

    // ========== TYPE CONVERSIONS ==========
    
    // Convert to different integer kinds
    auto convert_to_kind(integer_kind_e target_kind) const -> std::shared_ptr<int_object_t>;
    auto can_convert_to_kind(integer_kind_e target_kind) const -> bool;
    
    // Conversion helpers for specific types
    auto to_int() const -> int;      // Convert to int32_t (with range checking)
    auto to_int64() const -> int64_t;  // Get as int64_t
    auto to_uint64() const -> uint64_t;  // Get as uint64_t (for unsigned types)
    
    // Cast helpers for extracting specific sized values
    auto as_i8() const -> int8_t;
    auto as_u8() const -> uint8_t;
    auto as_i16() const -> int16_t;
    auto as_u16() const -> uint16_t;
    auto as_i32() const -> int32_t;
    auto as_u32() const -> uint32_t;
    auto as_i64() const -> int64_t;
    auto as_u64() const -> uint64_t;

    // ========== SAFE ARITHMETIC OPERATIONS ==========
    
    // Arithmetic operations with overflow detection
    auto safe_add(int64_t other_value, integer_kind_e result_kind) const -> int64_t;
    auto safe_subtract(int64_t other_value, integer_kind_e result_kind) const -> int64_t;
    auto safe_multiply(int64_t other_value, integer_kind_e result_kind) const -> int64_t;
    auto safe_divide(int64_t other_value, integer_kind_e result_kind) const -> int64_t;
    auto safe_modulo(int64_t other_value, integer_kind_e result_kind) const -> int64_t;
    auto safe_negate(integer_kind_e result_kind) const -> int64_t;
    
    // Convenient arithmetic methods that maintain current kind
    auto safe_add(int64_t other_value) const -> int64_t;
    auto safe_subtract(int64_t other_value) const -> int64_t;
    auto safe_multiply(int64_t other_value) const -> int64_t;
    auto safe_divide(int64_t other_value) const -> int64_t;
    auto safe_modulo(int64_t other_value) const -> int64_t;
    auto safe_negate() const -> int64_t;

    // ========== BITWISE OPERATIONS ==========
    
    auto bitwise_and(int64_t other_value) const -> int64_t;
    auto bitwise_or(int64_t other_value) const -> int64_t;
    auto bitwise_xor(int64_t other_value) const -> int64_t;
    auto bitwise_not() const -> int64_t;
    auto left_shift(int shift_amount) const -> int64_t;
    auto right_shift(int shift_amount) const -> int64_t;

    // ========== COMPATIBILITY HELPERS ==========
    
    // Check compatibility with other integer objects
    auto is_compatible_with(std::shared_ptr<object_t> other) const -> bool;
    auto extract_integer_value(std::shared_ptr<object_t> other) const -> std::pair<int64_t, integer_kind_e>;
    
    // Type promotion helpers
    auto get_promotion_kind(integer_kind_e other_kind) const -> integer_kind_e;
    auto get_promotion_kind(std::shared_ptr<int_object_t> other) const -> integer_kind_e;

    // ========== STATIC FACTORY METHODS ==========
    
    // Create specific integer types
    static auto create_i8(int8_t value) -> std::shared_ptr<int_object_t>;
    static auto create_i16(int16_t value) -> std::shared_ptr<int_object_t>;
    static auto create_i32(int32_t value) -> std::shared_ptr<int_object_t>;
    static auto create_i64(int64_t value) -> std::shared_ptr<int_object_t>;
    static auto create_u8(uint8_t value) -> std::shared_ptr<int_object_t>;
    static auto create_u16(uint16_t value) -> std::shared_ptr<int_object_t>;
    static auto create_u32(uint32_t value) -> std::shared_ptr<int_object_t>;
    static auto create_u64(uint64_t value) -> std::shared_ptr<int_object_t>;
    
    // Create regular int (backward compatibility)
    static auto create_int(int value) -> std::shared_ptr<int_object_t>;
    
    // Smart factory that chooses best type for value
    static auto create_best_fit(int64_t value, bool prefer_signed = true) -> std::shared_ptr<int_object_t>;
    
    // Create from string with automatic type detection
    static auto from_string(const std::string& str, const std::string& suffix = "") -> std::shared_ptr<int_object_t>;

    // ========== VALIDATION AND UTILITY ==========
    
    // Validation methods
    auto validate_value() const -> void;  // Ensures value fits in current kind
    auto validate_kind(integer_kind_e target_kind) const -> void;  // Checks if current value fits in target kind
    
    // Range clamping
    auto clamp_to_current_kind() -> void;
    auto clamp_to_kind(integer_kind_e target_kind) -> int64_t;

private:
    // ========== INTERNAL STORAGE ==========
    
    int64_t m_value;              // Internal 64-bit storage
    integer_kind_e m_kind;        // Specific integer kind (i8, u32, etc.)
    
    // ========== INTERNAL HELPERS ==========
    
    // Initialization helpers
    auto init_from_int(int val) -> void;
    auto init_from_int64(int64_t val, integer_kind_e kind) -> void;
    auto init_auto_kind(int64_t val) -> void;
    
    // Validation helpers
    auto validate_value_for_kind(int64_t val, integer_kind_e kind) const -> void;
    auto clamp_value_to_kind(int64_t val, integer_kind_e kind) const -> int64_t;
    
    // Arithmetic overflow checking
    auto check_arithmetic_overflow(int64_t result, integer_kind_e result_kind, const std::string& operation) const -> void;
    
    // Bitwise operation masking
    auto apply_kind_mask(int64_t value) const -> int64_t;
    auto apply_kind_mask(int64_t value, integer_kind_e kind) const -> int64_t;
    
    // Kind management
    auto normalize_kind() const -> integer_kind_e;  // Converts DEFAULT_INT to I32
    auto is_kind_compatible(integer_kind_e from, integer_kind_e to) const -> bool;
};

// ========== UTILITY FUNCTIONS ==========

namespace int_object_utils {
    
    // Type checking
    auto is_integer_object(std::shared_ptr<object_t> obj) -> bool;
    
    // Extract information from any integer object
    auto extract_integer_info(std::shared_ptr<object_t> obj) -> std::pair<int64_t, integer_kind_e>;
    auto extract_integer_value(std::shared_ptr<object_t> obj) -> int64_t;
    auto extract_integer_kind(std::shared_ptr<object_t> obj) -> integer_kind_e;
    
    // Conversion utilities
    auto convert_to_int_object(std::shared_ptr<object_t> obj) -> std::shared_ptr<int_object_t>;
    auto convert_to_kind(std::shared_ptr<object_t> obj, integer_kind_e target_kind) -> std::shared_ptr<int_object_t>;
    
    // Compatibility checking
    auto are_arithmetic_compatible(std::shared_ptr<object_t> left, std::shared_ptr<object_t> right) -> bool;
    auto get_arithmetic_result_kind(std::shared_ptr<object_t> left, std::shared_ptr<object_t> right) -> integer_kind_e;
    
    // Range checking
    auto fits_in_kind(int64_t value, integer_kind_e kind) -> bool;
    auto get_smallest_kind_for_value(int64_t value, bool prefer_signed = true) -> integer_kind_e;
    
    // Factory helpers
    auto create_from_literal(const std::string& literal_text) -> std::shared_ptr<int_object_t>;
    auto create_from_suffix(int64_t value, const std::string& suffix) -> std::shared_ptr<int_object_t>;
}

} // namespace zephyr