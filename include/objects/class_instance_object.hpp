#pragma once

#include "object.hpp"
#include <string>
#include <map>
#include <memory>
#include <set>

namespace zephyr
{

// Forward declarations
class class_object_t;
class function_definition_t;

class class_instance_t : public object_t
{
public:
    explicit class_instance_t(std::shared_ptr<class_object_t> class_obj);

    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;

    auto has_member(const std::string& name) const -> bool;
    auto get_member(const std::string& member_name) -> std::shared_ptr<object_t> override;
    auto set_member(const std::string& member_name, std::shared_ptr<object_t> value) -> void override;
    auto has_method(const std::string& name) const -> bool;
    auto get_method(const std::string& name) const -> std::shared_ptr<function_definition_t>;
    auto mark_member_const(const std::string& name) -> void;
    auto is_member_const(const std::string& name) const -> bool;

public:
    std::shared_ptr<class_object_t> m_class_obj;
    std::map<std::string, std::shared_ptr<object_t>> m_members;
    std::set<std::string> m_const_members;

private:
    auto initialize_default_members() -> void;
};

}
