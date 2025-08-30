#pragma once

#include "object.hpp"

namespace zephyr
{

class boolean_object_t : public object_t
{
public:
    explicit boolean_object_t(bool val);

    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto is_truthy() const -> bool override;

public:
    bool m_value;

private:
    // Boolean objects can be cached as there are only two possible values
    static std::shared_ptr<boolean_object_t> s_true_instance;
    static std::shared_ptr<boolean_object_t> s_false_instance;

public:
    static auto get_true() -> std::shared_ptr<boolean_object_t>;
    static auto get_false() -> std::shared_ptr<boolean_object_t>;
    static auto get_instance(bool value) -> std::shared_ptr<boolean_object_t>;
};

}
