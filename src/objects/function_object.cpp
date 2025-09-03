#include "objects/function_object.hpp"
#include "types/function_type.hpp"
#include "ast.hpp"
#include "errors.hpp"

namespace zephyr
{

function_object_t::function_object_t(std::vector<parameter_t> params, std::unique_ptr<block_t> body, const std::string& return_type_name, bool explicit_return_type, bool async)
    : m_parameters(std::move(params))
    , m_body(std::move(body))
    , m_return_type_name(return_type_name)
    , m_has_explicit_return_type(explicit_return_type)
    , m_is_async(async)
{
    validate_parameters();
}

auto function_object_t::type() const -> std::shared_ptr<type_t>
{
    return function_type_t::get_instance();
}

auto function_object_t::to_string() const -> std::string
{
    return m_is_async ? "<async function>" : "<function>";
}

auto function_object_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    // Function execution is handled directly in the interpreter's visit(function_call_t&) method
    // This method should not be called directly
    throw type_error_t("Function call should be handled by interpreter");
}

auto function_object_t::format_parameters() const -> std::string
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

auto function_object_t::validate_parameters() const -> void
{
    for (const auto& param : m_parameters)
    {
        if (param.name.empty())
        {
            throw value_error_t("Function parameter name cannot be empty");
        }
    }
}

} // namespace zephyr
