#pragma once

#include "object.hpp"

namespace zephyr
{

class none_object_t : public object_t
{
public:
    none_object_t();

    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto is_truthy() const -> bool override;

private:
    // None objects are singletons, so we store a static instance
    static std::shared_ptr<none_object_t> s_instance;

public:
    static auto get_instance() -> std::shared_ptr<none_object_t>;
};

}
