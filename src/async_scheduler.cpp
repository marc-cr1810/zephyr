#include "async_scheduler.hpp"
#include "objects/none_object.hpp"
#include "objects/list_object.hpp"
#include "errors.hpp"
#include <algorithm>

namespace zephyr
{

auto async_scheduler_t::instance() -> async_scheduler_t&
{
    static async_scheduler_t scheduler;
    return scheduler;
}

auto async_scheduler_t::create_task(std::function<std::shared_ptr<object_t>()> func) -> std::shared_ptr<task_t>
{
    int task_id = m_next_task_id++;
    auto task = std::make_shared<task_t>(task_id, func);
    register_task(task);
    return task;
}

auto async_scheduler_t::spawn_task(std::function<std::shared_ptr<object_t>()> func) -> std::shared_ptr<promise_object_t>
{
    // For now, execute tasks immediately and return resolved promises
    // This simplifies the implementation while maintaining the async interface
    try {
        auto result = func();

        // Check if the result is already a Promise (from async function calls)
        auto existing_promise = std::dynamic_pointer_cast<promise_object_t>(result);
        if (existing_promise) {
            // If it's already a Promise, return it directly to avoid double wrapping
            return existing_promise;
        }

        // Create a resolved promise with the result
        return create_resolved_promise(result);
    } catch (const std::exception& e) {
        // Create a rejected promise with the error
        return create_rejected_promise(e.what());
    }
}

auto async_scheduler_t::run_until_complete() -> void
{
    while (has_pending_tasks()) {
        run_one_iteration();
    }
}

auto async_scheduler_t::run_one_iteration() -> void
{
    if (m_ready_queue.empty()) {
        schedule_ready_tasks();
    }

    if (!m_ready_queue.empty()) {
        auto task = next_ready_task();
        if (task) {
            execute_task(task);
        }
    }

    wake_up_waiting_tasks();
    cleanup_completed_tasks();
}

auto async_scheduler_t::has_pending_tasks() const -> bool
{
    return !m_ready_queue.empty() || !m_suspended_tasks.empty();
}

auto async_scheduler_t::await_promise(std::shared_ptr<promise_object_t> promise) -> std::shared_ptr<object_t>
{
    if (!promise) {
        throw value_error_t("Cannot await null promise");
    }

    // For immediate execution mode, just return the promise result
    if (promise->is_fulfilled()) {
        return promise->m_result;
    } else if (promise->is_rejected()) {
        throw value_error_t("Promise rejected: " + promise->m_error_message);
    } else {
        // Promise is still pending - for now, we'll return none
        // In a full implementation, this would suspend the current task
        return std::make_shared<none_object_t>();
    }
}

auto async_scheduler_t::create_resolved_promise(std::shared_ptr<object_t> value) -> std::shared_ptr<promise_object_t>
{
    auto promise = std::make_shared<promise_object_t>();
    promise->resolve(value ? value : std::make_shared<none_object_t>());
    return promise;
}

auto async_scheduler_t::create_rejected_promise(const std::string& error) -> std::shared_ptr<promise_object_t>
{
    auto promise = std::make_shared<promise_object_t>();
    promise->reject(error);
    return promise;
}

auto async_scheduler_t::all(const std::vector<std::shared_ptr<promise_object_t>>& promises) -> std::shared_ptr<promise_object_t>
{
    auto result_promise = std::make_shared<promise_object_t>();

    if (promises.empty()) {
        // Return a resolved promise with an empty list
        auto empty_list = std::make_shared<list_object_t>();
        result_promise->resolve(empty_list);
        return result_promise;
    }

    // Check if all promises are already resolved
    std::vector<std::shared_ptr<object_t>> results;
    results.reserve(promises.size());

    for (const auto& promise : promises) {
        if (promise->is_fulfilled()) {
            results.push_back(promise->m_result);
        } else if (promise->is_rejected()) {
            // If any promise is rejected, reject the all promise
            result_promise->reject(promise->m_error_message);
            return result_promise;
        } else {
            // Promise is still pending - for now, we'll just return a pending promise
            // In a full implementation, this would wait for all promises to complete
            return result_promise; // Returns a pending promise
        }
    }

    // All promises are fulfilled, create result list
    auto result_list = std::make_shared<list_object_t>(results);
    result_promise->resolve(result_list);
    return result_promise;
}

auto async_scheduler_t::register_task(std::shared_ptr<task_t> task) -> void
{
    if (!task) {
        throw value_error_t("Cannot register null task");
    }

    validate_task(task);
    m_all_tasks[task->m_task_id] = task;
    m_ready_queue.push(task);

    log_task_state(task->m_task_id, "registered");
}

auto async_scheduler_t::complete_task(int task_id) -> void
{
    auto it = m_all_tasks.find(task_id);
    if (it != m_all_tasks.end()) {
        auto task = it->second;
        task->m_state = task_state_e::completed;
        log_task_state(task_id, "completed");

        // Remove from suspended tasks if present
        m_suspended_tasks.erase(
            std::remove(m_suspended_tasks.begin(), m_suspended_tasks.end(), task),
            m_suspended_tasks.end()
        );

        wake_up_waiting_tasks();
    }
}

auto async_scheduler_t::fail_task(int task_id, const std::string& error) -> void
{
    auto it = m_all_tasks.find(task_id);
    if (it != m_all_tasks.end()) {
        auto task = it->second;
        task->m_state = task_state_e::failed;
        task->m_error_message = error;
        log_task_state(task_id, "failed: " + error);

        // Remove from suspended tasks if present
        m_suspended_tasks.erase(
            std::remove(m_suspended_tasks.begin(), m_suspended_tasks.end(), task),
            m_suspended_tasks.end()
        );
    }
}

auto async_scheduler_t::reset() -> void
{
    // Clear all queues and reset state
    while (!m_ready_queue.empty()) {
        m_ready_queue.pop();
    }

    m_suspended_tasks.clear();
    m_all_tasks.clear();
    m_next_task_id = 1;
}

// Private helper methods

auto async_scheduler_t::schedule_ready_tasks() -> void
{
    for (auto& task : m_suspended_tasks) {
        if (task->is_ready()) {
            m_ready_queue.push(task);
        }
    }

    // Remove ready tasks from suspended list
    m_suspended_tasks.erase(
        std::remove_if(m_suspended_tasks.begin(), m_suspended_tasks.end(),
            [](const std::shared_ptr<task_t>& task) {
                return task->is_ready();
            }),
        m_suspended_tasks.end()
    );
}

auto async_scheduler_t::wake_up_waiting_tasks() -> void
{
    // Check for tasks that might be waiting on completed dependencies
    for (auto& [task_id, task] : m_all_tasks) {
        if (task->m_state == task_state_e::suspended && task->has_completed_dependencies()) {
            task->m_state = task_state_e::pending;
            m_ready_queue.push(task);
        }
    }
}

auto async_scheduler_t::next_ready_task() -> std::shared_ptr<task_t>
{
    if (m_ready_queue.empty()) {
        return nullptr;
    }

    auto task = m_ready_queue.front();
    m_ready_queue.pop();
    return task;
}

auto async_scheduler_t::execute_task(std::shared_ptr<task_t> task) -> void
{
    if (!task || task->m_state == task_state_e::completed || task->m_state == task_state_e::failed) {
        return;
    }

    try {
        task->execute();
    } catch (const std::exception& e) {
        fail_task(task->m_task_id, e.what());
    }
}

auto async_scheduler_t::validate_task(std::shared_ptr<task_t> task) -> void
{
    if (!task) {
        throw value_error_t("Task cannot be null");
    }

    if (m_all_tasks.find(task->m_task_id) != m_all_tasks.end()) {
        throw value_error_t("Task with ID " + std::to_string(task->m_task_id) + " already exists");
    }
}

auto async_scheduler_t::cleanup_completed_tasks() -> void
{
    // Remove completed and failed tasks from all_tasks map periodically
    // This prevents memory leaks in long-running applications
    auto it = m_all_tasks.begin();
    while (it != m_all_tasks.end()) {
        if (it->second->is_completed() || it->second->is_failed()) {
            it = m_all_tasks.erase(it);
        } else {
            ++it;
        }
    }
}

auto async_scheduler_t::log_task_state(int task_id, const std::string& state) -> void
{
    // For debugging - could be expanded to use a proper logging system
    // Currently just a placeholder for future logging implementation
    (void)task_id;  // Suppress unused parameter warning
    (void)state;    // Suppress unused parameter warning
}

} // namespace zephyr
