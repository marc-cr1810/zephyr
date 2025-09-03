#include "types/function_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"

namespace zephyr
{

auto function_type_t::get_instance() -> std::shared_ptr<function_type_t>
{
    static auto instance = std::shared_ptr<function_type_t>(new function_type_t());
    return instance;
}

auto function_type_t::get_name() const -> std::string
{
    return "function";
}

auto function_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Functions are always truthy
}

auto function_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "function")
    {
        return false;
    }

    // Function equality is pointer equality
    return self.get() == other.get();
}

} // namespace zephyr
