#pragma once

#include "object.hpp"
#include <string>
#include <map>
#include <vector>

namespace zephyr
{

// Forward declarations
class function_definition_t;
class member_variable_declaration_t;

// Durable storage for member variable information
struct member_variable_info_t
{
    std::string name;
    std::string type_name;
    std::shared_ptr<object_t> default_value;
    bool has_default;
    bool is_const;

    member_variable_info_t(const std::string& var_name, const std::string& var_type = "",
                            std::shared_ptr<object_t> default_val = nullptr, bool has_def = false, bool is_const_var = false)
        : name(var_name)
        , type_name(var_type)
        , default_value(default_val)
        , has_default(has_def)
        , is_const(is_const_var)
    {
    }
};

class class_object_t : public object_t
{
public:
    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;

    auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    explicit class_object_t(const std::string& class_name);

    auto add_method(const std::string& method_name, std::shared_ptr<function_definition_t> method) -> void;
    auto add_member_variable(const member_variable_info_t& var_info) -> void;
    auto has_method(const std::string& method_name) const -> bool;
    auto get_method(const std::string& method_name) const -> std::shared_ptr<function_definition_t>;
    auto has_member_variable(const std::string& var_name) const -> bool;
    auto get_member_variable_info(const std::string& var_name) const -> member_variable_info_t;

public:
    void add_interface(const std::string& interface_name);
    const std::vector<std::string>& get_interfaces() const;

public:
    std::string m_class_name;
    std::vector<std::string> m_interfaces;
    std::map<std::string, std::shared_ptr<function_definition_t>> m_methods;
    std::vector<member_variable_info_t> m_member_variables;

private:
    auto validate_method_name(const std::string& method_name) const -> void;
    auto validate_member_variable_name(const std::string& var_name) const -> void;
    auto format_methods() const -> std::string;
    auto format_member_variables() const -> std::string;
};

}
