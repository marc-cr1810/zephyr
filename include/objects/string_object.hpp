#pragma once

#include "object.hpp"
#include <string>

namespace zephyr
{

class string_object_t : public object_t
{
public:
    explicit string_object_t(const std::string& val);

    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Getter and setter methods
    auto value() const -> const std::string&;
    auto value(const std::string& val) -> void;

private:
    std::string m_value;
};

}
