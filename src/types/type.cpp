#include "types/type.hpp"
#include "objects/object.hpp"

namespace zephyr
{

auto type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support addition");
}

auto type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support subtraction");
}

auto type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support multiplication");
}

auto type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support division");
}

auto type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support modulo");
}

auto type_t::power(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support power operation");
}

auto type_t::get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " is not subscriptable");
}

auto type_t::set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support item assignment");
}

auto type_t::is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
{
    return false;
}

auto type_t::get_member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    throw attribute_error_t("Object of type " + self->get_type()->get_name() + " has no member '" + name + "'");
}

auto type_t::set_member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support member assignment");
}

auto type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    return self.get() == other.get();
}

auto type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support comparison");
}

auto type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true;
}

auto type_t::contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " does not support membership testing");
}

auto type_t::get_length(std::shared_ptr<object_t> self) -> int
{
    throw type_error_t("Object of type " + self->get_type()->get_name() + " has no length");
}

}
