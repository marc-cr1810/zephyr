#include "objects/builtin_function_object.hpp"
#include "types/builtin_function_type.hpp"
#include "errors.hpp"

namespace zephyr
{

builtin_function_object_t::builtin_function_object_t(builtin_function_ptr_t ptr, const std::string& function_name)
    : m_func_ptr(ptr)
    , m_name(function_name)
{
    validate_function_pointer();
    validate_function_name();
}

auto builtin_function_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return builtin_function_type_t::get_instance();
}

auto builtin_function_object_t::to_string() const -> std::string
{
    return "<builtin function " + m_name + ">";
}

auto builtin_function_object_t::call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    return m_func_ptr(args);
}

auto builtin_function_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("addition");
    return nullptr;
}

auto builtin_function_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("subtraction");
    return nullptr;
}

auto builtin_function_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("multiplication");
    return nullptr;
}

auto builtin_function_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("division");
    return nullptr;
}

auto builtin_function_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("modulo");
    return nullptr;
}

auto builtin_function_object_t::validate_function_pointer() const -> void
{
    if (!m_func_ptr)
    {
        throw value_error_t("Builtin function pointer cannot be null", 0, 0, 1);
    }
}

auto builtin_function_object_t::validate_function_name() const -> void
{
    if (m_name.empty())
    {
        throw value_error_t("Builtin function name cannot be empty", 0, 0, 1);
    }
}

auto builtin_function_object_t::throw_unsupported_operation(const std::string& operation) const -> void
{
    throw type_error_t("Operation " + operation + " not supported for builtin function '" + m_name + "'", 0, 0, 1);
}

} // namespace zephyr
