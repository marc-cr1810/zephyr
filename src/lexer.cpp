#include "lexer.hpp"
#include "error_context.hpp"
#include "errors.hpp"
#include <cctype>
#include <unordered_map>

namespace zephyr
{

namespace
{
    const std::unordered_map<std::string_view, token_type_e> keywords = {
        {"if", token_type_e::if_token},
        {"else", token_type_e::else_token},
        {"while", token_type_e::while_token},
        {"for", token_type_e::for_token},
        {"do", token_type_e::do_token},
        {"until", token_type_e::until},
        {"loop", token_type_e::loop},
        {"where", token_type_e::where},
        {"in", token_type_e::in},
        {"switch", token_type_e::switch_token},
        {"case", token_type_e::case_token},
        {"default", token_type_e::default_token},
        {"func", token_type_e::func},
        {"return", token_type_e::return_token},
        {"class", token_type_e::class_token},
        {"interface", token_type_e::interface_token},
        {"this", token_type_e::this_token},
        {"try", token_type_e::try_token},
        {"catch", token_type_e::catch_token},
        {"break", token_type_e::break_token},
        {"continue", token_type_e::continue_token},
        {"async", token_type_e::async},
        {"await", token_type_e::await},
        {"spawn", token_type_e::spawn},
        {"true", token_type_e::true_token},
        {"false", token_type_e::false_token},
        {"none", token_type_e::none},
        {"and", token_type_e::and_token},
        {"or", token_type_e::or_token},
        {"not", token_type_e::not_token},
        {"const", token_type_e::const_token},
        {"is", token_type_e::is_token},
    };
} // namespace

lexer_t::lexer_t(const std::string & source)
    : m_source(source), m_position(0), m_line(1), m_column(1)
{
}

auto lexer_t::next_token() -> token_t
{
    while (m_position < m_source.length())
    {
        if (isspace(m_source[m_position]))
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
            continue;
        }

        if (m_source[m_position] == '#')
        {
            while (m_position < m_source.length() && m_source[m_position] != '\n')
            {
                m_position++;
                m_column++;
            }
            continue;
        }
        break;
    }

    if (m_position >= m_source.length())
    {
        return { token_type_e::end_of_file, "", m_line, m_column, m_position, m_line, m_column };
    }

    size_t start_pos = m_position;
    int start_column = m_column;
    char current_char = m_source[m_position];

    auto make_token = [&](token_type_e type, std::string_view text) -> token_t
    {
        token_t token = {
            type,
            text,
            m_line,
            start_column,
            start_pos,
            m_line,
            start_column + static_cast<int>(text.length()) - 1
        };
        m_position += text.length();
        m_column += text.length();
        return token;
    };

    auto peek = [&](int offset = 1) -> char {
        if (m_position + offset < m_source.length()) {
            return m_source[m_position + offset];
        }
        return '\0';
    };

    // String literals (raw view, no un-escaping)
    if (current_char == 'f' && (peek() == '"' || peek() == '\''))
    {
        char quote_type = peek();
        size_t literal_start_pos = m_position;
        m_position += 2; // Skip f and the opening quote
        size_t content_start_pos = m_position;

        while (m_position < m_source.length() && m_source[m_position] != quote_type)
        {
            if (m_source[m_position] == '\\' && m_position + 1 < m_source.length()) {
                m_position++;
            }
            m_position++;
        }
        size_t content_length = m_position - content_start_pos;

        if (m_position < m_source.length()) {
            m_position++; // Skip closing quote
        }
        size_t full_literal_length = m_position - literal_start_pos;

        token_t token = {
            token_type_e::fstring,
            std::string_view(m_source.data() + content_start_pos, content_length),
            m_line,
            start_column,
            literal_start_pos,
            m_line,
            start_column + static_cast<int>(full_literal_length) - 1
        };
        m_column += full_literal_length;
        return token;
    }

    if (current_char == '\'' || current_char == '"')
    {
        size_t literal_start_pos = m_position;
        char quote_type = current_char;
        m_position++; // Skip opening quote
        size_t content_start_pos = m_position;

        while (m_position < m_source.length() && m_source[m_position] != quote_type)
        {
            if (m_source[m_position] == '\\' && m_position + 1 < m_source.length()) {
                m_position++;
            }
            m_position++;
        }
        size_t content_length = m_position - content_start_pos;

        if (m_position < m_source.length()) {
            m_position++; // Skip closing quote
        }
        size_t full_literal_length = m_position - literal_start_pos;

        token_t token = {
            token_type_e::string,
            std::string_view(m_source.data() + content_start_pos, content_length),
            m_line,
            start_column,
            literal_start_pos,
            m_line,
            start_column + static_cast<int>(full_literal_length) - 1
        };
        m_column += full_literal_length;
        return token;
    }

