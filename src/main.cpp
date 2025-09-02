#include "runtime.hpp"
#include <iostream>
#include <string>

auto main(int argc, char* argv[]) -> int
{
    zephyr::runtime_t runtime;

    if (argc == 1)
    {
        runtime.start_repl();
    }
    else if (argc == 2)
    {
        return runtime.execute_file(argv[1]);
    }
    else if (argc == 3 && std::string(argv[1]) == "-c")
    {
        return runtime.execute_string(argv[2], "<string>");
    }
    else
    {
        std::cerr << "Usage: " << argv[0] << " [path/to/file.zephyr]" << std::endl;
        std::cerr << "       " << argv[0] << " -c \"<source code>\"" << std::endl;
        std::cerr << "       " << argv[0] << std::endl;
        return 1;
    }

    return 0;
}