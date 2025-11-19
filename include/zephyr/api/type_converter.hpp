#pragma once

#include "zephyr/objects/object.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/boolean_object.hpp"
#include "zephyr/objects/list_object.hpp"
#include "zephyr/objects/dictionary_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/api/result.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <optional>
#include <type_traits>
#include <stdexcept>
#include <cstdint>

namespace zephyr::api {

// Forward declarations
using value_t = std::shared_ptr<object_t>;

// Type trait to check if a type can be converted
template<typename T>
struct is_convertible_to_zephyr : std::false_type {};

template<typename T>
struct is_convertible_from_zephyr : std::false_type {};

// Primary conversion functions
template<typename T>
auto to_zephyr(const T& value) -> value_t;

template<typename T>
auto from_zephyr(const value_t& value) -> result_t<T>;

// Safe conversion (returns optional instead of throwing)
template<typename T>
auto try_from_zephyr(const value_t& value) -> std::optional<T>;

// Type checking utilities
template<typename T>
auto is_zephyr_type(const value_t& value) -> bool;

auto get_zephyr_type_name(const value_t& value) -> std::string;

// =============================================================================
// Fundamental Type Specializations
// =============================================================================

// Integer types
template<>
struct is_convertible_to_zephyr<int> : std::true_type {};
template<>
struct is_convertible_from_zephyr<int> : std::true_type {};

template<>
inline auto to_zephyr<int>(const int& value) -> value_t {
    return std::make_shared<int_object_t>(value);
}

template<>
inline auto from_zephyr<int>(const value_t& value) -> result_t<int> {
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        return result_t<int>::success(int_obj->value());
    }
    return result_t<int>::error("Expected integer, got " + get_zephyr_type_name(value));
}

template<>
inline auto is_zephyr_type<int>(const value_t& value) -> bool {
    return std::dynamic_pointer_cast<int_object_t>(value) != nullptr;
}

// Long types
template<>
struct is_convertible_to_zephyr<long> : std::true_type {};
template<>
struct is_convertible_from_zephyr<long> : std::true_type {};

template<>
inline auto to_zephyr<long>(const long& value) -> value_t {
    return std::make_shared<int_object_t>(static_cast<int>(value));
}

template<>
inline auto from_zephyr<long>(const value_t& value) -> result_t<long> {
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        return result_t<long>::success(static_cast<long>(int_obj->value()));
    }
    return result_t<long>::error("Expected integer, got " + get_zephyr_type_name(value));
}

// Size_t types
template<>
struct is_convertible_to_zephyr<size_t> : std::true_type {};
template<>
struct is_convertible_from_zephyr<size_t> : std::true_type {};

template<>
inline auto to_zephyr<size_t>(const size_t& value) -> value_t {
    return std::make_shared<int_object_t>(static_cast<int>(value));
}

template<>
inline auto from_zephyr<size_t>(const value_t& value) -> result_t<size_t> {
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        auto val = int_obj->value();
        if (val < 0) {
            return result_t<size_t>::error("Cannot convert negative integer to size_t");
        }
        return result_t<size_t>::success(static_cast<size_t>(val));
    }
    return result_t<size_t>::error("Expected integer, got " + get_zephyr_type_name(value));
}

// Float/Double types
template<>
struct is_convertible_to_zephyr<float> : std::true_type {};
template<>
struct is_convertible_from_zephyr<float> : std::true_type {};

template<>
inline auto to_zephyr<float>(const float& value) -> value_t {
    return std::make_shared<float_object_t>(static_cast<double>(value));
}

template<>
inline auto from_zephyr<float>(const value_t& value) -> result_t<float> {
    if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(value)) {
        return result_t<float>::success(static_cast<float>(float_obj->value()));
    }
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        return result_t<float>::success(static_cast<float>(int_obj->value()));
    }
    return result_t<float>::error("Expected number, got " + get_zephyr_type_name(value));
}

template<>
struct is_convertible_to_zephyr<double> : std::true_type {};
template<>
struct is_convertible_from_zephyr<double> : std::true_type {};

template<>
inline auto to_zephyr<double>(const double& value) -> value_t {
    return std::make_shared<float_object_t>(value);
}

