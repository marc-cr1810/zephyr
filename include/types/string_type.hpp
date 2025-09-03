#pragma once

#include "type.hpp"

namespace zephyr
{

class string_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;
    auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> override;

    auto item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t> override;
    auto item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int override;
    auto contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool override;
    auto length(std::shared_ptr<object_t> self) -> int override;

    static auto get_instance() -> std::shared_ptr<string_type_t>;

private:
    string_type_t() = default;

    static std::shared_ptr<string_type_t> s_instance;
};

}
