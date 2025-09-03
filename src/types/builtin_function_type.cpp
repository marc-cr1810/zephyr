#include "types/builtin_function_type.hpp"
#include "objects/object.hpp"
#include "errors.hpp"

namespace zephyr
{

auto builtin_function_type_t::get_instance() -> std::shared_ptr<builtin_function_type_t>
{
    static auto instance = std::shared_ptr<builtin_function_type_t>(new builtin_function_type_t());
    return instance;
}

auto builtin_function_type_t::name() const -> std::string
{
    return "builtin_function";
}

auto builtin_function_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Builtin functions are always truthy
}

auto builtin_function_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->type()->name() != "builtin_function")
    {
        return false;
    }

    // Function equality is pointer equality
    return self.get() == other.get();
}

} // namespace zephyr
