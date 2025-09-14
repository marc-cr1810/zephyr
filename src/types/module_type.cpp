#include "zephyr/types/module_type.hpp"
#include "zephyr/objects/module_object.hpp"
#include "zephyr/errors.hpp"

namespace zephyr
{

std::shared_ptr<module_type_t> module_type_t::s_instance = nullptr;

auto module_type_t::get_instance() -> std::shared_ptr<module_type_t>
{
    if (!s_instance)
    {
        s_instance = std::shared_ptr<module_type_t>(new module_type_t());
    }
    return s_instance;
}

auto module_type_t::name() const -> std::string
{
    return "module";
}

auto module_type_t::is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
{
    auto module_obj = std::static_pointer_cast<module_object_t>(self);
    return module_obj->has_member(name);
}

auto module_type_t::member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    auto module_obj = std::static_pointer_cast<module_object_t>(self);
    return module_obj->member(name);
}

auto module_type_t::member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
{
    throw attribute_error_t("Cannot assign to module member '" + name + "'");
}

} // namespace zephyr