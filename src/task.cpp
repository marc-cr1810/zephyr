#include "task.hpp"
#include "async_scheduler.hpp"
#include "objects/promise_object.hpp"
#include "objects/none_object.hpp"
#include <stdexcept>

namespace zephyr
{

task_t::task_t(int id, std::function<std::shared_ptr<object_t>()> func)
    : m_task_id(id)
    , m_state(task_state_e::pending)
    , m_execution_function(func)
    , m_result(nullptr)
    , m_error_message("")
{
    m_promise = std::make_shared<promise_object_t>();
}

auto task_t::execute() -> void
{
    if (m_state != task_state_e::pending && m_state != task_state_e::suspended) {
        return; // Task already executed or in wrong state
    }

    if (m_state == task_state_e::pending) {
        validate_state_transition(task_state_e::running);
        m_state = task_state_e::running;
    } else if (m_state == task_state_e::suspended) {
        // Resume from suspended state
        validate_state_transition(task_state_e::running);
        m_state = task_state_e::running;
    }

    try {
        // Execute the function with cooperative yielding support
        if (!m_result) {
            // First execution or no result yet
            m_result = m_execution_function();
        }
        
        // If we get here without yielding, the task completed
        complete(m_result);
        
    } catch (const zephyr::yield_exception_t& e) {
        // Task yielded - suspend it (will be handled by scheduler)
        suspend();
        throw; // Re-throw for scheduler to catch
    } catch (const std::exception& e) {
        fail(e.what());
    }
}

auto task_t::suspend() -> void
{
    if (m_state == task_state_e::running || m_state == task_state_e::pending) {
        validate_state_transition(task_state_e::suspended);
        m_state = task_state_e::suspended;
    }
}

auto task_t::resume(std::shared_ptr<object_t> value) -> void
{
    if (m_state == task_state_e::suspended) {
        validate_state_transition(task_state_e::running);
        m_state = task_state_e::running;
        
        // Resume execution with the provided value
        if (m_continuation) {
            // Use continuation if available
            try {
                m_continuation(value);
                // Check if task completed after continuation
                if (m_state == task_state_e::running) {
                    complete(m_result ? m_result : value);
                }
            } catch (const std::exception& e) {
                fail(e.what());
            }
        } else {
            // No continuation - just complete with the value
            complete(value ? value : std::make_shared<none_object_t>());
        }
    }
}

auto task_t::complete(std::shared_ptr<object_t> value) -> void
{
    if (m_state == task_state_e::running || m_state == task_state_e::pending) {
        validate_state_transition(task_state_e::completed);
        m_state = task_state_e::completed;
        m_result = value ? value : std::make_shared<none_object_t>();

        if (m_promise) {
            m_promise->resolve(m_result);
        }

        notify_dependents();
        cleanup_resources();
    }
}

auto task_t::fail(const std::string& error) -> void
{
    if (m_state != task_state_e::completed && m_state != task_state_e::failed) {
        validate_state_transition(task_state_e::failed);
        m_state = task_state_e::failed;
        m_error_message = error;

        if (m_promise) {
            m_promise->reject(error);
        }

        notify_dependents();
        cleanup_resources();
    }
}

auto task_t::is_ready() const -> bool
{
    if (m_state == task_state_e::pending || m_state == task_state_e::suspended) {
        return has_completed_dependencies();
    }
    return false;
}

auto task_t::is_completed() const -> bool
{
    return m_state == task_state_e::completed;
}

auto task_t::is_failed() const -> bool
{
    return m_state == task_state_e::failed;
}

auto task_t::is_suspended() const -> bool
{
    return m_state == task_state_e::suspended;
}

auto task_t::add_dependency(std::shared_ptr<task_t> dependency) -> void
{
    if (!dependency) {
        return;
    }

    // Avoid duplicate dependencies
    for (const auto& existing_dep : m_dependencies) {
        if (existing_dep == dependency) {
            return;
        }
    }

    m_dependencies.push_back(dependency);
}

auto task_t::has_completed_dependencies() const -> bool
{
    for (const auto& dependency : m_dependencies) {
        if (!dependency || (!dependency->is_completed() && !dependency->is_failed())) {
            return false;
        }
    }
    return true;
}

// Private helper methods

auto task_t::validate_state_transition(task_state_e new_state) const -> void
{
    // Define valid state transitions
    bool valid = false;

    switch (m_state) {
        case task_state_e::pending:
            valid = (new_state == task_state_e::running ||
                    new_state == task_state_e::completed ||
                    new_state == task_state_e::failed);
            break;

        case task_state_e::running:
            valid = (new_state == task_state_e::suspended ||
                    new_state == task_state_e::completed ||
                    new_state == task_state_e::failed);
            break;

        case task_state_e::suspended:
            valid = (new_state == task_state_e::running ||
                    new_state == task_state_e::suspended ||
                    new_state == task_state_e::completed ||
                    new_state == task_state_e::failed);
            break;

        case task_state_e::completed:
        case task_state_e::failed:
            // Terminal states - no transitions allowed
            valid = false;
            break;
    }

    if (!valid) {
        throw std::runtime_error("Invalid task state transition from " +
                                format_state_string() + " to " +
                                "new_state");
    }
}

auto task_t::notify_dependents() -> void
{
    // In a more complete implementation, this would notify other tasks
    // that are waiting on this task to complete
    // For now, this is a placeholder
}

auto task_t::cleanup_resources() -> void
{
    // Clean up any resources held by the task
    // Clear dependencies to avoid circular references
    m_dependencies.clear();

    // The promise and result objects will be cleaned up by shared_ptr
}

auto task_t::format_state_string() const -> std::string
{
    switch (m_state) {
        case task_state_e::pending: return "pending";
        case task_state_e::running: return "running";
        case task_state_e::suspended: return "suspended";
        case task_state_e::completed: return "completed";
        case task_state_e::failed: return "failed";
        default: return "unknown";
    }
}

} // namespace zephyr
