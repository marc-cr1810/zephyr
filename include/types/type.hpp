#pragma once

#include <string>
#include <memory>

namespace zephyr
{

// Forward declarations
class object_t;

// Base type_t class
class type_t
{
public:
    virtual ~type_t() = default;

    virtual auto name() const -> std::string = 0;

    // Binary operation dispatch
    virtual auto add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;
    virtual auto power(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>;

    // Unary operations
    virtual auto negate(std::shared_ptr<object_t> self) -> std::shared_ptr<object_t>;

    // List operations
    virtual auto item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>;
    virtual auto item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void;

    // Dictionary operations
    virtual auto is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool;
    virtual auto member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t>;
    virtual auto member(std::shared_ptr<object_t> self, const std::string& name, std::shared_ptr<object_t> value) -> void;

    // Comparison operations
    virtual auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool;
    virtual auto compare(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> int;

    // Logical operations
    virtual auto is_truthy(std::shared_ptr<object_t> self) -> bool;

    // Container operations
    virtual auto contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool;
    virtual auto length(std::shared_ptr<object_t> self) -> int;
};

}
