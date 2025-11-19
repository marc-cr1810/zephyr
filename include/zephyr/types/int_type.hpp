#pragma once

#include "type.hpp"
#include "zephyr/integer_kinds.hpp"
#include <functional>

namespace zephyr
{

// Forward declarations

/**
 * int_type_t: The classic integer type that now uses the unified integer system
 * 
 * This type maintains full backward compatibility while internally leveraging
 * the unified integer system. It represents 32-bit signed integers and can
 * seamlessly interoperate with other sized integer types.
 * 
 * Key features:
 * - Maintains the same public API as before
 * - Internally uses unified int_object_t with appropriate integer kind
 * - Supports arithmetic with other integer types (all handled by unified system)
 * - Automatic type promotion when working with different integer sizes
 * - Backward compatibility with all existing int-based code
 */
class int_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    // Arithmetic operations - now support mixed int/sized_int arithmetic
    auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto power(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    // Unary operations
    auto negate(std::shared_ptr<object_t> self) -> std::shared_ptr<object_t> override;

    // Comparison and truthiness
    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int override;

    // Type information - matches sized integer interface
    auto is_signed() const -> bool;        // Always returns true for int
    auto is_unsigned() const -> bool;      // Always returns false for int
    auto bit_size() const -> int;          // Always returns 32 for int
    auto byte_size() const -> int;         // Always returns 4 for int
    auto integer_kind() const -> integer_kind_e;  // Returns I32/DEFAULT_INT

    // Compatibility methods for working with sized integers
    auto is_compatible_with_sized_int() const -> bool;  // Always returns true
    auto can_promote_with(std::shared_ptr<object_t> other) const -> bool;
    
    // Factory method
    static auto get_instance() -> std::shared_ptr<int_type_t>;

private:
    int_type_t() = default;

    static std::shared_ptr<int_type_t> s_instance;

    // Helper methods for mixed arithmetic
    auto perform_mixed_arithmetic(
        std::shared_ptr<object_t> self, 
        std::shared_ptr<object_t> other,
        const std::string& operation_name,
        std::function<int64_t(int64_t, int64_t)> operation
    ) -> std::shared_ptr<object_t>;
    
    // Helper methods for type promotion and result creation
    auto determine_result_type(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) const -> std::shared_ptr<type_t>;
    auto create_result_object(int64_t value, std::shared_ptr<type_t> result_type) const -> std::shared_ptr<object_t>;
    
    // Helper to extract integer value from any integer object type
    auto extract_integer_value(std::shared_ptr<object_t> obj) const -> std::pair<int64_t, integer_kind_e>;
    auto is_integer_compatible(std::shared_ptr<object_t> obj) const -> bool;
    
    // Legacy helper methods (still used for pure int-int operations)
    auto check_division_by_zero(std::shared_ptr<object_t> other) const -> void;
    auto perform_integer_division(int64_t left, int64_t right) const -> std::shared_ptr<object_t>;
    auto perform_integer_modulo(int64_t left, int64_t right) const -> std::shared_ptr<object_t>;
    auto perform_integer_power(int64_t base, int64_t exponent) const -> std::shared_ptr<object_t>;
};

}
