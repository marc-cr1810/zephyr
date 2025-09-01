#pragma once

#include "type.hpp"

namespace zephyr
{

class none_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    static auto get_instance() -> std::shared_ptr<none_type_t>;

private:
    none_type_t() = default;

    static std::shared_ptr<none_type_t> s_instance;
};

}
