#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include "ast.hpp"
#include "errors.hpp"

// ANSI escape codes for colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Function to print errors in red with context
auto print_error(const std::string& message, const std::string& error_type, const std::string& source_code, int line, int column, const std::string& filename = "", int length = 1) -> void
{
    // Python-style traceback header
    std::cerr << "Traceback (most recent call last):" << std::endl;

    // File and line information
    if (!filename.empty() && line > 0)
    {
        std::cerr << "  File \"" << filename << "\", line " << line << std::endl;
    }
    else if (filename.empty() && line > 0)
    {
        std::cerr << "  File \"<stdin>\", line " << line << std::endl;
    }

    // Show source code line if available
    if (line > 0 && !source_code.empty())
    {
        std::istringstream iss(source_code);
        std::string current_line_content;
        int current_line_num = 0;

        // Read lines until we reach the target line
        while (std::getline(iss, current_line_content))
        {
            current_line_num++;
            if (current_line_num == line)
            {
                break;
            }
        }

        // Show the source line if found
        if (current_line_num == line && !current_line_content.empty())
        {
            // Trim leading whitespace for display
            std::string trimmed_line = current_line_content;
            size_t first_non_space = trimmed_line.find_first_not_of(" \t");
            if (first_non_space != std::string::npos) {
                trimmed_line = trimmed_line.substr(first_non_space);
            }

            std::cerr << "    " << trimmed_line << std::endl;

            // Calculate visual column position for error indicator
            int visual_column = 4; // "    " prefix
            int adjusted_column = std::max(1, column);

            // Adjust for trimmed whitespace
            if (first_non_space != std::string::npos) {
                adjusted_column = std::max(1, adjusted_column - (int)first_non_space);
            }

            // Calculate visual position accounting for tabs
            for (int i = 0; i < std::min(adjusted_column - 1, (int)trimmed_line.length()); ++i)
            {
                if (trimmed_line[i] == '\t')
                {
                    visual_column += 8 - ((visual_column - 4) % 8); // Tab alignment
                }
                else
                {
                    visual_column++;
                }
            }

            // Print error indicator with spaces and caret followed by tildes
            std::cerr << "    ";
            for (int i = 4; i < visual_column; ++i)
            {
                std::cerr << " ";
            }

            // Print error indicator - start with ^ and trail with ~
            int effective_length = std::max(1, std::min(length, (int)trimmed_line.length() - (adjusted_column - 1)));
            std::cerr << ANSI_COLOR_RED << "^" << ANSI_COLOR_RESET;
            for (int i = 1; i < effective_length; ++i)
            {
                std::cerr << ANSI_COLOR_RED << "~" << ANSI_COLOR_RESET;
            }
            std::cerr << std::endl;
        }
    }

    // Python-style error type and message
    std::cerr << ANSI_COLOR_RED << error_type << ": " << message << ANSI_COLOR_RESET << std::endl;
}

// Function to process a single line of code with a persistent interpreter
// Returns true if parsing was successful and input was consumed, false otherwise (e.g., incomplete input)
auto process_code(const std::string& code_to_process, zephyr::interpreter_t& interpreter, const std::string& full_source_code, const std::string& filename = "") -> bool
{
    try
    {
        zephyr::lexer_t lexer(code_to_process);
        zephyr::parser_t parser(lexer);
        std::unique_ptr<zephyr::program_t> program_node = parser.parse();

        interpreter.interpret(*program_node); // Always interpret the program first

        // If the last statement was an expression, print its result in REPL mode
        if (program_node->statements.size() == 1)
        {
            if (auto expr_stmt = dynamic_cast<zephyr::expression_statement_t*>(program_node->statements[0].get()))
            {
                std::shared_ptr<zephyr::object_t> result = interpreter.get_current_result();
                // Only print if there's a result and it's not a NoneObject
                if (result && result->get_type()->get_name() != "none")
                {
                    std::cout << result->to_string() << std::endl;
                }
            }
        }
        return true;

    }
    catch (const zephyr::parsing_error_t& e)
    {
        // Check if the error is due to unexpected EOF, meaning more input is needed
        if (std::string(e.what()).find("Unexpected end of file") != std::string::npos)
        {
            return false; // Incomplete input
        }
        print_error(e.what(), "SyntaxError", full_source_code, e.line, e.column, filename);
        return false; // Error occurred
    }
    catch (const zephyr::runtime_error_with_location_t& e)
    {
        print_error(e.what(), e.error_name, full_source_code, e.line, e.column, filename, e.length);
        return false; // Error occurred
    }
    catch (const std::runtime_error& e)
    {
        print_error(e.what(), "Error", full_source_code, 0, 0, filename);
        return false; // Error occurred
    }
    return true; // Success
}

