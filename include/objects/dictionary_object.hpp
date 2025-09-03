#pragma once

#include "object.hpp"
#include <map>
#include <string>

namespace zephyr
{

class dictionary_object_t : public object_t
{
public:
    explicit dictionary_object_t(std::map<std::string, std::shared_ptr<object_t>> elems);
    dictionary_object_t();

    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Getter and setter methods
    auto get_elements() const -> const std::map<std::string, std::shared_ptr<object_t>>&;
    auto get_elements_mutable() -> std::map<std::string, std::shared_ptr<object_t>>&;
    auto set_elements(const std::map<std::string, std::shared_ptr<object_t>>& elems) -> void;

private:
    std::map<std::string, std::shared_ptr<object_t>> m_elements;
};

}
