#pragma once

#include "zephyr/interpreter.hpp"
#include "zephyr/async_scheduler.hpp"
#include "zephyr/ast.hpp"

#include <string>
#include <memory>
#include <vector>

namespace zephyr
{

// Forward declarations
class module_loader_t;
class module_t;

namespace api {
    class engine_t;
}

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

    // Module system functions
    auto initialize_module_system() -> void;
    auto create_main_module(const std::string& file_path, const std::string& source_code) -> std::shared_ptr<module_t>;

    // Variables
    async_scheduler_t& m_scheduler;
    std::vector<std::unique_ptr<program_t>> m_alive_programs; // For REPL object lifetime

public:
    // Module system access
    auto get_module_loader() -> std::shared_ptr<module_loader_t>;
    
private:
    // Module system variables
    std::shared_ptr<module_loader_t> m_module_loader;
};

} // namespace zephyr