template<>
inline auto from_zephyr<double>(const value_t& value) -> result_t<double> {
    if (auto float_obj = std::dynamic_pointer_cast<float_object_t>(value)) {
        return result_t<double>::success(float_obj->value());
    }
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        return result_t<double>::success(static_cast<double>(int_obj->value()));
    }
    return result_t<double>::error("Expected number, got " + get_zephyr_type_name(value));
}

template<>
inline auto is_zephyr_type<double>(const value_t& value) -> bool {
    return std::dynamic_pointer_cast<float_object_t>(value) != nullptr ||
           std::dynamic_pointer_cast<int_object_t>(value) != nullptr;
}

// String types
template<>
struct is_convertible_to_zephyr<std::string> : std::true_type {};
template<>
struct is_convertible_from_zephyr<std::string> : std::true_type {};

template<>
inline auto to_zephyr<std::string>(const std::string& value) -> value_t {
    return std::make_shared<string_object_t>(value);
}

template<>
inline auto from_zephyr<std::string>(const value_t& value) -> result_t<std::string> {
    if (auto str_obj = std::dynamic_pointer_cast<string_object_t>(value)) {
        return result_t<std::string>::success(str_obj->value());
    }
    return result_t<std::string>::error("Expected string, got " + get_zephyr_type_name(value));
}

template<>
inline auto is_zephyr_type<std::string>(const value_t& value) -> bool {
    return std::dynamic_pointer_cast<string_object_t>(value) != nullptr;
}

// C-string support
template<>
struct is_convertible_to_zephyr<const char*> : std::true_type {};

template<>
inline auto to_zephyr<const char*>(const char* const& value) -> value_t {
    return std::make_shared<string_object_t>(std::string(value));
}

// Boolean types
template<>
struct is_convertible_to_zephyr<bool> : std::true_type {};
template<>
struct is_convertible_from_zephyr<bool> : std::true_type {};

template<>
inline auto to_zephyr<bool>(const bool& value) -> value_t {
    return std::make_shared<boolean_object_t>(value);
}

template<>
inline auto from_zephyr<bool>(const value_t& value) -> result_t<bool> {
    if (auto bool_obj = std::dynamic_pointer_cast<boolean_object_t>(value)) {
        return result_t<bool>::success(bool_obj->m_value);
    }
    // In Zephyr, use truthiness for conversion
    return result_t<bool>::success(value->is_truthy());
}

template<>
inline auto is_zephyr_type<bool>(const value_t& value) -> bool {
    return std::dynamic_pointer_cast<boolean_object_t>(value) != nullptr;
}

// =============================================================================
// Container Type Specializations
// =============================================================================

// Vector/List types
template<typename T>
struct is_convertible_to_zephyr<std::vector<T>> : is_convertible_to_zephyr<T> {};

template<typename T>
struct is_convertible_from_zephyr<std::vector<T>> : is_convertible_from_zephyr<T> {};

template<typename T>
inline auto to_zephyr(const std::vector<T>& value) -> value_t {
    auto list_obj = std::make_shared<list_object_t>();
    std::vector<value_t> elements;
    for (const auto& item : value) {
        elements.push_back(to_zephyr(item));
    }
    list_obj->elements(elements);
    return list_obj;
}

template<typename T>
inline auto from_zephyr_vector(const value_t& value) -> result_t<std::vector<T>> {
    if (auto list_obj = std::dynamic_pointer_cast<list_object_t>(value)) {
        std::vector<T> result;
        result.reserve(list_obj->elements().size());
        
        for (size_t i = 0; i < list_obj->elements().size(); ++i) {
            auto item_result = from_zephyr<T>(list_obj->elements()[i]);
            if (!item_result) {
                return result_t<std::vector<T>>::error(
                    "Failed to convert list item at index " + std::to_string(i) + 
                    ": " + item_result.error()
                );
            }
            result.push_back(item_result.value());
        }
        
        return result_t<std::vector<T>>::success(std::move(result));
    }
    return result_t<std::vector<T>>::error("Expected list, got " + get_zephyr_type_name(value));
}

// Add specific overloads for common vector types
template<>
inline auto from_zephyr<std::vector<int>>(const value_t& value) -> result_t<std::vector<int>> {
    return from_zephyr_vector<int>(value);
}

template<>
inline auto from_zephyr<std::vector<double>>(const value_t& value) -> result_t<std::vector<double>> {
    return from_zephyr_vector<double>(value);
}

