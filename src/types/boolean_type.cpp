#include "types/boolean_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"

namespace zephyr
{

auto boolean_type_t::get_instance() -> std::shared_ptr<boolean_type_t>
{
    static auto instance = std::shared_ptr<boolean_type_t>(new boolean_type_t());
    return instance;
}

auto boolean_type_t::name() const -> std::string
{
    return "bool";
}

auto boolean_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto bool_obj = std::static_pointer_cast<boolean_object_t>(self);
    return bool_obj->m_value;
}

auto boolean_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->type()->name() != "bool")
    {
        return false;
    }

    auto self_bool = std::static_pointer_cast<boolean_object_t>(self);
    auto other_bool = std::static_pointer_cast<boolean_object_t>(other);
    return self_bool->m_value == other_bool->m_value;
}

auto boolean_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    if (other->type()->name() != "bool")
    {
        throw type_error_t("Cannot compare bool with " + other->type()->name());
    }

    auto self_bool = std::static_pointer_cast<boolean_object_t>(self);
    auto other_bool = std::static_pointer_cast<boolean_object_t>(other);

    if (self_bool->m_value == other_bool->m_value)
    {
        return 0;
    }
    return self_bool->m_value ? 1 : -1;  // true > false
}

} // namespace zephyr
