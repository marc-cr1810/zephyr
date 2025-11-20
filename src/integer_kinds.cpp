#include "zephyr/integer_kinds.hpp"
#include "zephyr/errors.hpp"
#include <algorithm>
#include <cstdint>

namespace zephyr
{

auto integer_kind_utils_t::kind_to_string(integer_kind_e kind) -> std::string
{
    switch (kind) {
        case integer_kind_e::I8: return "i8";
        case integer_kind_e::I16: return "i16";
        case integer_kind_e::I32: return "i32";  // DEFAULT_INT maps to this
        case integer_kind_e::I64: return "i64";
        case integer_kind_e::U8: return "u8";
        case integer_kind_e::U16: return "u16";
        case integer_kind_e::U32: return "u32";
        case integer_kind_e::U64: return "u64";
        default: return "unknown";
    }
}

auto integer_kind_utils_t::string_to_kind(const std::string& str) -> integer_kind_e
{
    if (str == "i8") return integer_kind_e::I8;
    if (str == "i16") return integer_kind_e::I16;
    if (str == "i32") return integer_kind_e::I32;
    if (str == "i64") return integer_kind_e::I64;
    if (str == "u8") return integer_kind_e::U8;
    if (str == "u16") return integer_kind_e::U16;
    if (str == "u32") return integer_kind_e::U32;
    if (str == "u64") return integer_kind_e::U64;
    if (str == "int") return integer_kind_e::I32;  // "int" maps to I32

    throw value_error_t("Unknown integer kind: " + str);
}

auto integer_kind_utils_t::is_signed(integer_kind_e kind) -> bool
{
    switch (kind) {
        case integer_kind_e::I8:
        case integer_kind_e::I16:
        case integer_kind_e::I32:
        case integer_kind_e::I64:
            return true;
        case integer_kind_e::U8:
        case integer_kind_e::U16:
        case integer_kind_e::U32:
        case integer_kind_e::U64:
            return false;
        default:
            return false;
    }
}

auto integer_kind_utils_t::is_unsigned(integer_kind_e kind) -> bool
{
    return !is_signed(kind);
}

auto integer_kind_utils_t::bit_size(integer_kind_e kind) -> int
{
    switch (kind) {
        case integer_kind_e::I8:
        case integer_kind_e::U8:
            return 8;
        case integer_kind_e::I16:
        case integer_kind_e::U16:
            return 16;
        case integer_kind_e::I32:  // DEFAULT_INT maps to this
        case integer_kind_e::U32:
            return 32;
        case integer_kind_e::I64:
        case integer_kind_e::U64:
            return 64;
        default:
            return 32;
    }
}

auto integer_kind_utils_t::byte_size(integer_kind_e kind) -> int
{
    return bit_size(kind) / 8;
}

auto integer_kind_utils_t::min_value(integer_kind_e kind) -> int64_t
{
    switch (kind) {
        case integer_kind_e::I8: return INT8_MIN;
        case integer_kind_e::I16: return INT16_MIN;
        case integer_kind_e::I32: return INT32_MIN;
        case integer_kind_e::I64: return INT64_MIN;
        case integer_kind_e::U8:
        case integer_kind_e::U16:
        case integer_kind_e::U32:
        case integer_kind_e::U64:
            return 0;
        default:
            return INT64_MIN;
    }
}

auto integer_kind_utils_t::max_value(integer_kind_e kind) -> int64_t
{
    switch (kind) {
        case integer_kind_e::I8: return INT8_MAX;
        case integer_kind_e::I16: return INT16_MAX;
        case integer_kind_e::I32: return INT32_MAX;
        case integer_kind_e::I64: return INT64_MAX;
        case integer_kind_e::U8: return UINT8_MAX;
        case integer_kind_e::U16: return UINT16_MAX;
        case integer_kind_e::U32: return UINT32_MAX;
        case integer_kind_e::U64: return INT64_MAX;
        default: return INT64_MAX;
    }
}

auto integer_kind_utils_t::value_fits(int64_t value, integer_kind_e kind) -> bool
{
    auto min_val = min_value(kind);
    auto max_val = max_value(kind);
    return value >= min_val && value <= max_val;
}

auto integer_kind_utils_t::clamp_to_range(int64_t value, integer_kind_e kind) -> int64_t
{
    auto min_val = min_value(kind);
    auto max_val = max_value(kind);
    return std::clamp(value, min_val, max_val);
}

auto integer_kind_utils_t::smallest_kind_for_value(int64_t value, bool prefer_signed) -> integer_kind_e
{
    // For negative values, must use signed type
    if (value < 0) {
        if (value >= INT8_MIN) return integer_kind_e::I8;
        if (value >= INT16_MIN) return integer_kind_e::I16;
        if (value >= INT32_MIN) return integer_kind_e::I32;
        return integer_kind_e::I64;
    }

    // For positive values, try signed first if preferred, then unsigned
    if (prefer_signed) {
        if (value <= INT8_MAX) return integer_kind_e::I8;
        if (value <= INT16_MAX) return integer_kind_e::I16;
        if (value <= INT32_MAX) return integer_kind_e::I32;
        if (value <= INT64_MAX) return integer_kind_e::I64;
    }

    // Try unsigned types
    if (value <= UINT8_MAX) return prefer_signed ? integer_kind_e::I16 : integer_kind_e::U8;
    if (value <= UINT16_MAX) return prefer_signed ? integer_kind_e::I32 : integer_kind_e::U16;
    if (value <= UINT32_MAX) return prefer_signed ? integer_kind_e::I64 : integer_kind_e::U32;

    return integer_kind_e::U64;
}

auto integer_kind_utils_t::promote_kinds(integer_kind_e left, integer_kind_e right) -> integer_kind_e
{
    // DEFAULT_INT already equals I32, so no conversion needed

    // If both are the same, no promotion needed
    if (left == right) return left;

    // Promote to the larger size first
    int left_bits = bit_size(left);
    int right_bits = bit_size(right);

    integer_kind_e larger_kind = (left_bits > right_bits) ? left :
                                 (right_bits > left_bits) ? right :
                                 // Same size: prefer unsigned for mixed signedness promotion
                                 (is_signed(left) != is_signed(right) && is_unsigned(right)) ? right : left;

    // If both are signed or both are unsigned, use the larger one
    if (is_signed(left) == is_signed(right)) {
        return (left_bits >= right_bits) ? left : right;
    }

    // Mixed signedness: promote to signed type of sufficient size
    int target_bits = std::max(left_bits, right_bits);

    // If we need more than 32 bits, go to i64
    if (target_bits > 32 || larger_kind == integer_kind_e::U32) {
        return integer_kind_e::I64;
    }

    // Otherwise, use signed version of the larger type (now correctly identifies unsigned types)
    switch (larger_kind) {
        case integer_kind_e::U8: return integer_kind_e::I16;
        case integer_kind_e::U16: return integer_kind_e::I32;
        case integer_kind_e::U32: return integer_kind_e::I64;
        default: return larger_kind;
    }
}

auto integer_kind_utils_t::can_convert_to(integer_kind_e from, integer_kind_e to) -> bool
{
    // DEFAULT_INT already equals I32, so no conversion needed

    // Same type is always convertible
    if (from == to) return true;

    int from_bits = bit_size(from);
    int to_bits = bit_size(to);

    // Can always convert to a larger type of the same signedness
    if (is_signed(from) == is_signed(to) && to_bits >= from_bits) {
        return true;
    }

    // Can convert unsigned to signed if signed type is larger
    if (is_unsigned(from) && is_signed(to) && to_bits > from_bits) {
        return true;
    }

    // Other conversions require explicit casting
    return false;
}

auto integer_kind_utils_t::parse_literal_suffix(const std::string& suffix) -> integer_kind_e
{
    if (suffix.empty()) {
        return integer_kind_e::I32;  // DEFAULT_INT equals I32
    }

    std::string lower_suffix = suffix;
    std::transform(lower_suffix.begin(), lower_suffix.end(), lower_suffix.begin(), ::tolower);

    if (lower_suffix == "i8") return integer_kind_e::I8;
    if (lower_suffix == "i16") return integer_kind_e::I16;
    if (lower_suffix == "i32") return integer_kind_e::I32;
    if (lower_suffix == "i64") return integer_kind_e::I64;
    if (lower_suffix == "u8") return integer_kind_e::U8;
    if (lower_suffix == "u16") return integer_kind_e::U16;
    if (lower_suffix == "u32") return integer_kind_e::U32;
    if (lower_suffix == "u64") return integer_kind_e::U64;

    throw value_error_t("Invalid integer literal suffix: " + suffix);
}

auto integer_kind_utils_t::to_unsigned(integer_kind_e signed_kind) -> integer_kind_e
{
    switch (signed_kind) {
        case integer_kind_e::I8: return integer_kind_e::U8;
        case integer_kind_e::I16: return integer_kind_e::U16;
        case integer_kind_e::I32: return integer_kind_e::U32;  // DEFAULT_INT maps to this
        case integer_kind_e::I64: return integer_kind_e::U64;
        default: return signed_kind; // Already unsigned or unknown
    }
}

auto integer_kind_utils_t::to_signed(integer_kind_e unsigned_kind) -> integer_kind_e
{
    switch (unsigned_kind) {
        case integer_kind_e::U8: return integer_kind_e::I8;
        case integer_kind_e::U16: return integer_kind_e::I16;
        case integer_kind_e::U32: return integer_kind_e::I32;
        case integer_kind_e::U64: return integer_kind_e::I64;
        default: return unsigned_kind; // Already signed or unknown
    }
}

auto integer_kind_utils_t::normalize_kind(integer_kind_e kind) -> integer_kind_e
{
    // DEFAULT_INT already equals I32, so just return as-is
    return kind;
}

auto integer_kind_utils_t::kinds_equivalent(integer_kind_e kind1, integer_kind_e kind2) -> bool
{
    // Since DEFAULT_INT equals I32, direct comparison works
    return kind1 == kind2;
}

auto integer_kind_utils_t::canonical_kind_name(integer_kind_e kind, bool prefer_classic_names) -> std::string
{
    switch (kind) {
        case integer_kind_e::I8: return "i8";
        case integer_kind_e::I16: return "i16";
        case integer_kind_e::I32: return prefer_classic_names ? "int" : "i32";  // DEFAULT_INT maps to this
        case integer_kind_e::I64: return "i64";
        case integer_kind_e::U8: return "u8";
        case integer_kind_e::U16: return "u16";
        case integer_kind_e::U32: return "u32";
        case integer_kind_e::U64: return "u64";
        default: return "unknown";
    }
}

} // namespace zephyr
