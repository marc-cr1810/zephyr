#pragma once

#include "object.hpp"
#include <map>
#include <string>

namespace zephyr
{

// Forward declaration
class module_t;

class module_object_t : public object_t
{
public:
    explicit module_object_t(std::shared_ptr<module_t> module, const std::string& module_name);

    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Method call support for module.function(args)
    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;

    // Member access support for module.symbol
    auto member(const std::string& member_name) -> std::shared_ptr<object_t> override;
    auto has_member(const std::string& name) const -> bool;

    // Getter methods
    auto get_module() const -> std::shared_ptr<module_t>;
    auto get_module_name() const -> const std::string&;

private:
    std::shared_ptr<module_t> m_module;
    std::string m_module_name;
};

}