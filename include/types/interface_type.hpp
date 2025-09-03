#pragma once

#include "type.hpp"
#include <string>
#include <memory>
#include <map>

namespace zephyr
{

class interface_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    static auto get_instance(const std::string& interface_name) -> std::shared_ptr<interface_type_t>;

private:
    explicit interface_type_t(const std::string& interface_name);

    std::string m_interface_name;

    static std::map<std::string, std::shared_ptr<interface_type_t>> s_instances;
};

}
