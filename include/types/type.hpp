#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include "errors.hpp"

namespace zephyr
{

// Forward declarations
class object_t;

// Base type_t class
class type_t
{
public:
    virtual ~type_t() = default;

    virtual auto get_name() const -> std::string = 0;

    // Binary operation dispatch
    virtual auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;
    virtual auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t> = 0;

    // List operations
    virtual auto get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
    {
        throw type_error_t("Object of type " + get_name() + " is not subscriptable");
    }

    virtual auto set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
    {
        throw type_error_t("Object of type " + get_name() + " does not support item assignment");
    }

    // Dictionary operations
    virtual auto is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool
    {
        return false;
    }

    virtual auto get_member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>
    {
        throw attribute_error_t("Object of type " + get_name() + " has no member '" + name + "'");
    }

    virtual auto set_member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void
    {
        throw type_error_t("Object of type " + get_name() + " does not support member assignment");
    }

    // Comparison operations
    virtual auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
    {
        return self.get() == other.get();
    }

    virtual auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int
    {
        throw type_error_t("Object of type " + get_name() + " does not support comparison");
    }

    // Logical operations
    virtual auto is_truthy(std::shared_ptr<object_t> self) -> bool = 0;

    // Container operations
    virtual auto contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
    {
        throw type_error_t("Object of type " + get_name() + " does not support membership testing");
    }

    virtual auto get_length(std::shared_ptr<object_t> self) -> int
    {
        throw type_error_t("Object of type " + get_name() + " has no length");
    }
};

}
