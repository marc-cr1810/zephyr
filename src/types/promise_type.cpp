#include "types/promise_type.hpp"
#include "errors.hpp"

namespace zephyr
{

std::shared_ptr<promise_type_t> promise_type_t::s_instance = nullptr;

auto promise_type_t::get_name() const -> std::string
{
    return "promise";
}

auto promise_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Promise objects do not support addition", 0, 0, 1);
}

auto promise_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Promise objects do not support subtraction", 0, 0, 1);
}

auto promise_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Promise objects do not support multiplication", 0, 0, 1);
}

auto promise_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Promise objects do not support division", 0, 0, 1);
}

auto promise_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Promise objects do not support modulo", 0, 0, 1);
}

auto promise_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Promises are always truthy
}

auto promise_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    return self.get() == other.get(); // Reference equality
}

auto promise_type_t::get_instance() -> std::shared_ptr<promise_type_t>
{
    if (!s_instance)
    {
        s_instance = std::shared_ptr<promise_type_t>(new promise_type_t());
    }
    return s_instance;
}

} // namespace zephyr
