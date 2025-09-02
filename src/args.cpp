#include "args.hpp"
#include <vector>

namespace zephyr
{

auto command_line_args_t::parse(int argc, char* argv[]) -> command_line_args_t
{
    command_line_args_t args;
    args.program_name = argv[0];
    std::vector<std::string> tokens(argv + 1, argv + argc);

    // No arguments means REPL mode
    if (tokens.empty())
    {
        args.mode = execution_mode_t::REPL;
        return args;
    }

    // Loop through tokens to find flags. This is more extensible.
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const std::string& token = tokens[i];

        if (token == "-h" || token == "--help")
        {
            args.mode = execution_mode_t::SHOW_HELP;
            return args; // Help is an exclusive action
        }
        else if (token == "-c")
        {
            // -c requires an argument and should be the only primary mode
            if (i + 1 < tokens.size() && tokens.size() == 2)
            {
                args.mode = execution_mode_t::EXECUTE_STRING;
                args.input_value = tokens[i + 1];
                return args;
            }
            else
            {
                args.mode = execution_mode_t::INVALID;
                return args; // Error: -c needs one argument or has extra args
            }
        }
        // To add a new flag like --version, you would just add another `else if` here.
    }

    // If we've looped through all tokens and didn't find a specific flag,
    // we assume the argument is a positional file path.
    if (tokens.size() == 1)
    {
        args.mode = execution_mode_t::EXECUTE_FILE;
        args.input_value = tokens[0];
        return args;
    }

    // If we have more than one token and none of the flag conditions were met,
    // the arguments are invalid.
    args.mode = execution_mode_t::INVALID;
    return args;
}

} // namespace zephyr