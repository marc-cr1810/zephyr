#include "zephyr/lexer.hpp"
#include "zephyr/error_context.hpp"
#include "zephyr/errors.hpp"
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
        {"enum", token_type_e::enum_token},
        {"interface", token_type_e::interface_token},
        {"this", token_type_e::this_token},
        {"try", token_type_e::try_token},
        {"catch", token_type_e::catch_token},
        {"finally", token_type_e::finally_token},
        {"throw", token_type_e::throw_token},
        {"with", token_type_e::with_token},
        {"as", token_type_e::as_token},
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
        {"final", token_type_e::final_token},
        {"abstract", token_type_e::abstract_token},
        {"super", token_type_e::super_token},
        {"import", token_type_e::import_token},
        {"from", token_type_e::from_token},
        {"internal", token_type_e::internal_token},
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

    // Raw string literals (r"..." or r'...')
    if (current_char == 'r' && (peek() == '"' || peek() == '\''))
    {
        char quote_type = peek();
        size_t literal_start_pos = m_position;
        m_position += 2; // Skip r and the opening quote
        size_t content_start_pos = m_position;

        // For raw strings, don't process escape sequences at all
        while (m_position < m_source.length())
        {
            if (m_source[m_position] == quote_type)
            {
                // Check if this quote is escaped with a backslash
                // In raw strings, backslash doesn't escape, so we just check for the quote
                break;
            }

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
        size_t content_length = m_position - content_start_pos;

        if (m_position < m_source.length()) {
            m_position++; // Skip closing quote
            m_column++;
        }
        size_t full_literal_length = m_position - literal_start_pos;

        token_t token = {
            token_type_e::raw_string,
            std::string_view(m_source.data() + content_start_pos, content_length),
            m_line,
            start_column,
            literal_start_pos,
            m_line,
            m_column - 1
        };
        return token;
    }

    // F-string literals (f"..." or f'...')
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

    // Multi-line string literals (""" ... """ or ''' ... ''')
    if ((current_char == '"' && peek() == '"' && peek(2) == '"') ||
        (current_char == '\'' && peek() == '\'' && peek(2) == '\''))
    {
        char quote_type = current_char;
        size_t literal_start_pos = m_position;
        int start_line = m_line;
        m_position += 3; // Skip opening triple quotes
        m_column += 3;
        size_t content_start_pos = m_position;

        // Read until we find closing triple quotes
        while (m_position + 2 < m_source.length())
        {
            if (m_source[m_position] == quote_type &&
                m_source[m_position + 1] == quote_type &&
                m_source[m_position + 2] == quote_type)
            {
                break;
            }

            if (m_source[m_position] == '\n')
            {
                m_line++;
                m_column = 1;
            }
            else
            {
                m_column++;
            }

            // Handle escape sequences in multi-line strings
            if (m_source[m_position] == '\\' && m_position + 1 < m_source.length())
            {
                m_position++;
                if (m_source[m_position] == '\n')
                {
                    m_line++;
                    m_column = 1;
                }
                else
                {
                    m_column++;
                }
            }
            m_position++;
        }

        size_t content_length = m_position - content_start_pos;

        // Skip closing triple quotes if found
        if (m_position + 2 < m_source.length() &&
            m_source[m_position] == quote_type &&
            m_source[m_position + 1] == quote_type &&
            m_source[m_position + 2] == quote_type)
        {
            m_position += 3;
            m_column += 3;
        }

        token_t token = {
            token_type_e::multiline_string,
            std::string_view(m_source.data() + content_start_pos, content_length),
            start_line,
            start_column,
            literal_start_pos,
            m_line,
            m_column - 1
        };
        return token;
    }

    // Regular string literals (single or double quotes)
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

    // Numbers (including floats, hex, binary, octal)
    if (isdigit(current_char))
    {
        size_t num_start = m_position;

        // Check for special number formats (0x, 0b, 0o)
        if (current_char == '0' && m_position + 1 < m_source.length())
        {
            char second_char = m_source[m_position + 1];

            // Hexadecimal: 0x or 0X
            if (second_char == 'x' || second_char == 'X')
            {
                m_position += 2; // Skip '0x'
                while (m_position < m_source.length() &&
                       (isdigit(m_source[m_position]) ||
                        (m_source[m_position] >= 'a' && m_source[m_position] <= 'f') ||
                        (m_source[m_position] >= 'A' && m_source[m_position] <= 'F')))
                {
                    m_position++;
                }
                size_t length = m_position - num_start;
                m_position = num_start; // reset for make_token
                return make_token(token_type_e::hex_number, std::string_view(m_source.data() + num_start, length));
            }

            // Binary: 0b or 0B
            if (second_char == 'b' || second_char == 'B')
            {
                m_position += 2; // Skip '0b'
                while (m_position < m_source.length() &&
                       (m_source[m_position] == '0' || m_source[m_position] == '1'))
                {
                    m_position++;
                }
                size_t length = m_position - num_start;
                m_position = num_start; // reset for make_token
                return make_token(token_type_e::binary_number, std::string_view(m_source.data() + num_start, length));
            }

            // Octal: 0o or 0O
            if (second_char == 'o' || second_char == 'O')
            {
                m_position += 2; // Skip '0o'
                while (m_position < m_source.length() &&
                       (m_source[m_position] >= '0' && m_source[m_position] <= '7'))
                {
                    m_position++;
                }
                size_t length = m_position - num_start;
                m_position = num_start; // reset for make_token
                return make_token(token_type_e::octal_number, std::string_view(m_source.data() + num_start, length));
            }
        }

        // Regular decimal numbers (integers and floats)
        bool is_float = false;
        while (m_position < m_source.length() && (isdigit(m_source[m_position]) || m_source[m_position] == '.'))
        {
            if (m_source[m_position] == '.')
            {
                if (is_float)
                {
                    break; // Second dot, stop parsing
                }
                is_float = true;
            }
            m_position++;
        }

        // Check for integer size suffix (only for non-float numbers)
        if (!is_float && m_position < m_source.length() && isalpha(m_source[m_position])) {
            size_t suffix_start = m_position;

            // Parse potential suffix (u8, i32, u64, etc.)
            while (m_position < m_source.length() &&
                   (isalnum(m_source[m_position]))) {
                m_position++;
            }

            std::string suffix = std::string(m_source.substr(suffix_start, m_position - suffix_start));

            // Check if it's a valid integer suffix
            try {
                // Just validate the suffix without throwing - we'll parse it later
                if (suffix == "i8" || suffix == "i16" || suffix == "i32" || suffix == "i64" ||
                    suffix == "u8" || suffix == "u16" || suffix == "u32" || suffix == "u64") {
                    // Valid suffix found - return as sized integer literal
                    size_t total_length = m_position - num_start;
                    m_position = num_start; // reset for make_token
                    return make_token(token_type_e::sized_int_literal,
                                    std::string_view(m_source.data() + num_start, total_length));
                }
            } catch (...) {
                // Not a valid integer suffix - backtrack and treat as separate tokens
            }

            // Not a valid suffix - backtrack
            m_position = suffix_start;
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
