#pragma once

#include "token.hpp"
#include <string>
#include <vector>

namespace zephyr
{

class lexer_t
{
public:
    lexer_t(const std::string& source);

    auto get_next_token() -> token_t;
    auto peek_next_token() -> token_t;
    auto peek_after(token_t since) -> token_t;

private:
    std::string m_source;
    size_t m_position;
    int m_line;
    int m_column;
};

}
