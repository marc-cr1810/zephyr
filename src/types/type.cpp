#include "types/type.hpp"
#include "objects/object.hpp"
#include "errors.hpp"

namespace zephyr
{

auto type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support addition");
}

auto type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support subtraction");
}

auto type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support multiplication");
}

auto type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support division");
}

auto type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support modulo");
}

auto type_t::power(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support power operation");
}

auto type_t::negate(std::shared_ptr<object_t> self) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support negation");
}

auto type_t::item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    throw type_error_t("Object of type " + self->type()->name() + " is not subscriptable");
}

auto type_t::item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support item assignment");
}

auto type_t::is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
{
    return false;
}

auto type_t::member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    throw attribute_error_t("Object of type " + self->type()->name() + " has no member '" + name + "'");
}

auto type_t::member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support member assignment");
}

auto type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    return self.get() == other.get();
}

auto type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support comparison");
}

auto type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true;
}

auto type_t::contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
{
    throw type_error_t("Object of type " + self->type()->name() + " does not support membership testing");
}

auto type_t::length(std::shared_ptr<object_t> self) -> int
{
    throw type_error_t("Type '" + name() + "' does not support length operation");
}

auto type_t::slice(std::shared_ptr<object_t> self, 
                   std::shared_ptr<object_t> start, 
                   std::shared_ptr<object_t> end, 
                   std::shared_ptr<object_t> step) -> std::shared_ptr<object_t>
{
    throw type_error_t("Type '" + name() + "' does not support slicing");
}

}