template<>
inline auto from_zephyr<std::vector<std::string>>(const value_t& value) -> result_t<std::vector<std::string>> {
    return from_zephyr_vector<std::string>(value);
}

// Specialization for vector<value_t> (no conversion needed)
template<>
inline auto to_zephyr<std::vector<value_t>>(const std::vector<value_t>& value) -> value_t {
    auto list_obj = std::make_shared<list_object_t>();
    list_obj->elements(value);
    return list_obj;
}

template<>
inline auto from_zephyr<std::vector<value_t>>(const value_t& value) -> result_t<std::vector<value_t>> {
    if (auto list_obj = std::dynamic_pointer_cast<list_object_t>(value)) {
        return result_t<std::vector<value_t>>::success(list_obj->elements());
    }
    return result_t<std::vector<value_t>>::error("Expected list, got " + get_zephyr_type_name(value));
}

// Map/Dictionary types
template<typename V>
struct is_convertible_to_zephyr<std::map<std::string, V>> : is_convertible_to_zephyr<V> {};

template<typename V>
struct is_convertible_from_zephyr<std::map<std::string, V>> : is_convertible_from_zephyr<V> {};

template<typename V>
inline auto to_zephyr(const std::map<std::string, V>& value) -> value_t {
    auto dict_obj = std::make_shared<dictionary_object_t>();
    std::map<std::string, value_t> elements;
    for (const auto& [key, val] : value) {
        elements[key] = to_zephyr(val);
    }
    dict_obj->elements(elements);
    return dict_obj;
}

template<typename V>
inline auto from_zephyr_map(const value_t& value) -> result_t<std::map<std::string, V>> {
    if (auto dict_obj = std::dynamic_pointer_cast<dictionary_object_t>(value)) {
        std::map<std::string, V> result;
        
        for (const auto& [key, val] : dict_obj->elements()) {
            auto val_result = from_zephyr<V>(val);
            if (!val_result) {
                return result_t<std::map<std::string, V>>::error(
                    "Failed to convert dictionary value for key '" + key + 
                    "': " + val_result.error()
                );
            }
            result[key] = val_result.value();
        }
        
        return result_t<std::map<std::string, V>>::success(std::move(result));
    }
    return result_t<std::map<std::string, V>>::error("Expected dictionary, got " + get_zephyr_type_name(value));
}

// Add specific overloads for common map types
template<>
inline auto from_zephyr<std::map<std::string, int>>(const value_t& value) -> result_t<std::map<std::string, int>> {
    return from_zephyr_map<int>(value);
}

template<>
inline auto from_zephyr<std::map<std::string, double>>(const value_t& value) -> result_t<std::map<std::string, double>> {
    return from_zephyr_map<double>(value);
}

template<>
inline auto from_zephyr<std::map<std::string, std::string>>(const value_t& value) -> result_t<std::map<std::string, std::string>> {
    return from_zephyr_map<std::string>(value);
}

// Specialization for map<string, value_t> (no conversion needed)
template<>
inline auto to_zephyr<std::map<std::string, value_t>>(const std::map<std::string, value_t>& value) -> value_t {
    auto dict_obj = std::make_shared<dictionary_object_t>();
    dict_obj->elements(value);
    return dict_obj;
}

template<>
inline auto from_zephyr<std::map<std::string, value_t>>(const value_t& value) -> result_t<std::map<std::string, value_t>> {
    if (auto dict_obj = std::dynamic_pointer_cast<dictionary_object_t>(value)) {
        return result_t<std::map<std::string, value_t>>::success(dict_obj->elements());
    }
    return result_t<std::map<std::string, value_t>>::error("Expected dictionary, got " + get_zephyr_type_name(value));
}

// =============================================================================
// Optional and Variant Types
// =============================================================================

// Optional types
template<typename T>
struct is_convertible_to_zephyr<std::optional<T>> : is_convertible_to_zephyr<T> {};

template<typename T>
struct is_convertible_from_zephyr<std::optional<T>> : is_convertible_from_zephyr<T> {};

template<typename T>
inline auto to_zephyr(const std::optional<T>& value) -> value_t {
    if (value.has_value()) {
        return to_zephyr(value.value());
    } else {
        return std::make_shared<none_object_t>();
    }
}

