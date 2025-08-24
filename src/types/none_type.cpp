#include "types/none_type.hpp"
#include "errors.hpp"

namespace zephyr
{

auto none_type_t::get_instance() -> std::shared_ptr<none_type_t>
{
    static auto instance = std::shared_ptr<none_type_t>(new none_type_t());
    return instance;
}

auto none_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation: none + " + other->get_type()->get_name());
}

auto none_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation: none - " + other->get_type()->get_name());
}

auto none_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation: none * " + other->get_type()->get_name());
}

auto none_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation: none / " + other->get_type()->get_name());
}

auto none_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for none");
}

auto none_type_t::get_name() const -> std::string
{
    return "none";
}

auto none_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return false; // none is always falsy
}

auto none_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    return other->get_type()->get_name() == "none";
}


} // namespace zephyr