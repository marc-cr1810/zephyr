#pragma once

#include <string>
#include <memory>
#include <vector>

namespace zephyr
{

// Forward declarations
class type_t;

// Base object_t class
class object_t : public std::enable_shared_from_this<object_t>
{
public:
    virtual ~object_t() = default;

    virtual auto get_type() const -> std::shared_ptr<type_t> = 0;
    virtual auto to_string() const -> std::string;

    // Binary operations (will be delegated to type_t)
    virtual auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;

    // Truthiness check
    virtual auto is_truthy() const -> bool;

    // Negation operation
    virtual auto negate() -> std::shared_ptr<object_t>;

    // List operations
    virtual auto get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>;
    virtual auto set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void;

    // Member access operations
    virtual auto get_member(const std::string& member_name) -> std::shared_ptr<object_t>;
    virtual auto set_member(const std::string& member_name, std::shared_ptr<object_t> value) -> void;

    // Function call operations
    virtual auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>;
    virtual auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>;

    // Container operations
    virtual auto contains(std::shared_ptr<object_t> item) -> bool;
};

}
