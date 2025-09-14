#pragma once

#include "type.hpp"

namespace zephyr
{

class module_type_t : public type_t
{
public:
    static auto get_instance() -> std::shared_ptr<module_type_t>;

    auto name() const -> std::string override;

    // Member access operations
    auto is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool override;
    auto member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t> override;
    auto member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void override;

private:
    module_type_t() = default;
    static std::shared_ptr<module_type_t> s_instance;
};

}