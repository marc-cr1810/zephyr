#pragma once

#include "object.hpp"
#include <memory>
#include <functional>
#include <vector>

namespace zephyr
{

enum class promise_state_e
{
    pending,
    fulfilled,
    rejected
};

class promise_object_t : public object_t
{
public:
    explicit promise_object_t();
    ~promise_object_t() override = default;

    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;

    // Promise control methods
    auto resolve(std::shared_ptr<object_t> value) -> void;
    auto reject(const std::string& error) -> void;

    // Promise chaining methods
    auto then(std::function<void(std::shared_ptr<object_t>)> callback) -> void;
    auto catch_error(std::function<void(const std::string&)> callback) -> void;

    // Promise state checking
    auto is_resolved() const -> bool;
    auto is_pending() const -> bool;
    auto is_fulfilled() const -> bool;
    auto is_rejected() const -> bool;

public:
    promise_state_e m_state;
    std::shared_ptr<object_t> m_result;
    std::string m_error_message;

    // Callbacks for promise resolution
    std::vector<std::function<void(std::shared_ptr<object_t>)>> m_then_callbacks;
    std::vector<std::function<void(const std::string&)>> m_catch_callbacks;

private:
    auto execute_then_callbacks() -> void;
    auto execute_catch_callbacks() -> void;
    auto validate_state_transition(promise_state_e new_state) const -> void;
    auto format_state_string() const -> std::string;
};

}
