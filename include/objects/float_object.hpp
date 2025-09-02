#pragma once

#include "object.hpp"

namespace zephyr
{

class float_object_t : public object_t
{
public:
    explicit float_object_t(double val);

    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Getter and setter methods
    auto get_value() const -> double;
    auto set_value(double val) -> void;

private:
    double m_value;

    
    auto format_float_string() const -> std::string;
};

}
