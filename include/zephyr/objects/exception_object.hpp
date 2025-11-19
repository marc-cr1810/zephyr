#pragma once

#include "zephyr/objects/object.hpp"
#include "zephyr/types/type.hpp"
#include <string>
#include <memory>

namespace zephyr
{

class exception_object_t : public object_t
{
public:
    exception_object_t(const std::string& exception_type, const std::string& message);
    exception_object_t(const std::string& exception_type, const std::string& message, std::shared_ptr<object_t> cause);

    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    
    // Exception-specific methods
    auto get_exception_type() const -> const std::string& { return m_exception_type; }
    auto get_message() const -> const std::string& { return m_message; }
    auto get_cause() const -> std::shared_ptr<object_t> { return m_cause; }
    
    // Member access for exception properties
    auto member(const std::string& member_name) -> std::shared_ptr<object_t> override;

private:
    std::string m_exception_type;
    std::string m_message;
    std::shared_ptr<object_t> m_cause;
};



}