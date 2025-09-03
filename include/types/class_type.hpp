#pragma once

#include "type.hpp"
#include <string>
#include <memory>
#include <map>

namespace zephyr
{

class class_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    auto member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t> override;
    auto member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    static auto get_instance(const std::string& class_name) -> std::shared_ptr<class_type_t>;

private:
    explicit class_type_t(const std::string& class_name);

    std::string m_class_name;

    static std::map<std::string, std::shared_ptr<class_type_t>> s_instances;
};

}