// REPL-specific version that keeps class definition Programs alive
auto process_code_repl(const std::string& code_to_process, zephyr::interpreter_t& interpreter, const std::string& full_source_code, std::vector<std::unique_ptr<zephyr::program_t>>& alive_programs) -> bool
{
    try
    {
        zephyr::lexer_t lexer(code_to_process);
        zephyr::parser_t parser(lexer);
        std::unique_ptr<zephyr::program_t> program_node = parser.parse();

        interpreter.interpret(*program_node); // Always interpret the program first

        // Check if this program contains class definitions - if so, keep it alive
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
        if (program_node->statements.size() == 1)
        {
            if (auto expr_stmt = dynamic_cast<zephyr::expression_statement_t*>(program_node->statements[0].get()))
            {
                std::shared_ptr<zephyr::object_t> result = interpreter.get_current_result();
                // Only print if there's a result and it's not a NoneObject
                if (result && result->get_type()->get_name() != "none")
                {
                    std::cout << result->to_string() << std::endl;
                }
            }
        }

        // Keep the program alive if it contains class definitions
        if (has_class_definition)
        {
            alive_programs.push_back(std::move(program_node));
        }

        return true;

    }
    catch (const zephyr::parsing_error_t& e)
    {
        // Check if the error is due to unexpected EOF, meaning more input is needed
        if (std::string(e.what()).find("Unexpected end of file") != std::string::npos)
        {
            return false; // Incomplete input - don't clear accumulated_input
        }
        print_error(e.what(), "SyntaxError", full_source_code, e.line, e.column);
        return true; // Parsing error occurred - clear accumulated_input
    }
    catch (const zephyr::runtime_error_with_location_t& e)
    {
        print_error(e.what(), e.error_name, full_source_code, e.line, e.column, "", e.length);
        return true; // Runtime error occurred - clear accumulated_input
    }
    catch (const std::runtime_error& e)
    {
        print_error(e.what(), "Error", full_source_code, 0, 0);
        return true; // Runtime error occurred - clear accumulated_input
    }
    return true; // Success
}

// Interactive REPL
auto run_repl() -> void
{
    std::string line;
    std::string accumulated_input;
    std::cout << "Zephyr REPL (Ctrl+D to exit)" << std::endl;
    zephyr::interpreter_t interpreter("<stdin>", accumulated_input); // Create one interpreter for the whole REPL session

    // Keep Program objects alive to prevent dangling pointers to class definitions and methods
    std::vector<std::unique_ptr<zephyr::program_t>> alive_programs;

    while (true)
    {
        std::cout << (accumulated_input.empty() ? ">> " : ".. ");
        if (!std::getline(std::cin, line))
        {
            break; // Exit on Ctrl+D or EOF
        }

        accumulated_input += line + "\n";

        if (process_code_repl(accumulated_input, interpreter, accumulated_input, alive_programs))
        {
            accumulated_input.clear(); // Clear on successful processing or error
        }
    }
}

// File-based execution
auto run_from_file(const std::string& filename) -> int
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

    zephyr::interpreter_t interpreter(filename, source_code); // Create one interpreter for the whole file
    bool success = process_code(source_code, interpreter, source_code, filename);
    return success ? 0 : 1;
}

auto main(int argc, char* argv[]) -> int
{
    if (argc > 2)
    {
        print_error("Usage: " + std::string(argv[0]) + " [file.gemini]", "Error", "", 0, 0);
        return 1;
    }

    if (argc == 1)
    {
        run_repl();
        return 0;
    }
    else
    {
        return run_from_file(argv[1]);
    }
}
