#pragma once

#include "object.hpp"
#include <vector>
#include <string>
#include <map>

namespace zephyr
{

// Forward declarations
class block_t;
class expression_t;
struct parameter_t;

class lambda_object_t : public object_t
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

    // Constructor for expression body
    explicit lambda_object_t(std::vector<parameter_t> params, std::shared_ptr<expression_t> body_expr,
                    std::map<std::string, std::shared_ptr<object_t>> captured, bool async = false);

    // Constructor for block body
    explicit lambda_object_t(std::vector<parameter_t> params, std::shared_ptr<block_t> body_blk,
                    std::map<std::string, std::shared_ptr<object_t>> captured, bool async = false);

public:
    std::vector<parameter_t> m_parameters;
    std::shared_ptr<block_t> m_body_block;
    std::shared_ptr<expression_t> m_body_expression;
    bool m_is_block_body;
    bool m_is_async;
    std::map<std::string, std::shared_ptr<object_t>> m_captured_variables;

private:
    auto format_parameters() const -> std::string;
    auto validate_parameters() const -> void;
    auto copy_captured_variables(const std::map<std::string, std::shared_ptr<object_t>>& captured) -> void;
};

}
