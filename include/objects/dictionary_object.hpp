#pragma once

#include "object.hpp"
#include <map>
#include <string>
#include <vector>

namespace zephyr
{

class dictionary_object_t : public object_t
{
public:
    explicit dictionary_object_t(std::map<std::string, std::shared_ptr<object_t>> elems);
    dictionary_object_t();

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

    // Getter and setter methods
    auto get_elements() const -> const std::map<std::string, std::shared_ptr<object_t>>&;
    auto get_elements_mutable() -> std::map<std::string, std::shared_ptr<object_t>>&;
    auto set_elements(const std::map<std::string, std::shared_ptr<object_t>>& elems) -> void;

private:
    std::map<std::string, std::shared_ptr<object_t>> m_elements;

    auto key_to_string(std::shared_ptr<object_t> key) const -> std::string;
    auto has_key(const std::string& key) const -> bool;
    auto get_keys() const -> std::vector<std::string>;
    auto get_values() const -> std::vector<std::shared_ptr<object_t>>;
};

}
