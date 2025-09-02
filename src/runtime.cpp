#include "runtime.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "errors.hpp"
#include "runtime_error.hpp"
#include "types/type.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

namespace zephyr
{

// ANSI escape codes for colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

runtime_t::runtime_t()
    : m_scheduler(async_scheduler_t::instance())
{
}

auto runtime_t::execute_file(const std::string& filename) -> int
{
    if (filename.rfind(".zephyr") == std::string::npos)
    {
        print_error("File must have a .zephyr extension.", "Error", "", 0, 0);
        return 1;
    }

    std::ifstream file(filename);
    if (!file.is_open())
    {
        print_error("Could not open file " + filename, "Error", "", 0, 0);
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();

    zephyr::interpreter_t interpreter(filename, source_code);
    bool success = process_code(interpreter, source_code, filename);
    return success ? 0 : 1;
}

auto runtime_t::execute_string(const std::string& source, const std::string& context_name) -> int
{
    zephyr::interpreter_t interpreter(context_name, source);
    bool success = process_code(interpreter, source, context_name);
    return success ? 0 : 1;
}

auto runtime_t::start_repl() -> void
{
    std::string line;
    std::string accumulated_input;
    std::cout << "Zephyr REPL (Ctrl+D to exit)" << std::endl;
    
    zephyr::interpreter_t interpreter("<stdin>", accumulated_input);

    while (true)
    {
        std::cout << (accumulated_input.empty() ? ">> " : ".. ");
        if (!std::getline(std::cin, line))
        {
            break; // Exit on Ctrl+D or EOF
        }

        accumulated_input += line + "\n";

        if (process_code_repl(interpreter, accumulated_input, accumulated_input))
        {
            accumulated_input.clear(); // Clear on successful processing or error
        }
    }
}

auto runtime_t::process_code(zephyr::interpreter_t& interpreter, const std::string& source, const std::string& filename) -> bool
{
    try
    {
        zephyr::lexer_t lexer(source);
        zephyr::parser_t parser(lexer);
        std::unique_ptr<zephyr::program_t> program_node = parser.parse();

        interpreter.interpret(*program_node);

        // If the last statement was an expression, print its result
        if (!program_node->statements.empty())
        {
            if (auto expr_stmt = dynamic_cast<zephyr::expression_statement_t*>(program_node->statements.back().get()))
            {
                std::shared_ptr<zephyr::object_t> result = interpreter.get_current_result();
                if (result && result->get_type()->get_name() != "none")
                {
                    std::cout << result->to_string() << std::endl;
                }
            }
        }

        return true;
    }
    catch (const zephyr::syntax_error_t& e)
    {
        print_error(e.what(), e.error_name(), source, e.line(), e.column(), filename, e.length());
        return false;
    }
    catch (const zephyr::runtime_error_with_location_t& e)
    {
        print_error(e.what(), e.error_name(), source, e.line(), e.column(), filename, e.length());
        return false;
    }
    catch (const std::runtime_error& e)
    {
        print_error(e.what(), "Error", source, 0, 0, filename);
        return false;
    }
    return true;
}

auto runtime_t::process_code_repl(zephyr::interpreter_t& interpreter, const std::string& source, const std::string& full_source_code) -> bool
{
    try
    {
        zephyr::lexer_t lexer(source);
        zephyr::parser_t parser(lexer);
        std::unique_ptr<zephyr::program_t> program_node = parser.parse();

        interpreter.interpret(*program_node);

        bool has_class_definition = false;
        for (const auto& stmt : program_node->statements)
        {
            if (dynamic_cast<zephyr::class_definition_t*>(stmt.get()))
            {
                has_class_definition = true;
                break;
            }
        }

        // If the last statement was an expression, print its result in REPL mode
        if (!program_node->statements.empty())
        {
            if (auto expr_stmt = dynamic_cast<zephyr::expression_statement_t*>(program_node->statements.back().get()))
            {
                std::shared_ptr<zephyr::object_t> result = interpreter.get_current_result();
                // Only print if there's a result and it's not a NoneObject
                if (result && result->get_type()->get_name() != "none")
                {
                    std::cout << result->to_string() << std::endl;
                }
            }
        }

        if (has_class_definition)
        {
            m_alive_programs.push_back(std::move(program_node));
        }

        return true;
    }
    catch (const zephyr::syntax_error_t& e)
    {
        if (std::string(e.what()).find("Unexpected end of file") != std::string::npos)
        {
            return false; // Incomplete input
        }
        print_error(e.what(), e.error_name(), full_source_code, e.line(), e.column());
        return true; // Error occurred, clear input
    }
    catch (const zephyr::runtime_error_with_location_t& e)
    {
        print_error(e.what(), e.error_name(), full_source_code, e.line(), e.column(), "", e.length());
        return true; // Error occurred, clear input
    }
    catch (const std::runtime_error& e)
    {
        print_error(e.what(), "Error", full_source_code, 0, 0);
        return true; // Error occurred, clear input
    }
    return true;
}

auto runtime_t::print_error(const std::string& message, const std::string& error_type, const std::string& source_code, int line, int column, const std::string& filename, int length) -> void
{
    std::cerr << "Traceback (most recent call last):" << std::endl;

    if (!filename.empty() && line > 0)
    {
        std::cerr << "  File \"" << filename << "\", line " << line << std::endl;
    }
    else if (filename.empty() && line > 0)
    {
        std::cerr << "  File \"<stdin>\", line " << line << std::endl;
    }

    if (line > 0 && !source_code.empty())
    {
        std::istringstream iss(source_code);
        std::string current_line_content;
        int current_line_num = 0;

        while (std::getline(iss, current_line_content))
        {
            current_line_num++;
            if (current_line_num == line)
            {
                break;
            }
        }

        if (current_line_num == line && !current_line_content.empty())
        {
            std::string trimmed_line = current_line_content;
            size_t first_non_space = trimmed_line.find_first_not_of(" 	");
            if (first_non_space != std::string::npos) {
                trimmed_line = trimmed_line.substr(first_non_space);
            }

            std::cerr << "    " << trimmed_line << std::endl;

            int visual_column = 4;
            int adjusted_column = std::max(1, column);

            if (first_non_space != std::string::npos) {
                adjusted_column = std::max(1, adjusted_column - (int)first_non_space);
            }

            for (int i = 0; i < std::min(adjusted_column - 1, (int)trimmed_line.length()); ++i)
            {
                if (trimmed_line[i] == '\t')
                {
                    visual_column += 8 - ((visual_column - 4) % 8);
                }
                else
                {
                    visual_column++;
                }
            }

            std::cerr << "    ";
            for (int i = 4; i < visual_column; ++i)
            {
                std::cerr << " ";
            }

            int effective_length = std::max(1, std::min(length, (int)trimmed_line.length() - (adjusted_column - 1)));
            std::cerr << ANSI_COLOR_RED << "^" << ANSI_COLOR_RESET;
            for (int i = 1; i < effective_length; ++i)
            {
                std::cerr << ANSI_COLOR_RED << "~" << ANSI_COLOR_RESET;
            }
            std::cerr << std::endl;
        }
    }

    std::cerr << ANSI_COLOR_RED << error_type << ": " << message << ANSI_COLOR_RESET << std::endl;
}

} // namespace zephyr
