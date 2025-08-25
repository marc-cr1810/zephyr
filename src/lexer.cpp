#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include "errors.hpp"
#include "error_context.hpp"
#include <iostream>

namespace zephyr
{

lexer_t::lexer_t(const std::string& source)
    : m_source(source), m_position(0), m_line(1), m_column(1)
{
}

auto lexer_t::get_next_token() -> token_t
{
    while (m_position < m_source.length() && isspace(m_source[m_position]))
    {
        if (m_source[m_position] == '\n')
        {
            m_line++;
            m_column = 1;
        }
        else
        {
            m_column++;
        }
        m_position++;
    }

    if (m_position >= m_source.length())
    {
        return { token_type_e::end_of_file, "", m_line, m_column, m_position, m_line, m_column };
    }

    size_t start_pos = m_position;
    int start_column = m_column;
    char current_char = m_source[m_position];

    auto make_token = [&](token_type_e type, const std::string& text) -> token_t
    {
        token_t token = { type, text, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(text.length()) - 1 };
        m_column += text.length();
        m_position += text.length();
        return token;
    };

    // Skip comments
    if (current_char == '#')
    {
        while (m_position < m_source.length() && m_source[m_position] != '\n')
        {
            m_position++;
            m_column++;
        }
        // After skipping the comment, call get_next_token() again to get the actual next token
        return get_next_token();
    }

    // String literals
    if (current_char == 'f' && m_position + 1 < m_source.length() && m_source[m_position + 1] == '"')
    {
        m_position++; // Consume the 'f'
        current_char = m_source[m_position];
        m_position++; // Consume the opening quote
        std::string str_value;
        while (m_position < m_source.length() && m_source[m_position] != '"')
        {
            if (m_source[m_position] == '\\')
            { // Handle escape sequences
                m_position++;
                if (m_position >= m_source.length())
                {
                    zephyr::get_current_error_location() = {m_line, m_column, 1};
                    throw zephyr::syntax_error_t("Unterminated string literal");
                }
                char escaped_char = m_source[m_position];
                switch (escaped_char)
                {
                    case 'n': str_value += '\n'; break;
                    case 't': str_value += '\t'; break;
                    case '\'': str_value += '\''; break;
                    case '"': str_value += '"'; break;
                    case '\\': str_value += '\\'; break;
                    default: zephyr::get_current_error_location() = {m_line, m_column, 1};
                             throw zephyr::syntax_error_t("Unknown escape sequence: \\" + std::string(1, escaped_char));
                }
            }
            else
            {
                str_value += m_source[m_position];
            }
            m_position++;
        }
        if (m_position >= m_source.length())
        {
            zephyr::get_current_error_location() = {m_line, m_column, 1};
            throw zephyr::syntax_error_t("Unterminated string literal");
        }
        m_position++; // Consume the closing quote
        return { token_type_e::fstring, str_value, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(str_value.length()) - 1 };
    }

    if (current_char == '\'' || current_char == '"')
    {
        char quote_type = current_char;
        m_position++; // Consume the opening quote
        std::string str_value;
        while (m_position < m_source.length() && m_source[m_position] != quote_type)
        {
            if (m_source[m_position] == '\\')
            { // Handle escape sequences
                m_position++;
                if (m_position >= m_source.length())
                {
                    zephyr::get_current_error_location() = {m_line, m_column, 1};
                    throw zephyr::syntax_error_t("Unterminated string literal");
                }
                char escaped_char = m_source[m_position];
                switch (escaped_char)
                {
                    case 'n': str_value += '\n'; break;
                    case 't': str_value += '\t'; break;
                    case '\'': str_value += '\''; break;
                    case '"': str_value += '"'; break;
                    case '\\': str_value += '\\'; break;
                    default: zephyr::get_current_error_location() = {m_line, m_column, 1};
                             throw zephyr::syntax_error_t("Unknown escape sequence: \\" + std::string(1, escaped_char));
                }
            }
            else
            {
                str_value += m_source[m_position];
            }
            m_position++;
        }
        if (m_position >= m_source.length())
        {
            zephyr::get_current_error_location() = {m_line, m_column, 1};
            throw zephyr::syntax_error_t("Unterminated string literal");
        }
        m_position++; // Consume the closing quote
        return { token_type_e::string, str_value, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(str_value.length()) - 1 };
    }

    // Numbers (including floats)
    if (isdigit(current_char))
    {
        std::string number_str;
        bool is_float = false;
        while (m_position < m_source.length() && (isdigit(m_source[m_position]) || m_source[m_position] == '.'))
        {
            if (m_source[m_position] == '.')
            {
                if (is_float)
                {
                    break; // Second dot, not part of this number
                }
                is_float = true;
            }
            number_str += m_source[m_position];
            m_position++;
        }
        m_column += number_str.length();
        return { is_float ? token_type_e::float_token : token_type_e::number, number_str, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(number_str.length()) - 1 };
    }

    // Identifiers and keywords
    if (isalpha(current_char) || current_char == '_')
    {
        std::string identifier;
        while (m_position < m_source.length() && (isalnum(m_source[m_position]) || m_source[m_position] == '_'))
        {
            identifier += m_source[m_position];
            m_position++;
        }
        m_column += identifier.length();

        // Check for keywords
        if (identifier == "if") return { token_type_e::if_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "else") return { token_type_e::else_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "while") return { token_type_e::while_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "for") return { token_type_e::for_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "do") return { token_type_e::do_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "until") return { token_type_e::until, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "loop") return { token_type_e::loop, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "where") return { token_type_e::where, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "in") return { token_type_e::in, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "switch") return { token_type_e::switch_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "case") return { token_type_e::case_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "default") return { token_type_e::default_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "func") return { token_type_e::func, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "return") return { token_type_e::return_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "class") return { token_type_e::class_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "interface") return { token_type_e::interface_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "this") return { token_type_e::this_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "try") return { token_type_e::try_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "catch") return { token_type_e::catch_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "break") return { token_type_e::break_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "continue") return { token_type_e::continue_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "async") return { token_type_e::async, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "await") return { token_type_e::await, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "spawn") return { token_type_e::spawn, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "true") return { token_type_e::true_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "false") return { token_type_e::false_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "none") return { token_type_e::none, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "and") return { token_type_e::and_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "or") return { token_type_e::or_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "not") return { token_type_e::not_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
        if (identifier == "const") return { token_type_e::const_token, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };

        return { token_type_e::name, identifier, m_line, start_column, start_pos, m_line, start_column + static_cast<int>(identifier.length()) - 1 };
    }

    // Two-character operators
    if (m_position + 1 < m_source.length())
    {
        std::string two_char = m_source.substr(m_position, 2);
        if (two_char == "==") return make_token(token_type_e::eq, "==");
        if (two_char == "!=") return make_token(token_type_e::ne, "!=");
        if (two_char == "<=") return make_token(token_type_e::le, "<=");
        if (two_char == ">=") return make_token(token_type_e::ge, ">=");
        if (two_char == "&&") return make_token(token_type_e::and_op, "&&");
        if (two_char == "||") return make_token(token_type_e::or_op, "||");
        if (two_char == "->") return make_token(token_type_e::arrow, "->");
        if (two_char == "??") return make_token(token_type_e::nullish_coalescing, "??");
        if (two_char == "|>") return make_token(token_type_e::pipe, "|>");
        if (two_char == "?.") return make_token(token_type_e::question_dot, "?.");
        if (two_char == "+=") return make_token(token_type_e::plus_assign, "+=");
        if (two_char == "-=") return make_token(token_type_e::minus_assign, "-=");
        if (two_char == "*=") return make_token(token_type_e::mul_assign, "*=");
        if (two_char == "/=") return make_token(token_type_e::div_assign, "/=");
        if (two_char == "%=") return make_token(token_type_e::modulo_assign, "%=");
        if (two_char == "++") return make_token(token_type_e::increment, "++");
        if (two_char == "--") return make_token(token_type_e::decrement, "--");
    }

    // Single-character operators
    switch (current_char)
    {
        case '(': return make_token(token_type_e::lparen, "(");
        case ')': return make_token(token_type_e::rparen, ")");
        case '{': return make_token(token_type_e::lbrace, "{");
        case '}': return make_token(token_type_e::rbrace, "}");
        case '[': return make_token(token_type_e::lbracket, "[");
        case ']': return make_token(token_type_e::rbracket, "]");
        case ',': return make_token(token_type_e::comma, ",");
        case ';': return make_token(token_type_e::semicolon, ";");
        case '+': return make_token(token_type_e::plus, "+");
        case '-': return make_token(token_type_e::minus, "-");
        case '*': return make_token(token_type_e::mul, "*");
        case '/': return make_token(token_type_e::div, "/");
        case '%': return make_token(token_type_e::modulo, "%");
        case '=': return make_token(token_type_e::assign, "=");
        case '<': return make_token(token_type_e::lt, "<");
        case '>': return make_token(token_type_e::gt, ">");
        case '!': return make_token(token_type_e::not_op, "!");
        case '.': return make_token(token_type_e::dot, ".");
        case ':': return make_token(token_type_e::colon, ":");
        case '?': return make_token(token_type_e::question, "?");
        default:
            zephyr::get_current_error_location() = {m_line, m_column, 1};
            throw zephyr::syntax_error_t("Unexpected character: " + std::string(1, current_char));
    }
}

auto lexer_t::peek_next_token() -> token_t
{
    // Save current state
    size_t saved_position = m_position;
    int saved_line = m_line;
    int saved_column = m_column;

    // Get next token
    token_t next_token = get_next_token();

    // Restore state
    m_position = saved_position;
    m_line = saved_line;
    m_column = saved_column;

    return next_token;
}

auto lexer_t::peek_after(token_t since) -> token_t
{
    // Save current state
    size_t saved_position = m_position;
    int saved_line = m_line;
    int saved_column = m_column;

    // Skip to after the 'since' token
    token_t current_token;
    do
    {
        current_token = get_next_token();
    } while (current_token.type != since.type || current_token.text != since.text);

    // Get the token after 'since'
    token_t next_token = get_next_token();

    // Restore state
    m_position = saved_position;
    m_line = saved_line;
    m_column = saved_column;

    return next_token;
}

}
