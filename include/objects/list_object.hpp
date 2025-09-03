#pragma once

#include "object.hpp"
#include <vector>

namespace zephyr
{

class list_object_t : public object_t
{
public:
    explicit list_object_t(const std::vector<std::shared_ptr<object_t>>& elements);
    list_object_t();

    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Getter and setter methods
    auto elements() const -> const std::vector<std::shared_ptr<object_t>>&;
    auto elements_mutable() -> std::vector<std::shared_ptr<object_t>>&;
    auto elements(const std::vector<std::shared_ptr<object_t>>& elems) -> void;

private:
    std::vector<std::shared_ptr<object_t>> m_elements;
};

}