template<typename T>
inline auto from_zephyr_optional(const value_t& value) -> result_t<std::optional<T>> {
    if (std::dynamic_pointer_cast<none_object_t>(value)) {
        return result_t<std::optional<T>>::success(std::nullopt);
    }
    
    auto val_result = from_zephyr<T>(value);
    if (!val_result) {
        return result_t<std::optional<T>>::error(val_result.error());
    }
    
    return result_t<std::optional<T>>::success(val_result.value());
}

// Add specific overloads for common optional types
template<>
inline auto from_zephyr<std::optional<int>>(const value_t& value) -> result_t<std::optional<int>> {
    return from_zephyr_optional<int>(value);
}

template<>
inline auto from_zephyr<std::optional<double>>(const value_t& value) -> result_t<std::optional<double>> {
    return from_zephyr_optional<double>(value);
}

template<>
inline auto from_zephyr<std::optional<std::string>>(const value_t& value) -> result_t<std::optional<std::string>> {
    return from_zephyr_optional<std::string>(value);
}

// =============================================================================
// Safe Conversion Implementations
// =============================================================================

template<typename T>
inline auto try_from_zephyr(const value_t& value) -> std::optional<T> {
    auto result = from_zephyr<T>(value);
    return result ? std::make_optional(result.value()) : std::nullopt;
}

// =============================================================================
// Utility Functions
// =============================================================================

// Check if value is none/null
inline auto is_none(const value_t& value) -> bool {
    return std::dynamic_pointer_cast<none_object_t>(value) != nullptr;
}

// Create none value
inline auto make_none() -> value_t {
    return std::make_shared<none_object_t>();
}

// Convert any supported type to Zephyr using SFINAE
template<typename T>
auto convert_to_zephyr(const T& value) -> std::enable_if_t<is_convertible_to_zephyr<T>::value, value_t> {
    return to_zephyr<T>(value);
}

// Convert from Zephyr to any supported type using SFINAE
template<typename T>
auto convert_from_zephyr(const value_t& value) -> std::enable_if_t<is_convertible_from_zephyr<T>::value, result_t<T>> {
    return from_zephyr<T>(value);
}

// Batch conversion functions
auto to_zephyr_list(const std::vector<value_t>& values) -> value_t;
auto from_zephyr_list(const value_t& list) -> result_t<std::vector<value_t>>;

template<typename T>
auto to_zephyr_list(const std::vector<T>& values) -> value_t {
    auto list_obj = std::make_shared<list_object_t>();
    std::vector<value_t> zephyr_values;
    zephyr_values.reserve(values.size());
    for (const auto& value : values) {
        zephyr_values.push_back(to_zephyr(value));
    }
    list_obj->elements(zephyr_values);
    return list_obj;
}

template<typename T>
auto from_zephyr_list(const value_t& list) -> result_t<std::vector<T>> {
    return from_zephyr<std::vector<T>>(list);
}

// Dictionary conversion helpers
auto to_zephyr_dict(const std::map<std::string, value_t>& values) -> value_t;
auto from_zephyr_dict(const value_t& dict) -> result_t<std::map<std::string, value_t>>;

template<typename T>
auto to_zephyr_dict(const std::map<std::string, T>& values) -> value_t {
    auto dict_obj = std::make_shared<dictionary_object_t>();
    std::map<std::string, value_t> zephyr_values;
    for (const auto& [key, value] : values) {
        zephyr_values[key] = to_zephyr(value);
    }
    dict_obj->elements(zephyr_values);
    return dict_obj;
}

template<typename T>
auto from_zephyr_dict(const value_t& dict) -> result_t<std::map<std::string, T>> {
    return from_zephyr<std::map<std::string, T>>(dict);
}

// Type information
auto get_cpp_type_name(const std::type_info& type) -> std::string;

template<typename T>
auto get_cpp_type_name() -> std::string {
    return get_cpp_type_name(typeid(T));
}

// Validation helpers
template<typename T>
auto validate_zephyr_type(const value_t& value, const std::string& context = "") -> result_t<T> {
    if (!is_zephyr_type<T>(value)) {
        std::string error = "Type mismatch";
        if (!context.empty()) {
            error += " in " + context;
        }
        error += ": expected " + get_cpp_type_name<T>() + 
                 ", got " + get_zephyr_type_name(value);
        return result_t<T>::error(error);
    }
    return from_zephyr<T>(value);
}



} // namespace zephyr::api