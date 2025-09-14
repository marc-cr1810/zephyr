#include "zephyr/types/none_type.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/objects/object.hpp"

namespace zephyr
{

auto none_type_t::get_instance() -> std::shared_ptr<none_type_t>
{
    static auto instance = std::shared_ptr<none_type_t>(new none_type_t());
    return instance;
}

auto none_type_t::name() const -> std::string
{
    return "none";
}

auto none_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return false; // none is always falsy
}

auto none_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    return other->type()->name() == "none";
}


} // namespace zephyr
