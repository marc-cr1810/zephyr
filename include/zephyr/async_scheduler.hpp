#pragma once

#include "zephyr/task.hpp"
#include "objects/promise_object.hpp"
#include <memory>
#include <queue>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>

namespace zephyr
{

// Exception thrown when a task needs to yield control
class yield_exception_t : public std::runtime_error
{
public:
    yield_exception_t() : std::runtime_error("Task yielding") {}
};
class async_scheduler_t
{
public:
    static auto instance() -> async_scheduler_t&;

    // Task management
    auto create_task(std::function<std::shared_ptr<object_t>()> func) -> std::shared_ptr<task_t>;
    auto spawn_task(std::function<std::shared_ptr<object_t>()> func) -> std::shared_ptr<promise_object_t>;

    // Scheduler operations
    auto run_until_complete() -> void;
    auto run_one_iteration() -> void;
    auto has_pending_tasks() const -> bool;

    // Await support
    auto await_promise(std::shared_ptr<promise_object_t> promise) -> std::shared_ptr<object_t>;

    // Cooperative execution support
    auto yield_execution() -> void;
    auto run_until_promise_resolved(std::shared_ptr<promise_object_t> promise) -> void;
    auto create_cooperative_task(std::function<std::shared_ptr<object_t>()> func) -> std::shared_ptr<task_t>;
    auto should_yield_execution() const -> bool;
    auto get_current_task() const -> std::shared_ptr<task_t>;
    auto execute_task_with_yielding(std::shared_ptr<task_t> task) -> void;

    // Promise utilities
    auto create_resolved_promise(std::shared_ptr<object_t> value) -> std::shared_ptr<promise_object_t>;
    auto create_rejected_promise(const std::string& error) -> std::shared_ptr<promise_object_t>;
    auto all(const std::vector<std::shared_ptr<promise_object_t>>& promises) -> std::shared_ptr<promise_object_t>;

    // Task tracking
    auto register_task(std::shared_ptr<task_t> task) -> void;
    auto complete_task(int task_id) -> void;
    auto fail_task(int task_id, const std::string& error) -> void;

    // Reset scheduler state
    auto reset() -> void;

private:
    async_scheduler_t();

    std::queue<std::shared_ptr<task_t>> m_ready_queue;
    std::vector<std::shared_ptr<task_t>> m_suspended_tasks;
    std::unordered_map<int, std::shared_ptr<task_t>> m_all_tasks;
    int m_next_task_id;

    // Current running task for cooperative yielding
    std::shared_ptr<task_t> m_current_task;
    bool m_should_yield;

    // Helper methods
    auto schedule_ready_tasks() -> void;
    auto wake_up_waiting_tasks() -> void;
    auto next_ready_task() -> std::shared_ptr<task_t>;
    auto execute_task(std::shared_ptr<task_t> task) -> void;
    auto validate_task(std::shared_ptr<task_t> task) -> void;
    auto cleanup_completed_tasks() -> void;
    auto log_task_state(int task_id, const std::string& state) -> void;

    // Cooperative execution helpers
    auto set_current_task(std::shared_ptr<task_t> task) -> void;
};

}
