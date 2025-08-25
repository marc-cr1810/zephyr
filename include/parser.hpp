#pragma once

#include "lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <stdexcept>

namespace zephyr
{



class parser_t
{
public:
    parser_t(lexer_t& lexer);

    auto parse() -> std::unique_ptr<program_t>;

private:
    auto eat(token_type_e type) -> void;
    auto program() -> std::unique_ptr<program_t>;
    auto statement() -> std::unique_ptr<statement_t>;
    auto expression() -> std::unique_ptr<expression_t>;
    auto ternary_expression() -> std::unique_ptr<expression_t>;
    auto pipe_expression() -> std::unique_ptr<expression_t>;
    auto logical_or_expression() -> std::unique_ptr<expression_t>;
    auto nullish_coalescing_expression() -> std::unique_ptr<expression_t>;
    auto logical_and_expression() -> std::unique_ptr<expression_t>;
    auto bitwise_or_expression() -> std::unique_ptr<expression_t>;
    auto bitwise_xor_expression() -> std::unique_ptr<expression_t>;
    auto bitwise_and_expression() -> std::unique_ptr<expression_t>;
    auto bitwise_shift_expression() -> std::unique_ptr<expression_t>;
    auto unary() -> std::unique_ptr<expression_t>;
    auto comparison() -> std::unique_ptr<expression_t>;
    auto additive_expression() -> std::unique_ptr<expression_t>;
    auto term() -> std::unique_ptr<expression_t>;
    auto unary_factor() -> std::unique_ptr<expression_t>;
    auto factor() -> std::unique_ptr<expression_t>;
    auto block() -> std::unique_ptr<block_t>;
    auto if_statement() -> std::unique_ptr<if_statement_t>;
    auto while_statement() -> std::unique_ptr<while_statement_t>;
    auto do_while_statement() -> std::unique_ptr<do_while_statement_t>;
    auto do_until_statement() -> std::unique_ptr<do_until_statement_t>;
    auto for_statement() -> std::unique_ptr<for_statement_t>;
    auto for_each_statement() -> std::unique_ptr<for_each_statement_t>;
    auto loop_statement() -> std::unique_ptr<loop_statement_t>;

    auto list_literal() -> std::unique_ptr<list_literal_t>;
    auto dictionary_literal() -> std::unique_ptr<dictionary_literal_t>;
    auto index_access(std::unique_ptr<expression_t> list_expression) -> std::unique_ptr<index_access_t>;
    auto function_definition() -> std::unique_ptr<function_definition_t>;
    auto return_statement() -> std::unique_ptr<return_statement_t>;
    auto assignment_statement() -> std::unique_ptr<assignment_t>;
    auto list_destructuring_assignment() -> std::unique_ptr<list_destructuring_assignment_t>;
    auto const_declaration() -> std::unique_ptr<const_declaration_t>;
    auto typed_declaration() -> std::unique_ptr<typed_declaration_t>;
    auto typed_const_declaration() -> std::unique_ptr<typed_const_declaration_t>;
    auto empty_declaration() -> std::unique_ptr<empty_declaration_t>;
    auto empty_typed_declaration() -> std::unique_ptr<empty_typed_declaration_t>;
    auto member_variable_declaration() -> std::unique_ptr<member_variable_declaration_t>;
    auto switch_statement() -> std::unique_ptr<switch_statement_t>;
    auto case_statement() -> std::unique_ptr<case_statement_t>;
    auto break_statement() -> std::unique_ptr<break_statement_t>;
    auto continue_statement() -> std::unique_ptr<continue_statement_t>;
    auto try_catch_statement() -> std::unique_ptr<try_catch_statement_t>;
    auto function_call(std::string function_name, token_t name_token) -> std::unique_ptr<function_call_t>;
    auto lambda_expression() -> std::unique_ptr<lambda_expression_t>;
    auto class_definition() -> std::unique_ptr<class_definition_t>;
    auto interface_definition() -> std::unique_ptr<interface_definition_t>;
    auto function_signature() -> function_signature_t;
    auto method_call(std::unique_ptr<expression_t> object, std::string method_name, token_t method_token) -> std::unique_ptr<method_call_t>;
    auto member_access(std::unique_ptr<expression_t> object, std::string member_name, token_t member_token) -> std::unique_ptr<member_access_t>;
    auto member_assignment() -> std::unique_ptr<member_assignment_t>;
    auto is_assignment_lookahead() -> bool;
    auto is_list_destructuring_assignment_lookahead() -> bool;
    auto is_typed_declaration_lookahead() -> bool;
    auto is_empty_declaration_lookahead() -> bool;
    auto is_member_variable_declaration_lookahead() -> bool;
    auto lambda_function() -> std::unique_ptr<lambda_expression_t>;
    auto parse_parameter_list() -> std::vector<parameter_t>;

private:
    struct for_each_head_struct_t
    {
        std::vector<for_each_variable_t> variables;
    };

    auto for_each_head() -> std::unique_ptr<parser_t::for_each_head_struct_t> ;

private:
    lexer_t& m_lexer;
    token_t m_current_token;
};

}
