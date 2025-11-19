#pragma once

#include <cstdint>
#include <string>
#include <limits>

namespace zephyr
{

enum class integer_kind_e
{
    I8,   // signed 8-bit: -128 to 127
    I16,  // signed 16-bit: -32,768 to 32,767
    I32,  // signed 32-bit: -2,147,483,648 to 2,147,483,647
    I64,  // signed 64-bit: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
    U8,   // unsigned 8-bit: 0 to 255
    U16,  // unsigned 16-bit: 0 to 65,535
    U32,  // unsigned 32-bit: 0 to 4,294,967,295
    U64,  // unsigned 64-bit: 0 to 18,446,744,073,709,551,615
    DEFAULT_INT = I32  // Backward compatibility: regular 'int' maps to I32
};

class integer_kind_utils_t
{
public:
    // Get the name of an integer kind (for type system)
    static auto kind_to_string(integer_kind_e kind) -> std::string;
    
    // Parse string to integer kind (for type annotations)
    static auto string_to_kind(const std::string& str) -> integer_kind_e;
    
    // Check if a kind is signed
    static auto is_signed(integer_kind_e kind) -> bool;
    
    // Check if a kind is unsigned  
    static auto is_unsigned(integer_kind_e kind) -> bool;
    
    // Get bit size of a kind
    static auto bit_size(integer_kind_e kind) -> int;
    
    // Get byte size of a kind
    static auto byte_size(integer_kind_e kind) -> int;
    
    // Check if value fits in the range for this kind
    static auto value_fits(int64_t value, integer_kind_e kind) -> bool;
    
    // Get minimum value for this kind
    static auto min_value(integer_kind_e kind) -> int64_t;
    
    // Get maximum value for this kind  
    static auto max_value(integer_kind_e kind) -> int64_t;
    
    // Clamp value to valid range for kind
    static auto clamp_to_range(int64_t value, integer_kind_e kind) -> int64_t;
    
    // Determine the smallest kind that can hold a value
    static auto smallest_kind_for_value(int64_t value, bool prefer_signed = true) -> integer_kind_e;
    
    // Get promotion result when combining two kinds in arithmetic
    static auto promote_kinds(integer_kind_e left, integer_kind_e right) -> integer_kind_e;
    
    // Check if one kind can be implicitly converted to another
    static auto can_convert_to(integer_kind_e from, integer_kind_e to) -> bool;
    
    // Parse literal suffix to determine kind (e.g., "u64" -> U64)
    static auto parse_literal_suffix(const std::string& suffix) -> integer_kind_e;
    
    // Normalize DEFAULT_INT to I32 for consistency
    static auto normalize_kind(integer_kind_e kind) -> integer_kind_e;
    
    // Check if two kinds are equivalent (handles DEFAULT_INT == I32)
    static auto kinds_equivalent(integer_kind_e kind1, integer_kind_e kind2) -> bool;
    
    // Get the canonical form of a kind name (DEFAULT_INT -> "int", I32 -> "i32")
    static auto canonical_kind_name(integer_kind_e kind, bool prefer_classic_names = false) -> std::string;
    
private:
    // Helper to get unsigned equivalent of signed kind
    static auto to_unsigned(integer_kind_e signed_kind) -> integer_kind_e;
    
    // Helper to get signed equivalent of unsigned kind
    static auto to_signed(integer_kind_e unsigned_kind) -> integer_kind_e;
};

// Convenience functions for common operations
inline auto is_signed_kind(integer_kind_e kind) -> bool 
{
    return integer_kind_utils_t::is_signed(kind);
}

inline auto is_unsigned_kind(integer_kind_e kind) -> bool 
{
    return integer_kind_utils_t::is_unsigned(kind);
}

inline auto kind_name(integer_kind_e kind) -> std::string
{
    return integer_kind_utils_t::kind_to_string(kind);
}

// Convenience function for normalized kinds
inline auto normalize_kind(integer_kind_e kind) -> integer_kind_e
{
    return integer_kind_utils_t::normalize_kind(kind);
}

// Convenience function to check kind equivalence
inline auto kinds_equivalent(integer_kind_e kind1, integer_kind_e kind2) -> bool
{
    return integer_kind_utils_t::kinds_equivalent(kind1, kind2);
}

} // namespace zephyr

// Hash specialization for integer_kind_e to enable use in unordered_map
namespace std {
    template<>
    struct hash<zephyr::integer_kind_e> {
        std::size_t operator()(const zephyr::integer_kind_e& k) const {
            return std::hash<int>{}(static_cast<int>(k));
        }
    };
}