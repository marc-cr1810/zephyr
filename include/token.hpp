#pragma once

#include <string>
#include <vector>

namespace zephyr
{

enum class token_type_e
{
    lparen,
    rparen,
    lbrace,
    rbrace,
    lbracket,
    rbracket,
    question,
    nullish_coalescing,
    colon,
    comma,
    semicolon,
    plus,
    minus,
    mul,
    div,
    modulo,
    bitwise_and,
    bitwise_or,
    bitwise_xor,
    bitwise_not,
    left_shift,
    right_shift,
    assign,
    plus_assign,
    minus_assign,
    mul_assign,
    div_assign,
    modulo_assign,
    increment,
    decrement,
    dot,
    question_dot,
    arrow,
    pipe,
    eq,
    ne,
    lt,
    le,
    gt,
    ge,
    and_token,
    or_token,
    not_token,
    and_op,
    or_op,
    not_op,
    name,
    number,
    string,
    fstring,
    true_token,
    false_token,
    none,
    float_token,
    if_token,
    else_token,
    while_token,
    for_token,
    do_token,
    until,
    loop,
    where,
    in,
    switch_token,
    case_token,
    default_token,
    func,
    return_token,
    class_token,
    interface_token,
    this_token,
    try_token,
    catch_token,
    break_token,
    continue_token,
    async,
    await,
    spawn,
    const_token,

    // End of file
    end_of_file,

    // Unknown token type
    unknown
};

struct token_t
{
    token_type_e type;
    std::string text;
    int line;
    int column;
    size_t position; // Added back for lexer_t internal use
    int end_line; // Added for ast node creation
    int end_column; // Added for ast node creation
};

}
