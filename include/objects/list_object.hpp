#pragma once

#include "object.hpp"
#include <vector>

namespace zephyr
{

class list_object_t : public object_t
{
public:
    auto get_type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t> override;
    auto set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void override;

    auto is_truthy() const -> bool override;

    explicit list_object_t(const std::vector<std::shared_ptr<object_t>>& elements);
    list_object_t();

    // Getter and setter methods
    auto get_elements() const -> const std::vector<std::shared_ptr<object_t>>&;
    auto get_elements_mutable() -> std::vector<std::shared_ptr<object_t>>&;
    auto set_elements(const std::vector<std::shared_ptr<object_t>>& elems) -> void;

private:
    std::vector<std::shared_ptr<object_t>> m_elements;

    auto check_bounds(int index) const -> void;
    auto normalize_index(int index) const -> int;
    auto append_element(std::shared_ptr<object_t> element) -> void;
    auto pop_element(int index) -> std::shared_ptr<object_t>;
};

}