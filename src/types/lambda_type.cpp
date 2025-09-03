#include "types/lambda_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"

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

}
