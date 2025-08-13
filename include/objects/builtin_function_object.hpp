#pragma once

#include "object.hpp"
#include <string>
#include <vector>
#include <memory>

namespace zephyr
{

// Define a type for built-in function pointers
using builtin_function_ptr_t = std::shared_ptr<object_t> (*)(const std::vector<std::shared_ptr<object_t>>&);

class builtin_function_object_t : public object_t
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

    explicit builtin_function_object_t(builtin_function_ptr_t ptr, const std::string& function_name);

public:
    builtin_function_ptr_t m_func_ptr;
    std::string m_name;

private:
    auto validate_function_pointer() const -> void;
    auto validate_function_name() const -> void;
    auto throw_unsupported_operation(const std::string& operation) const -> void;
};

}
