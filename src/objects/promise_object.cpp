#include "objects/promise_object.hpp"
#include "types/promise_type.hpp"

namespace zephyr
{

promise_object_t::promise_object_t()
    : m_state(promise_state_e::pending), m_result(nullptr), m_error_message("")
{
}

auto promise_object_t::resolve(std::shared_ptr<object_t> value) -> void
{
    if (m_state != promise_state_e::pending)
    {
        return; // Promise already resolved
    }

    m_state = promise_state_e::fulfilled;
    m_result = value;

    // Execute all pending then callbacks
    for (const auto& callback : m_then_callbacks)
    {
        callback(m_result);
    }
    m_then_callbacks.clear();
}

auto promise_object_t::reject(const std::string& error) -> void
{
    if (m_state != promise_state_e::pending)
    {
        return; // Promise already resolved
    }

    m_state = promise_state_e::rejected;
    m_error_message = error;

    // Execute all pending catch callbacks
    for (const auto& callback : m_catch_callbacks)
    {
        callback(m_error_message);
    }
    m_catch_callbacks.clear();
}

auto promise_object_t::then(std::function<void(std::shared_ptr<object_t>)> callback) -> void
{
    if (m_state == promise_state_e::fulfilled)
    {
        // Promise already resolved, execute callback immediately
        callback(m_result);
    }
    else if (m_state == promise_state_e::pending)
    {
        // Promise still pending, store callback for later
        m_then_callbacks.push_back(callback);
    }
    // If rejected, don't execute then callback
}

auto promise_object_t::catch_error(std::function<void(const std::string&)> callback) -> void
{
    if (m_state == promise_state_e::rejected)
    {
        // Promise already rejected, execute callback immediately
        callback(m_error_message);
    }
    else if (m_state == promise_state_e::pending)
    {
        // Promise still pending, store callback for later
        m_catch_callbacks.push_back(callback);
    }
    // If fulfilled, don't execute catch callback
}

auto promise_object_t::to_string() const -> std::string
{
    switch (m_state)
    {
        case promise_state_e::pending:
            return "Promise{PENDING}";
        case promise_state_e::fulfilled:
            return "Promise{FULFILLED: " + (m_result ? m_result->to_string() : "null") + "}";
        case promise_state_e::rejected:
            return "Promise{REJECTED: " + m_error_message + "}";
        default:
            return "Promise{UNKNOWN}";
    }
}

auto promise_object_t::type() const -> std::shared_ptr<type_t>
{
    return promise_type_t::get_instance();
}

auto promise_object_t::is_resolved() const -> bool
{
    return m_state != promise_state_e::pending;
}

auto promise_object_t::is_pending() const -> bool
{
    return m_state == promise_state_e::pending;
}

auto promise_object_t::is_fulfilled() const -> bool
{
    return m_state == promise_state_e::fulfilled;
}

auto promise_object_t::is_rejected() const -> bool
{
    return m_state == promise_state_e::rejected;
}

} // namespace zephyr
