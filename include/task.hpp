#pragma once

#include "objects/objects.hpp"
#include <memory>
#include <functional>
#include <vector>

namespace zephyr
{
    enum class task_state_e
    {
        pending,
        running,
        suspended,
        completed,
        failed
    };

    class task_t
    {
    public:
        auto execute() -> void;
        auto suspend() -> void;
        auto resume(std::shared_ptr<object_t> value) -> void;
        auto complete(std::shared_ptr<object_t> value) -> void;
        auto fail(const std::string& error) -> void;

        auto is_ready() const -> bool;
        auto is_completed() const -> bool;
        auto is_failed() const -> bool;
        auto is_suspended() const -> bool;

        auto add_dependency(std::shared_ptr<task_t> dependency) -> void;
        auto has_completed_dependencies() const -> bool;

        explicit task_t(int id, std::function<std::shared_ptr<object_t>()> func);
        ~task_t() = default;

    public:
        int m_task_id;
        task_state_e m_state;
        std::shared_ptr<promise_object_t> m_promise;
        std::shared_ptr<object_t> m_result;
        std::string m_error_message;

        // Function to execute
        std::function<std::shared_ptr<object_t>()> m_execution_function;

        // Continuation function (for await resumption)
        std::function<void(std::shared_ptr<object_t>)> m_continuation;

        // Dependencies (tasks this task is awaiting)
        std::vector<std::shared_ptr<task_t>> m_dependencies;

    private:
        auto validate_state_transition(task_state_e new_state) const -> void;
        auto notify_dependents() -> void;
        auto cleanup_resources() -> void;
        auto format_state_string() const -> std::string;
    };
}
