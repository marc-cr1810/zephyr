#pragma once

#include "object.hpp"
#include "ast.hpp"
#include <string>
#include <vector>

namespace zephyr
{

class interface_object_t : public object_t
{
public:
    explicit interface_object_t(const std::string& interface_name);

    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    void add_method(const function_signature_t& method);
    const std::vector<function_signature_t>& get_methods() const;

public:
    std::string m_interface_name;
    std::vector<function_signature_t> m_methods;
};

}
