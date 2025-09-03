#pragma once

#include "type.hpp"

namespace zephyr
{

class list_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t> override;
    auto set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool override;
    auto get_length(std::shared_ptr<object_t> self) -> int override;

    static auto get_instance() -> std::shared_ptr<list_type_t>;

private:
    list_type_t() = default;

    static std::shared_ptr<list_type_t> s_instance;
};

}
