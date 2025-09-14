#include "zephyr/runtime.hpp"
#include "zephyr/args.hpp"
#include "zephyr/zephyr.hpp"
#include <iostream>

auto show_usage(const std::string& program_name) -> void
{
    std::cerr << "Usage: " << program_name << " [path/to/file.zephyr]" << std::endl;
    std::cerr << "       " << program_name << " -c \"<source code>\"" << std::endl;
    std::cerr << "       " << program_name << " [-h|--help]" << std::endl;
    std::cerr << "       " << program_name << " (for REPL mode)" << std::endl;
}

auto main(int argc, char* argv[]) -> int
{
    zephyr::runtime_t runtime;
    const auto args = zephyr::command_line_args_t::parse(argc, argv);

    switch (args.mode)
    {
        case zephyr::execution_mode_t::REPL:
            runtime.start_repl();
            break;
        case zephyr::execution_mode_t::EXECUTE_FILE:
            return runtime.execute_file(args.input_value);
        case zephyr::execution_mode_t::EXECUTE_STRING:
            return runtime.execute_string(args.input_value, "<string>");
        case zephyr::execution_mode_t::SHOW_HELP:
            show_usage(args.program_name);
            break;
        case zephyr::execution_mode_t::SHOW_VERSION:
            std::cout << "Zephyr Interpreter, version " << ZEPHYR_VERSION_STRING << std::endl;
            break;
        case zephyr::execution_mode_t::INVALID:
            show_usage(args.program_name);
            return 1;
    }

    return 0;
}
