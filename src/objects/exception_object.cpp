#include "zephyr/objects/exception_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/types/exception_type.hpp"

namespace zephyr
{



exception_object_t::exception_object_t(const std::string& exception_type, const std::string& message)
    : m_exception_type(exception_type), m_message(message), m_cause(nullptr)
{
}

exception_object_t::exception_object_t(const std::string& exception_type, const std::string& message, std::shared_ptr<object_t> cause)
    : m_exception_type(exception_type), m_message(message), m_cause(std::move(cause))
{
}

auto exception_object_t::type() const -> std::shared_ptr<type_t>
{
    return exception_type_t::get_instance();
}

auto exception_object_t::to_string() const -> std::string
{
    return m_exception_type + ": " + m_message;
}

auto exception_object_t::member(const std::string& member_name) -> std::shared_ptr<object_t>
{
    // Delegate to the type's member implementation
    return type()->member(shared_from_this(), member_name);
}

}