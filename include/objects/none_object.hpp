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

private:
    // None objects are singletons, so we store a static instance
    static std::shared_ptr<none_object_t> s_instance;

public:
    static auto get_instance() -> std::shared_ptr<none_object_t>;
};

}
