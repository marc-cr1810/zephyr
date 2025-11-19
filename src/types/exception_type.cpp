#include "zephyr/types/exception_type.hpp"
#include "zephyr/objects/exception_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/errors.hpp"

namespace zephyr
{

std::shared_ptr<exception_type_t> exception_type_t::s_instance = nullptr;

auto exception_type_t::name() const -> std::string
{
    return "exception";
}

auto exception_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    return true; // Exceptions are always truthy
}

auto exception_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    auto exception_self = std::dynamic_pointer_cast<exception_object_t>(self);
    auto exception_other = std::dynamic_pointer_cast<exception_object_t>(other);
    
    if (!exception_self || !exception_other)
    {
        return false;
    }
    
    return (exception_self->get_exception_type() == exception_other->get_exception_type() &&
            exception_self->get_message() == exception_other->get_message());
}

auto exception_type_t::compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
{
    auto exception_self = std::dynamic_pointer_cast<exception_object_t>(self);
    auto exception_other = std::dynamic_pointer_cast<exception_object_t>(other);
    
    if (!exception_self || !exception_other)
    {
        throw type_error_t("Cannot compare exception with non-exception");
    }
    
    // Compare by exception type first, then by message
    int type_cmp = exception_self->get_exception_type().compare(exception_other->get_exception_type());
    if (type_cmp != 0)
    {
        return type_cmp;
    }
    
    return exception_self->get_message().compare(exception_other->get_message());
}

auto exception_type_t::member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
{
    auto exception_obj = std::dynamic_pointer_cast<exception_object_t>(self);
    if (!exception_obj)
    {
        throw type_error_t("Invalid object type for exception member access");
    }
    
    if (name == "message")
    {
        return std::make_shared<string_object_t>(exception_obj->get_message());
    }
    else if (name == "type")
    {
        return std::make_shared<string_object_t>(exception_obj->get_exception_type());
    }
    else if (name == "cause")
    {
        return exception_obj->get_cause() ? exception_obj->get_cause() : std::make_shared<none_object_t>();
    }
    
    throw attribute_error_t("'" + exception_obj->get_exception_type() + "' object has no attribute '" + name + "'");
}

auto exception_type_t::is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
{
    return (name == "message" || name == "type" || name == "cause");
}

auto exception_type_t::get_instance() -> std::shared_ptr<exception_type_t>
{
    if (!s_instance)
    {
        s_instance = std::shared_ptr<exception_type_t>(new exception_type_t());
    }
    return s_instance;
}

}