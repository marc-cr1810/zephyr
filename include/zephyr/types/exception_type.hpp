#pragma once

#include "type.hpp"

namespace zephyr
{

class exception_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int override;

    auto member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t> override;
    auto is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool override;

    static auto get_instance() -> std::shared_ptr<exception_type_t>;

private:
    exception_type_t() = default;

    static std::shared_ptr<exception_type_t> s_instance;
};

}