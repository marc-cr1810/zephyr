#include "zephyr/async_scheduler.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/objects/list_object.hpp"
#include "zephyr/errors.hpp"
#include <algorithm>
#include <iostream>

namespace zephyr
{

auto async_scheduler_t::instance() -> async_scheduler_t&
{
    static async_scheduler_t scheduler;
    return scheduler;
}

async_scheduler_t::async_scheduler_t()
    : m_next_task_id(1), m_current_task(nullptr), m_should_yield(false)
{
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
    // Create a new task and add it to the ready queue (don't execute immediately)
    int task_id = m_next_task_id++;
    auto task = std::make_shared<task_t>(task_id, func);
    
    // Add to task tracking and ready queue
    m_all_tasks[task_id] = task;
    m_ready_queue.push(task);
    
    log_task_state(task_id, "spawned");
    
    // Return the task's promise (which is still pending)
    return task->m_promise;
}

auto async_scheduler_t::run_until_complete() -> void
{
    while (has_pending_tasks()) {
        run_one_iteration();
    }
}

auto async_scheduler_t::run_one_iteration() -> void
{
    // Move suspended tasks that are ready back to the ready queue
    schedule_ready_tasks();
    
    // Round-robin execution: run a slice of each ready task
    std::vector<std::shared_ptr<task_t>> tasks_to_process;
    
    // Collect all ready tasks
    while (!m_ready_queue.empty()) {
        tasks_to_process.push_back(m_ready_queue.front());
        m_ready_queue.pop();
    }
    
    // Execute each task with yielding (they may suspend and resume)
    for (auto& task : tasks_to_process) {
        if (task && task->is_ready()) {
            execute_task_with_yielding(task);
            
            // If task is still running/suspended, put it back in queue
            if (!task->is_completed() && !task->is_failed()) {
                if (task->m_state == task_state_e::suspended) {
                    m_suspended_tasks.push_back(task);
                } else {
                    m_ready_queue.push(task);
                }
            }
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

    // If promise is already resolved, return immediately
    if (promise->is_fulfilled()) {
        return promise->m_result;
    } else if (promise->is_rejected()) {
        throw value_error_t("Promise rejected: " + promise->m_error_message);
    }
    
    // Promise is still pending - run scheduler until it resolves
    while (promise->is_pending() && has_pending_tasks()) {
        run_one_iteration();
    }
    
    // Check final state
    if (promise->is_fulfilled()) {
        return promise->m_result;
    } else if (promise->is_rejected()) {
        throw value_error_t("Promise rejected: " + promise->m_error_message);
    } else {
        // Promise is still pending but no more tasks - return none
        return std::make_shared<none_object_t>();
    }
}

auto async_scheduler_t::yield_execution() -> void
{
    m_should_yield = true;
}

auto async_scheduler_t::run_until_promise_resolved(std::shared_ptr<promise_object_t> promise) -> void
{
    while (promise->is_pending() && has_pending_tasks()) {
        run_one_iteration();
    }
}

auto async_scheduler_t::create_cooperative_task(std::function<std::shared_ptr<object_t>()> func) -> std::shared_ptr<task_t>
{
    int task_id = m_next_task_id++;
    auto task = std::make_shared<task_t>(task_id, func);
    return task;
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

    // Run scheduler until all promises are resolved
    bool all_resolved = false;
    int iteration = 0;
    while (!all_resolved && has_pending_tasks()) {
        run_one_iteration();
        
        // Check if all promises are now resolved
        all_resolved = true;
        for (const auto& promise : promises) {
            if (promise->is_pending()) {
                all_resolved = false;
                break;
            }
        }
    }

    // Collect results from all promises
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
            // Promise is still pending - this shouldn't happen if we ran the scheduler properly
            // but handle it gracefully
            results.push_back(std::make_shared<none_object_t>());
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
    // Move tasks from suspended list to ready queue if they're ready
    auto it = m_suspended_tasks.begin();
    while (it != m_suspended_tasks.end()) {
        if ((*it)->is_ready()) {
            m_ready_queue.push(*it);
            it = m_suspended_tasks.erase(it);
        } else {
            ++it;
        }
    }
}

auto async_scheduler_t::wake_up_waiting_tasks() -> void
{
    // Check for tasks that might be waiting on completed dependencies
    for (auto& [task_id, task] : m_all_tasks) {
        if (task->m_state == task_state_e::suspended && task->has_completed_dependencies()) {
            task->m_state = task_state_e::pending;
            
            // Move from suspended to ready queue
            auto it = std::find(m_suspended_tasks.begin(), m_suspended_tasks.end(), task);
            if (it != m_suspended_tasks.end()) {
                m_suspended_tasks.erase(it);
                m_ready_queue.push(task);
            }
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
    execute_task_with_yielding(task);
}

auto async_scheduler_t::execute_task_with_yielding(std::shared_ptr<task_t> task) -> void
{
    if (!task || task->m_state == task_state_e::completed || task->m_state == task_state_e::failed) {
        return;
    }

    try {
        // Set current task for cooperative yielding
        set_current_task(task);
        m_should_yield = false;
        
        // Execute the task with yield handling
        task->execute();
        
        // If task completed, mark it as such
        if (task->is_completed()) {
            log_task_state(task->m_task_id, "completed");
        }
        
        // Clear current task
        set_current_task(nullptr);
        
    } catch (const yield_exception_t& e) {
        // Task yielded - it's now suspended
        task->suspend();
        log_task_state(task->m_task_id, "yielded");
        set_current_task(nullptr);
    } catch (const std::exception& e) {
        fail_task(task->m_task_id, e.what());
        set_current_task(nullptr);
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
    // Disable debug output for clean interleaving display
    (void)task_id;  // Suppress unused parameter warning
    (void)state;    // Suppress unused parameter warning
}

auto async_scheduler_t::should_yield_execution() const -> bool
{
    return m_should_yield;
}

auto async_scheduler_t::set_current_task(std::shared_ptr<task_t> task) -> void
{
    m_current_task = task;
}

auto async_scheduler_t::get_current_task() const -> std::shared_ptr<task_t>
{
    return m_current_task;
}

} // namespace zephyr