    // Numbers (including floats)
    if (isdigit(current_char))
    {
        size_t num_start = m_position;
        bool is_float = false;
        while (m_position < m_source.length() && (isdigit(m_source[m_position]) || m_source[m_position] == '.'))
        {
            if (m_source[m_position] == '.')
            {
                if (is_float)
                {
                    break;
                }
                is_float = true;
            }
            m_position++;
        }
        size_t length = m_position - num_start;
        m_position = num_start; // reset for make_token
        return make_token(is_float ? token_type_e::float_token : token_type_e::number, std::string_view(m_source.data() + num_start, length));
    }

    // Identifiers and keywords
    if (isalpha(current_char) || current_char == '_')
    {
        size_t ident_start = m_position;
        while (m_position < m_source.length() && (isalnum(m_source[m_position]) || m_source[m_position] == '_'))
        {
            m_position++;
        }
        size_t length = m_position - ident_start;
        std::string_view identifier(m_source.data() + ident_start, length);
        m_position = ident_start; // reset for make_token

        if (auto it = keywords.find(identifier); it != keywords.end())
        {
            return make_token(it->second, identifier);
        }

        return make_token(token_type_e::name, identifier);
    }

    // Operators and punctuation
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
        case ':': return make_token(token_type_e::colon, ":");
        case '~': return make_token(token_type_e::bitwise_not, "~");
        case '^': return make_token(token_type_e::bitwise_xor, "^");
        case '.': return make_token(token_type_e::dot, ".");

        case '+':
            if (peek() == '=') { return make_token(token_type_e::plus_assign, "+="); }
            if (peek() == '+') { return make_token(token_type_e::increment, "++"); }
            return make_token(token_type_e::plus, "+");
        case '-':
            if (peek() == '=') { return make_token(token_type_e::minus_assign, "-="); }
            if (peek() == '-') { return make_token(token_type_e::decrement, "--"); }
            if (peek() == '>') { return make_token(token_type_e::arrow, "->"); }
            return make_token(token_type_e::minus, "-");
        case '*':
            if (peek() == '*')
            {
                if (peek(2) == '=') { return make_token(token_type_e::power_assign, "**="); }
                return make_token(token_type_e::power, "**");
            }
            if (peek() == '=') { return make_token(token_type_e::mul_assign, "*="); }
            return make_token(token_type_e::mul, "*");
        case '/':
            if (peek() == '=') { return make_token(token_type_e::div_assign, "/="); }
            return make_token(token_type_e::div, "/");
        case '%':
            if (peek() == '=') { return make_token(token_type_e::modulo_assign, "%="); }
            return make_token(token_type_e::modulo, "%");
        case '=':
            if (peek() == '=') { return make_token(token_type_e::eq, "=="); }
            return make_token(token_type_e::assign, "=");
        case '!':
            if (peek() == '=') { return make_token(token_type_e::ne, "!="); }
            return make_token(token_type_e::not_op, "!");
        case '<':
            if (peek() == '<') { return make_token(token_type_e::left_shift, "<<"); }
            if (peek() == '=') { return make_token(token_type_e::le, "<="); }
            return make_token(token_type_e::lt, "<");
        case '>':
            if (peek() == '>') { return make_token(token_type_e::right_shift, ">>"); }
            if (peek() == '=') { return make_token(token_type_e::ge, ">="); }
            return make_token(token_type_e::gt, ">");
        case '&':
            if (peek() == '&') { return make_token(token_type_e::and_op, "&&"); }
            return make_token(token_type_e::bitwise_and, "&");
        case '|':
            if (peek() == '|') { return make_token(token_type_e::or_op, "||"); }
            if (peek() == '>') { return make_token(token_type_e::pipe, "|>"); }
            return make_token(token_type_e::bitwise_or, "|");
        case '?':
            if (peek() == '?') { return make_token(token_type_e::nullish_coalescing, "??"); }
            if (peek() == '.') { return make_token(token_type_e::question_dot, "?."); }
            return make_token(token_type_e::question, "?");

        default:
            zephyr::current_error_location() = {m_line, m_column, 1};
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
    auto next = next_token();

    // Restore state
    m_position = saved_position;
    m_line = saved_line;
    m_column = saved_column;

    return next;
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
        current_token = next_token();
    } while (current_token.type != since.type || current_token.text != since.text);

    // Get the token after 'since'
    auto next = next_token();

    // Restore state
    m_position = saved_position;
    m_line = saved_line;
    m_column = saved_column;

    return next;
}

}
