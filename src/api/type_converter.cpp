#include "zephyr/api/type_converter.hpp"
#include "zephyr/types/type.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/boolean_object.hpp"
#include "zephyr/objects/list_object.hpp"
#include "zephyr/objects/dictionary_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include <typeinfo>
#include <cxxabi.h>

namespace zephyr::api {

// =============================================================================
// Type Name Utilities Implementation
// =============================================================================

auto get_zephyr_type_name(const value_t& value) -> std::string {
    if (!value) {
        return "null";
    }
    
    if (std::dynamic_pointer_cast<int_object_t>(value)) {
        return "int";
    }
    if (std::dynamic_pointer_cast<float_object_t>(value)) {
        return "float";
    }
    if (std::dynamic_pointer_cast<string_object_t>(value)) {
        return "string";
    }
    if (std::dynamic_pointer_cast<boolean_object_t>(value)) {
        return "boolean";
    }
    if (std::dynamic_pointer_cast<list_object_t>(value)) {
        return "list";
    }
    if (std::dynamic_pointer_cast<dictionary_object_t>(value)) {
        return "dictionary";
    }
    if (std::dynamic_pointer_cast<none_object_t>(value)) {
        return "none";
    }
    
    // For unknown types, try to get the type name
    auto type = value->type();
    if (type) {
        return type->name();
    }
    return "unknown";
}

auto get_cpp_type_name(const std::type_info& type) -> std::string {
    int status = 0;
    char* demangled_name = abi::__cxa_demangle(type.name(), 0, 0, &status);
    if (status == 0) {
        std::string result(demangled_name);
        free(demangled_name);
        return result;
    }
    // If demangling fails, return the raw name
    return type.name();
}

// =============================================================================
// Batch Conversion Function Implementations
// =============================================================================

auto to_zephyr_list(const std::vector<value_t>& values) -> value_t {
    auto list_obj = std::make_shared<list_object_t>();
    list_obj->elements(values);
    return list_obj;
}

auto from_zephyr_list(const value_t& list) -> result_t<std::vector<value_t>> {
    if (auto list_obj = std::dynamic_pointer_cast<list_object_t>(list)) {
        return result_t<std::vector<value_t>>::success(list_obj->elements());
    }
    return result_t<std::vector<value_t>>::error("Expected list, got " + get_zephyr_type_name(list));
}

auto to_zephyr_dict(const std::map<std::string, value_t>& values) -> value_t {
    auto dict_obj = std::make_shared<dictionary_object_t>();
    dict_obj->elements(values);
    return dict_obj;
}

auto from_zephyr_dict(const value_t& dict) -> result_t<std::map<std::string, value_t>> {
    if (auto dict_obj = std::dynamic_pointer_cast<dictionary_object_t>(dict)) {
        return result_t<std::map<std::string, value_t>>::success(dict_obj->elements());
    }
    return result_t<std::map<std::string, value_t>>::error("Expected dictionary, got " + get_zephyr_type_name(dict));
}

} // namespace zephyr::api