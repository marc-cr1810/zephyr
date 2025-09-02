#pragma once

#include "interpreter.hpp"
#include "async_scheduler.hpp"
#include "ast.hpp"

#include <string>
#include <memory>
#include <vector>

namespace zephyr
{

class runtime_t
{
public:
    // Functions
    runtime_t();
    auto execute_file(const std::string& path) -> int;
    auto execute_string(const std::string& source, const std::string& context_name) -> int;
    auto start_repl() -> void;

private:
    // Functions
    auto process_code(
        zephyr::interpreter_t& interpreter,
        const std::string& source,
        const std::string& filename
    ) -> bool;

    auto process_code_repl(
        zephyr::interpreter_t& interpreter,
        const std::string& source,
        const std::string& full_source_code
    ) -> bool;

    auto print_error(
        const std::string& message,
        const std::string& error_type,
        const std::string& source_code,
        int line,
        int column,
        const std::string& filename = "",
        int length = 1
    ) -> void;

    // Variables
    async_scheduler_t& m_scheduler;
    std::vector<std::unique_ptr<program_t>> m_alive_programs; // For REPL object lifetime
};

} // namespace zephyr
