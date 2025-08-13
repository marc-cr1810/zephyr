#pragma once

#include "type.hpp"
#include <stdexcept>

namespace zephyr
{

// Forward declarations
class list_object_t;
class int_object_t;
class string_object_t;

class list_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t> override;
    auto set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool override;
    auto get_length(std::shared_ptr<object_t> self) -> int override;

    static auto get_instance() -> std::shared_ptr<list_type_t>;

private:
    list_type_t() = default;

    static std::shared_ptr<list_type_t> s_instance;

    auto validate_list_index(std::shared_ptr<object_t> self, int index) const -> void;
    auto normalize_list_index(std::shared_ptr<object_t> self, int index) const -> int;
    auto concatenate_lists(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) const -> std::shared_ptr<object_t>;
    auto repeat_list(std::shared_ptr<object_t> self, int count) const -> std::shared_ptr<object_t>;
    auto validate_multiplication_operand(std::shared_ptr<object_t> other) const -> void;
};

}
