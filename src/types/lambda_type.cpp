#include "types/lambda_type.hpp"
#include "objects/objects.hpp"
#include <stdexcept>

namespace zephyr
{

std::shared_ptr<lambda_type_t> lambda_type_t::s_instance = nullptr;

auto lambda_type_t::get_instance() -> std::shared_ptr<lambda_type_t>
{
    if (!s_instance) {
        s_instance = std::shared_ptr<lambda_type_t>(new lambda_type_t());
    }
    return s_instance;
}

auto lambda_type_t::get_name() const -> std::string
{
    return "lambda";
}

auto lambda_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("addition");
    return nullptr;
}

auto lambda_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("subtraction");
    return nullptr;
}

auto lambda_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("multiplication");
    return nullptr;
}

auto lambda_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("division");
    return nullptr;
}

auto lambda_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw_unsupported_operation("modulo");
    return nullptr;
}

auto lambda_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Lambda functions are always truthy
}

auto lambda_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "lambda")
    {
        return false;
    }
    
    // Lambda equality is pointer equality
    return self.get() == other.get();
}

auto lambda_type_t::throw_unsupported_operation(const std::string& operation) const -> void
{
    throw std::runtime_error("Unsupported operation '" + operation + "' for lambda functions");
}

auto lambda_type_t::format_lambda_string(std::shared_ptr<object_t> self) const -> std::string
{
    return "<lambda>";
}

}