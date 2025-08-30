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

    auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override; // Added for **= operator

    auto is_truthy() const -> bool override;

    // Getter and setter methods
    auto get_value() const -> double;
    auto set_value(double val) -> void;

private:
    double m_value;

    auto check_division_by_zero(std::shared_ptr<object_t> other) const -> void;
    auto format_float_string() const -> std::string;
};

}
