#pragma once

#include "type.hpp"

namespace zephyr
{

class float_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto power(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto negate(std::shared_ptr<object_t> self) -> std::shared_ptr<object_t> override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int override;

    static auto get_instance() -> std::shared_ptr<float_type_t>;

private:
    float_type_t() = default;

    static std::shared_ptr<float_type_t> s_instance;

    auto check_division_by_zero(std::shared_ptr<object_t> other) const -> void;
    auto perform_float_division(double left, double right) const -> std::shared_ptr<object_t>;
    auto perform_float_modulo(double left, double right) const -> std::shared_ptr<object_t>;
    auto format_float_result(double value) const -> std::shared_ptr<object_t>;
};

}
