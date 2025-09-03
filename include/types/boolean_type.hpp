#pragma once

#include "type.hpp"

namespace zephyr
{

class boolean_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int override;

    static auto get_instance() -> std::shared_ptr<boolean_type_t>;

private:
    boolean_type_t() = default;

    static std::shared_ptr<boolean_type_t> s_instance;

    auto validate_boolean_operation(const std::string& operation) const -> void;
    auto convert_to_int(bool value) const -> std::shared_ptr<object_t>;
};

}
