#pragma once

#include "object.hpp"
#include <vector>
#include <string>
#include <memory>

namespace zephyr
{

// Forward declarations
class block_t;
struct parameter_t;

class function_object_t : public object_t
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

    explicit function_object_t(std::vector<parameter_t> params, std::unique_ptr<block_t> body, const std::string& return_type_name, bool has_explicit_return_type, bool async = false);

public:
    std::vector<parameter_t> m_parameters;
    std::unique_ptr<block_t> m_body;
    std::string m_return_type_name;
    bool m_has_explicit_return_type;
    bool m_is_async;

private:
    auto format_parameters() const -> std::string;
    auto validate_parameters() const -> void;
};

}
