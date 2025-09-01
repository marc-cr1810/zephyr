#pragma once

#include "type.hpp"

namespace zephyr
{

// Forward declarations
class dictionary_object_t;
class int_object_t;
class string_object_t;
class list_object_t;

class dictionary_type_t : public type_t
{
public:
    auto get_name() const -> std::string override;

    

    auto get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t> override;
    auto set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void override;

    auto is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool override;
    auto get_member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t> override;
    auto set_member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void override;

    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;
    auto contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool override;
    auto get_length(std::shared_ptr<object_t> self) -> int override;

    static auto get_instance() -> std::shared_ptr<dictionary_type_t>;

private:
    dictionary_type_t() = default;

    static std::shared_ptr<dictionary_type_t> s_instance;

    auto key_to_string(std::shared_ptr<object_t> key) const -> std::string;
};

}
