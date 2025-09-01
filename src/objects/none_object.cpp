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

auto none_object_t::add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->add(shared_from_this(), other);
}

auto none_object_t::subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->subtract(shared_from_this(), other);
}

auto none_object_t::multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->multiply(shared_from_this(), other);
}

auto none_object_t::divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->divide(shared_from_this(), other);
}

auto none_object_t::modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    return get_type()->modulo(shared_from_this(), other);
}

auto none_object_t::is_truthy() const -> bool
{
    return get_type()->is_truthy(std::const_pointer_cast<object_t>(shared_from_this()));
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