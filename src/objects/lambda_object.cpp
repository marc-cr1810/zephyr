#include "objects/lambda_object.hpp"
#include "types/lambda_type.hpp"
#include "ast.hpp"

namespace zephyr
{

lambda_object_t::lambda_object_t(std::vector<parameter_t> params, std::shared_ptr<expression_t> body_expr, 
                                std::map<std::string, std::shared_ptr<object_t>> captured, bool async)
    : m_parameters(std::move(params))
    , m_body_block(nullptr)
    , m_body_expression(body_expr)
    , m_is_block_body(false)
    , m_is_async(async)
    , m_captured_variables(std::move(captured))
{
    validate_parameters();
    copy_captured_variables(m_captured_variables);
}

lambda_object_t::lambda_object_t(std::vector<parameter_t> params, std::shared_ptr<block_t> body_blk,
                                std::map<std::string, std::shared_ptr<object_t>> captured, bool async)
    : m_parameters(std::move(params))
    , m_body_block(body_blk)
    , m_body_expression(nullptr)
    , m_is_block_body(true)
    , m_is_async(async)
    , m_captured_variables(std::move(captured))
{
    validate_parameters();
    copy_captured_variables(m_captured_variables);
}

auto lambda_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return lambda_type_t::get_instance();
}

auto lambda_object_t::to_string() const -> std::string
{
    return m_is_async ? "<async lambda>" : "<lambda>";
}

auto lambda_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for lambda functions");
}

auto lambda_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for lambda functions");
}

auto lambda_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for lambda functions");
}

auto lambda_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for lambda functions");
}

auto lambda_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw std::runtime_error("Operation not supported for lambda functions");
}

auto lambda_object_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Lambda execution is handled directly in the interpreter's visit(function_call_t&) method
    // This method should not be called directly
    throw std::runtime_error("Lambda call should be handled by interpreter");
}

auto lambda_object_t::format_parameters() const -> std::string
{
    std::string result = "(";
    bool first = true;
    
    for (const auto& param : m_parameters)
    {
        if (!first)
        {
            result += ", ";
        }
        first = false;
        
        if (param.is_const)
        {
            result += "const ";
        }
        
        result += param.name;
        
        if (param.has_explicit_type)
        {
            result += " : " + param.type_name;
        }
    }
    
    result += ")";
    return result;
}

auto lambda_object_t::validate_parameters() const -> void
{
    for (const auto& param : m_parameters)
    {
        if (param.name.empty())
        {
            throw std::runtime_error("Lambda parameter name cannot be empty");
        }
    }
}

auto lambda_object_t::copy_captured_variables(const std::map<std::string, std::shared_ptr<object_t>>& captured) -> void
{
    // The captured variables are already copied by the move constructor
    // This method could be used for additional validation or processing if needed
    for (const auto& pair : captured)
    {
        if (pair.first.empty())
        {
            throw std::runtime_error("Captured variable name cannot be empty");
        }
    }
}

} // namespace zephyr