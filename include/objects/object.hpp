#pragma once

#include <string>
#include <memory>
#include "errors.hpp"
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
    virtual auto to_string() const -> std::string = 0;

    // Binary operations (will be delegated to type_t)
    virtual auto add(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto subtract(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto multiply(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto divide(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto modulo(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto power(std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> // Added for **= operator
    {
        throw type_error_t("Object does not support power operation");
    }

    // Truthiness check
    virtual auto is_truthy() const -> bool
    {
        return true; // Default: most objects are truthy
    }

    // Negation operation
    virtual auto negate() -> std::shared_ptr<object_t>
    {
        throw type_error_t("Object does not support negation");
    }

    // List operations
    virtual auto get_item(std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
    {
        throw type_error_t("Object is not subscriptable");
    }

    virtual auto set_item(std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
    {
        throw type_error_t("Object does not support item assignment");
    }

    // Member access operations
    virtual auto get_member(const std::string& member_name) -> std::shared_ptr<object_t>
    {
        throw attribute_error_t("Object has no member '" + member_name + "'");
    }

    virtual auto set_member(const std::string& member_name, std::shared_ptr<object_t> value) -> void
    {
        throw type_error_t("Object does not support member assignment");
    }

    // Function call operations
    virtual auto call(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
    {
        throw type_error_t("Object is not callable");
    }

    virtual auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
    {
        throw attribute_error_t("Object has no method '" + method_name + "'");
    }

    // Container operations
    virtual auto contains(std::shared_ptr<object_t> item) -> bool
    {
        throw type_error_t("Object does not support containment check");
    }
};

}
