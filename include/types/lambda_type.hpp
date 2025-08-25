#pragma once

#include "type.hpp"

namespace zephyr
{

// Forward declarations
class lambda_object_t;
class string_object_t;

class lambda_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    static auto get_instance() -> std::shared_ptr<lambda_type_t>;

private:
    lambda_type_t() = default;

    static std::shared_ptr<lambda_type_t> s_instance;

    auto throw_unsupported_operation(const std::string& operation) const -> void;
    auto format_lambda_string(std::shared_ptr<object_t> self) const -> std::string;
};

}
