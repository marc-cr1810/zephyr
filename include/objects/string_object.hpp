#pragma once

#include "object.hpp"
#include <string>

namespace zephyr
{

class string_object_t : public object_t
{
public:
    explicit string_object_t(const std::string& val);

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
    auto get_value() const -> const std::string&;
    auto set_value(const std::string& val) -> void;

private:
    std::string m_value;
};

}
