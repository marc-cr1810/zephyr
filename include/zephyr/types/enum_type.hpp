#pragma once

#include "type.hpp"
#include <memory>

namespace zephyr
{

// Forward declarations
class enum_object_t;
class enum_variant_object_t;
class bound_enum_method_t;

// Type for enum definitions (e.g., Color enum)
class enum_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    // Member access for enum variants
    auto is_member_present(std::shared_ptr<object_t> self, const std::string& name) -> bool override;
    auto member(std::shared_ptr<object_t> self, const std::string& name) -> std::shared_ptr<object_t> override;

    // Comparison operations
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    // Logical operations
    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;

    // Get singleton instance
    static auto get_instance() -> std::shared_ptr<enum_type_t>;

private:
    enum_type_t() = default;
    static std::shared_ptr<enum_type_t> s_instance;
};

// Type for enum variant instances (e.g., Color.RED)
class enum_variant_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    // Comparison operations
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    // Logical operations  
    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;

    // Get singleton instance
    static auto get_instance() -> std::shared_ptr<enum_variant_type_t>;

private:
    enum_variant_type_t() = default;
    static std::shared_ptr<enum_variant_type_t> s_instance;
};

// Type for enum variant constructors (e.g., callable Color.RED)
class enum_variant_constructor_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    // Comparison operations
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    // Logical operations
    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;

    // Get singleton instance
    static auto get_instance() -> std::shared_ptr<enum_variant_constructor_type_t>;

private:
    enum_variant_constructor_type_t() = default;
    static std::shared_ptr<enum_variant_constructor_type_t> s_instance;
};

// Type for bound enum methods (e.g., callable Enum.method)
class bound_enum_method_type_t : public type_t
{
public:
    auto name() const -> std::string override;

    // Comparison operations
    auto equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool override;

    // Logical operations
    auto is_truthy(std::shared_ptr<object_t> self) -> bool override;

    // Get singleton instance
    static auto get_instance() -> std::shared_ptr<bound_enum_method_type_t>;

private:
    bound_enum_method_type_t() = default;
    static std::shared_ptr<bound_enum_method_type_t> s_instance;
};

} // namespace zephyr