#include "args.hpp"

namespace zephyr
{

auto command_line_args_t::parse(int argc, char* argv[]) -> command_line_args_t
{
    command_line_args_t args;
    args.program_name = argv[0];
    std::vector<std::string> tokens(argv + 1, argv + argc);

    if (tokens.empty())
    {
        args.mode = execution_mode_t::REPL;
    }
    else if (tokens.size() == 1)
    {
        if (tokens[0] == "-h" || tokens[0] == "--help")
        {
            args.mode = execution_mode_t::SHOW_HELP;
        }
        else
        {
            args.mode = execution_mode_t::EXECUTE_FILE;
            args.input_value = tokens[0];
        }
    }
    else if (tokens.size() == 2 && tokens[0] == "-c")
    {
        args.mode = execution_mode_t::EXECUTE_STRING;
        args.input_value = tokens[1];
    }
    // else: mode remains INVALID by default

    return args;
}

} // namespace zephyr
