#pragma once

#include "object.hpp"

namespace zephyr
{

class int_object_t : public object_t
{
public:
    explicit int_object_t(int val);

    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Getter and setter methods
    auto value() const -> int;
    auto value(int val) -> void;

private:
    int m_value;
};

}
