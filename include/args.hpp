#pragma once

#include <string>
#include <vector>

namespace zephyr
{

enum class execution_mode_t
{
    REPL,
    EXECUTE_FILE,
    EXECUTE_STRING,
    SHOW_HELP,
    INVALID
};

struct command_line_args_t
{
    execution_mode_t mode = execution_mode_t::INVALID;
    std::string input_value; // Can be a file path or a source string
    std::string program_name;

    static auto parse(int argc, char* argv[]) -> command_line_args_t;
};

} // namespace zephyr
