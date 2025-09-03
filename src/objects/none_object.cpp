#include "objects/none_object.hpp"
#include "types/none_type.hpp"

namespace zephyr
{

std::shared_ptr<none_object_t> none_object_t::s_instance = nullptr;

none_object_t::none_object_t()
{
}

auto none_object_t::get_type() const -> std::shared_ptr<type_t>
{
    return none_type_t::get_instance();
}

auto none_object_t::to_string() const -> std::string
{
    return "none";
}

auto none_object_t::get_instance() -> std::shared_ptr<none_object_t>
{
    if (!s_instance)
    {
        s_instance = std::shared_ptr<none_object_t>(new none_object_t());
    }
    return s_instance;
}

} // namespace zephyr
