#include "zephyr/api/cpp_object_wrapper.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/api/result.hpp"
#include <iostream>

namespace zephyr::api {

// =============================================================================
// C++ Object Wrapper Base Implementation
// =============================================================================

cpp_object_wrapper_base_t::cpp_object_wrapper_base_t(std::shared_ptr<native_class_t> class_def)
    : m_class_definition(class_def) {
}

auto cpp_object_wrapper_base_t::type() const -> std::shared_ptr<type_t> {
    // Return nullptr for now - would need proper type system integration
    return nullptr;
}

auto cpp_object_wrapper_base_t::to_string() const -> std::string {
    // Get the string representation from the concrete wrapper
    return get_cpp_object_string();
}

auto cpp_object_wrapper_base_t::member(const std::string& member_name) -> std::shared_ptr<object_t> {
    std::cout << "DEBUG: cpp_object_wrapper_base_t::member() called for member '" << member_name << "'" << std::endl;
    auto result = get_member_value(member_name);
    if (result) {
        std::cout << "DEBUG: get_member_value succeeded for '" << member_name << "'" << std::endl;
        return result.value();
    } else {
        std::cout << "DEBUG: get_member_value failed for '" << member_name << "': " << result.error() << std::endl;
        // For compatibility with existing code, return nullptr instead of throwing
        // The caller should check for nullptr to handle missing members gracefully
        return nullptr;
    }
}

auto cpp_object_wrapper_base_t::member(const std::string& member_name, std::shared_ptr<object_t> value) -> void {
    std::cout << "DEBUG: cpp_object_wrapper_base_t::member(set) called for member '" << member_name << "'" << std::endl;
    auto result = set_member_value(member_name, value);
    if (!result) {
        std::cout << "DEBUG: set_member_value failed for '" << member_name << "': " << result.error() << std::endl;
        throw std::runtime_error("Failed to set member '" + member_name + "': " + result.error());
    } else {
        std::cout << "DEBUG: set_member_value succeeded for '" << member_name << "'" << std::endl;
    }
}

auto cpp_object_wrapper_base_t::has_member(const std::string& name) const -> bool {
    std::cout << "DEBUG: cpp_object_wrapper_base_t::has_member() called for member '" << name << "'" << std::endl;
    bool result = m_class_definition && m_class_definition->has_property(name);
    std::cout << "DEBUG: has_member('" << name << "') returning " << (result ? "true" : "false") << std::endl;
    return result;
}

auto cpp_object_wrapper_base_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
    auto result = call_cpp_method(method_name, args);
    if (result) {
        return result.value();
    } else {
        throw std::runtime_error("Method call failed: " + result.error());
    }
}

} // namespace zephyr::api