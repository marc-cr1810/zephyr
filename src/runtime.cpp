#include "runtime.hpp"
#include "module_loader.hpp"
#include "lexer.hpp"
#include "objects/object.hpp"
#include "parser.hpp"
#include "errors.hpp"
#include "runtime_error.hpp"
#include "types/type.hpp"

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

namespace zephyr
{

// ANSI escape codes for colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static auto print_result(const std::shared_ptr<object_t>& obj) -> void
{
    if (obj == nullptr or obj->type()->name() == "none")
    {
        return;
    }

    auto output = obj->to_string();
    if (obj->type()->name() == "string")
    {
        char quote_char = '\'';
        if ((output.find_first_of('\'') != std::string::npos) and not (output.find_first_of('"') != std::string::npos))
        {
            quote_char = '"';
        }
        output = quote_char + output + quote_char;
    }
    std::cout << output << std::endl;
}

runtime_t::runtime_t()
    : m_scheduler(async_scheduler_t::instance())
{
    initialize_module_system();
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

    interpreter_t interpreter(filename, source_code);
    
    // Set up module system for main script
    auto main_module = create_main_module(filename, source_code);
    interpreter.set_module_loader(m_module_loader);
    interpreter.set_current_module(main_module);
    
    bool success = process_code(interpreter, source_code, filename);
    return success ? 0 : 1;
}

auto runtime_t::execute_string(const std::string& source, const std::string& context_name) -> int
{
    interpreter_t interpreter(context_name, source);
    
    // Set up module system for string execution
    auto main_module = create_main_module(context_name, source);
    interpreter.set_module_loader(m_module_loader);
    interpreter.set_current_module(main_module);
    
    bool success = process_code(interpreter, source, context_name);
    return success ? 0 : 1;
}

auto runtime_t::start_repl() -> void
{
    std::string line;
    std::string accumulated_input;
    std::cout << "Zephyr " << ZEPHYR_VERSION << " [GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << "] on " << ZEPHYR_SYSTEM_NAME << std::endl;

    interpreter_t interpreter("<stdin>", accumulated_input);
    
    // Set up module system for REPL
    auto repl_module = create_main_module("<stdin>", "");
    interpreter.set_module_loader(m_module_loader);
    interpreter.set_current_module(repl_module);

    while (true)
    {
        std::cout << ANSI_COLOR_MAGENTA << (accumulated_input.empty() ? ">> " : ".. ") << ANSI_COLOR_RESET;
        if (!std::getline(std::cin, line))
        {
            break; // Exit on Ctrl+D or EOF
        }

        accumulated_input += line + "\n";
        
        // Update module source code for current input
        repl_module->set_source_code(accumulated_input);
        // Re-inject module name variable for current session
        interpreter.update_module_name_variable();

        if (process_code_repl(interpreter, accumulated_input, accumulated_input))
        {
            accumulated_input.clear(); // Clear on successful processing or error
            // Reset module source when input is cleared
            repl_module->set_source_code("");
        }
    }
}

auto runtime_t::process_code(interpreter_t& interpreter, const std::string& source, const std::string& filename) -> bool
{
    try
    {
        lexer_t lexer(source);
        parser_t parser(lexer);
        std::unique_ptr<program_t> program_node = parser.parse();

        interpreter.interpret(*program_node);

        // If the last statement was an expression, print its result
        if (!program_node->statements.empty())
        {
            if (auto expr_stmt = dynamic_cast<expression_statement_t*>(program_node->statements.back().get()))
            {
                auto result = interpreter.current_result();
                print_result(result);
            }
        }

        return true;
    }
    catch (const runtime_error_with_location_t& e)
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

auto runtime_t::process_code_repl(interpreter_t& interpreter, const std::string& source, const std::string& full_source_code) -> bool
{
    try
    {
        lexer_t lexer(source);
        parser_t parser(lexer);
        std::unique_ptr<program_t> program_node = parser.parse();

        interpreter.interpret(*program_node);

        bool has_class_definition = false;
        for (const auto& stmt : program_node->statements)
        {
            if (dynamic_cast<class_definition_t*>(stmt.get()))
            {
                has_class_definition = true;
                break;
            }
        }

        // If the last statement was an expression, print its result in REPL mode
        if (!program_node->statements.empty())
        {
            if (auto expr_stmt = dynamic_cast<expression_statement_t*>(program_node->statements.back().get()))
            {
                auto result = interpreter.current_result();
                print_result(result);
            }
        }

        if (has_class_definition)
        {
            m_alive_programs.push_back(std::move(program_node));
        }

        return true;
    }
    catch (const syntax_error_t& e)
    {
        if (std::string(e.what()).find("Unexpected end of file") != std::string::npos)
        {
            return false; // Incomplete input
        }
        print_error(e.what(), e.error_name(), full_source_code, e.line(), e.column());
        return true; // Error occurred, clear input
    }
    catch (const runtime_error_with_location_t& e)
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
        std::cerr << "  File " << ANSI_COLOR_MAGENTA << "\"" << filename << "\"" << ANSI_COLOR_RESET << ", line " << ANSI_COLOR_MAGENTA << line << ANSI_COLOR_RESET << std::endl;
    }
    else if (filename.empty() && line > 0)
    {
        std::cerr << "  File " << ANSI_COLOR_MAGENTA << "\"<stdin>\"" << ANSI_COLOR_RESET << ", line " << ANSI_COLOR_MAGENTA << line << ANSI_COLOR_RESET << std::endl;
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

auto runtime_t::initialize_module_system() -> void
{
    m_module_loader = std::make_shared<module_loader_t>();
}

auto runtime_t::create_main_module(const std::string& file_path, const std::string& source_code) -> std::shared_ptr<module_t>
{
    auto main_module = std::make_shared<module_t>("__main__", file_path);
    main_module->set_source_code(source_code);

    // Parse the main module for potential exports
    try {
        lexer_t lexer(source_code);
        parser_t parser(lexer);
        auto ast = parser.parse();
        main_module->set_ast(std::move(ast));
    } catch (const std::exception&) {
        // If parsing fails, we'll handle it during execution
        // This is just for setting up the module structure
    }

    return main_module;
}

} // namespace zephyr
