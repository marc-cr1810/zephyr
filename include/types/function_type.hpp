#pragma once

#include "type.hpp"

namespace zephyr
{

// Forward declarations
class function_object_t;
class string_object_t;

class function_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    static auto get_instance() -> std::shared_ptr<function_type_t>;

private:
    function_type_t() = default;

    static std::shared_ptr<function_type_t> s_instance;
};

}
