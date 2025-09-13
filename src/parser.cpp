#include "parser.hpp"
#include "errors.hpp"
#include "error_context.hpp"
#include <string>
#include <memory>
#include <cctype>
#include "ast.hpp"

namespace zephyr
{

// Helper function to convert token_type_e to string for error messages
std::string token_type_to_string(token_type_e type) {
    switch (type) {
        case token_type_e::lparen: return "lparen (";
        case token_type_e::rparen: return "rparen )";
        case token_type_e::lbrace: return "lbrace {";
        case token_type_e::rbrace: return "rbrace }";
        case token_type_e::lbracket: return "lbracket [";
        case token_type_e::rbracket: return "rbracket ]";
        case token_type_e::colon: return "colon :";
        case token_type_e::comma: return "comma ,";
        case token_type_e::semicolon: return "semicolon ;";
        case token_type_e::plus: return "plus +";
        case token_type_e::minus: return "minus -";
        case token_type_e::mul: return "mul *";
        case token_type_e::div: return "div /";
        case token_type_e::assign: return "assign =";
        case token_type_e::eq: return "eq ==";
        case token_type_e::ne: return "ne !=";
        case token_type_e::lt: return "lt <";
        case token_type_e::le: return "le <=";
        case token_type_e::gt: return "gt >";
        case token_type_e::ge: return "ge >=";
        case token_type_e::and_token: return "and";
        case token_type_e::or_token: return "or";
        case token_type_e::not_token: return "not";
        case token_type_e::and_op: return "and_op &&";
        case token_type_e::or_op: return "or_op ||";
        case token_type_e::not_op: return "not_op !";
        case token_type_e::name: return "name";
        case token_type_e::class_token: return "class";
        case token_type_e::interface_token: return "interface";
        case token_type_e::number: return "number";
        case token_type_e::string: return "string";
        case token_type_e::true_token: return "true";
        case token_type_e::false_token: return "false";
        case token_type_e::none: return "none";
        case token_type_e::float_token: return "float";
        case token_type_e::if_token: return "if";
        case token_type_e::else_token: return "else";
        case token_type_e::while_token: return "while";
        case token_type_e::for_token: return "for";
        case token_type_e::do_token: return "do";
        case token_type_e::until: return "until";
        case token_type_e::loop: return "loop";
        case token_type_e::where: return "where";
        case token_type_e::in: return "in";
        case token_type_e::try_token: return "try";
        case token_type_e::catch_token: return "catch";
        case token_type_e::end_of_file: return "end_of_file";
        case token_type_e::unknown: return "unknown";
        default: return ""; // Should not happen
    }
}

parser_t::parser_t(lexer_t& lexer)
    : m_lexer(lexer), m_current_token(lexer.next_token())
{}

void parser_t::eat(token_type_e type) {
    if (m_current_token.type == type) {
        m_current_token = m_lexer.next_token();
    } else {
        if (m_current_token.type == token_type_e::end_of_file) {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t("Unexpected end of file. Expected " + token_type_to_string(type) + ".");
        }
        std::string error_msg = "Expected " + token_type_to_string(type) + ", but got " + token_type_to_string(m_current_token.type) + " ('" + std::string(m_current_token.text) + "').";
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t(error_msg);
    }
}

std::unique_ptr<program_t> parser_t::parse() {
    std::unique_ptr<program_t> program_node = program();
    if (m_current_token.type != token_type_e::end_of_file) {
        std::string error_msg = "Extra tokens at end of file. Unexpected token: " +
                                token_type_to_string(m_current_token.type) +
                                " ('" + std::string(m_current_token.text) + "').";
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t(error_msg);
    }

    return program_node;
}

std::unique_ptr<program_t> parser_t::program() {
    int start_line = 1;
    int start_column = 1;
    if (m_current_token.type != token_type_e::end_of_file) {
        start_line = m_current_token.line;
        start_column = m_current_token.column;
    }

    auto program_node = std::make_unique<program_t>(start_line, start_column, start_line, start_column);

    std::unique_ptr<statement_t> last_statement = nullptr;
    while(m_current_token.type != token_type_e::end_of_file) {
        auto stmt = statement();
        program_node->add_statement(std::move(stmt));
    }

    if (!program_node->statements.empty()) {
        program_node->end_line = program_node->statements.back()->end_line;
        program_node->end_column = program_node->statements.back()->end_column;
    }
    return program_node;
}

std::unique_ptr<const_declaration_t> parser_t::const_declaration() {
    token_t const_token = m_current_token;
    eat(token_type_e::const_token);
    token_t name_token = m_current_token;
    eat(token_type_e::name);
    eat(token_type_e::assign);
    auto value_expr = expression();
    return std::make_unique<const_declaration_t>(name_token.text, std::move(value_expr), false, const_token.line, const_token.column, value_expr->end_line, value_expr->end_column);
}

std::unique_ptr<statement_t> parser_t::statement() {
    // Check for import statements first
    if (m_current_token.type == token_type_e::import_token) {
        return import_statement();
    }

    // Check for internal declarations
    if (m_current_token.type == token_type_e::internal_token) {
        return internal_declaration();
    }

    // Add this check at the very beginning
    if (m_current_token.type == token_type_e::const_token) {
        // Check if this is a typed const declaration: const x : type = value
        if (is_typed_declaration_lookahead()) {
            return typed_const_declaration();
        } else {
            return const_declaration();
        }
    }

    // Check for typed declaration: x : type = value
    if (is_typed_declaration_lookahead()) {
        return typed_declaration();
    }

    // Check for empty declarations: x : type (no assignment)
    if (is_empty_declaration_lookahead()) {
        return empty_typed_declaration();
    }

    // 1. Check for list destructuring assignment first using robust lookahead
    if (is_list_destructuring_assignment_lookahead()) {
        return list_destructuring_assignment();
    }

    // 2. Handle other specific statement types
    if (m_current_token.type == token_type_e::func) {
        return function_definition();
    } else if (m_current_token.type == token_type_e::async) {
        return function_definition();
    } else if (m_current_token.type == token_type_e::final_token || 
               m_current_token.type == token_type_e::abstract_token ||
               m_current_token.type == token_type_e::class_token) {
        return class_definition();
    } else if (m_current_token.type == token_type_e::interface_token) {
        return interface_definition();
    } else if (m_current_token.type == token_type_e::return_token) {
        return return_statement();
    } else if (m_current_token.type == token_type_e::if_token) {
        return if_statement();
    } else if (m_current_token.type == token_type_e::while_token) {
        return while_statement();
    } else if (m_current_token.type == token_type_e::loop) {
        return loop_statement();
    } else if (m_current_token.type == token_type_e::for_token) {
        // Look ahead to distinguish between for and for-each
        token_t next_token = m_lexer.peek_next_token();
        if (next_token.type == token_type_e::lparen) {
            token_t after_lparen = m_lexer.peek_after(next_token);
            if (after_lparen.type == token_type_e::name) {
                token_t after_name = m_lexer.peek_after(after_lparen);
                if (after_name.type == token_type_e::in || after_name.type == token_type_e::comma) {
                    return for_each_statement();
                } else if (after_name.type == token_type_e::colon) {
                    // Handle typed for-each: for (item : type in collection)
                    token_t after_colon = m_lexer.peek_after(after_name);
                    if (after_colon.type == token_type_e::name) {
                        token_t after_type = m_lexer.peek_after(after_colon);
                        if (after_type.type == token_type_e::in || after_type.type == token_type_e::comma) {
                            return for_each_statement();
                        }
                    }
                }
            } else if (after_lparen.type == token_type_e::const_token) {
                // Handle const for-each: for (const item in collection) or for (const item : type in collection)
                token_t after_const = m_lexer.peek_after(after_lparen);
                if (after_const.type == token_type_e::name) {
                    token_t after_name = m_lexer.peek_after(after_const);
                    if (after_name.type == token_type_e::in || after_name.type == token_type_e::comma) {
                        return for_each_statement();
                    } else if (after_name.type == token_type_e::colon) {
                        token_t after_colon = m_lexer.peek_after(after_name);
                        if (after_colon.type == token_type_e::name) {
                            token_t after_type = m_lexer.peek_after(after_colon);
                            if (after_type.type == token_type_e::in || after_type.type == token_type_e::comma) {
                                return for_each_statement();
                            }
                        }
                    }
                }
            } else if (after_lparen.type == token_type_e::rparen) {
                 token_t after_rparen = m_lexer.peek_after(after_lparen);
                 if (after_rparen.type == token_type_e::in) {
                    return for_each_statement();
                 }
            }
        } else if (next_token.type == token_type_e::name) {
            token_t after_name = m_lexer.peek_after(next_token);
            if (after_name.type == token_type_e::in) {
                return for_each_statement();
            } else if (after_name.type == token_type_e::colon) {
                // Handle typed for-each: for item : type in collection
                token_t after_colon = m_lexer.peek_after(after_name);
                if (after_colon.type == token_type_e::name) {
                    token_t after_type = m_lexer.peek_after(after_colon);
                    if (after_type.type == token_type_e::in) {
                        return for_each_statement();
                    }
                }
            }
        } else if (next_token.type == token_type_e::const_token) {
            // Handle const for-each: for const item in collection or for const item : type in collection
            token_t after_const = m_lexer.peek_after(next_token);
            if (after_const.type == token_type_e::name) {
                token_t after_name = m_lexer.peek_after(after_const);
                if (after_name.type == token_type_e::in) {
                    return for_each_statement();
                } else if (after_name.type == token_type_e::colon) {
                    token_t after_colon = m_lexer.peek_after(after_name);
                    if (after_colon.type == token_type_e::name) {
                        token_t after_type = m_lexer.peek_after(after_colon);
                        if (after_type.type == token_type_e::in) {
                            return for_each_statement();
                        }
                    }
                }
            }
        }
        return for_statement();
    } else if (m_current_token.type == token_type_e::switch_token) {
        return switch_statement();
    } else if (m_current_token.type == token_type_e::do_token) {
        token_t do_token = m_current_token;
        eat(token_type_e::do_token);
        auto body = block();

        // Now check the keyword after the block
        if (m_current_token.type == token_type_e::while_token) {
            eat(token_type_e::while_token);
            auto condition = expression();
            return std::make_unique<do_while_statement_t>(std::move(body), std::move(condition), do_token.line, do_token.column, body->end_line, body->end_column);
        } else if (m_current_token.type == token_type_e::until) {
            eat(token_type_e::until);
            auto condition = expression();
            return std::make_unique<do_until_statement_t>(std::move(body), std::move(condition), do_token.line, do_token.column, body->end_line, body->end_column);
        } else {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
            throw zephyr::syntax_error_t("Expected while_token or until after do_token block.");
        }
    } else if (m_current_token.type == token_type_e::break_token) {
        return break_statement();
    } else if (m_current_token.type == token_type_e::continue_token) {
        return continue_statement();
    } else if (m_current_token.type == token_type_e::try_token) {
        return try_catch_statement();
    } else if ((m_current_token.type == token_type_e::name || m_current_token.type == token_type_e::this_token || m_current_token.type == token_type_e::super_token) &&
               m_lexer.peek_next_token().type == token_type_e::dot) {
        // Check if this is a member assignment
        token_t first_token = m_current_token;
        token_t dot_token = m_lexer.peek_next_token();
        token_t member_token = m_lexer.peek_after(dot_token);
        token_t assign_token = m_lexer.peek_after(member_token);
        if (assign_token.type == token_type_e::assign) {
            return member_assignment();
        }
    } else if (m_current_token.type == token_type_e::name) {
        // Check for increment/decrement operators
        token_t name_token = m_current_token;
        token_t next_token = m_lexer.peek_next_token();

        // Postfix increment/decrement (x++, x--)
        if (next_token.type == token_type_e::increment || next_token.type == token_type_e::decrement) {
            eat(token_type_e::name);
            bool is_increment = (m_current_token.type == token_type_e::increment);
            eat(m_current_token.type);
            return std::make_unique<increment_decrement_t>(name_token.text, is_increment, false, name_token.line, name_token.column, m_current_token.line, m_current_token.column);
        }
    } else if (m_current_token.type == token_type_e::increment || m_current_token.type == token_type_e::decrement) {
        // Prefix increment/decrement (++x, --x)
        token_t op_token = m_current_token;
        bool is_increment = (m_current_token.type == token_type_e::increment);
        eat(m_current_token.type);
        if (m_current_token.type != token_type_e::name) {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
            throw zephyr::syntax_error_t("Expected variable name after " + std::string(op_token.text));
        }
        token_t name_token = m_current_token;
        eat(token_type_e::name);
        return std::make_unique<increment_decrement_t>(name_token.text, is_increment, true, op_token.line, op_token.column, name_token.line, name_token.column);
    }
    // 3. General expression or assignment statement
    // This branch handles assignments and expression statements
    auto potential_lhs = expression();
    int stmt_line = potential_lhs->line;
    int stmt_column = potential_lhs->column;
    if (m_current_token.type == token_type_e::assign ||
        m_current_token.type == token_type_e::plus_assign ||
        m_current_token.type == token_type_e::minus_assign ||
        m_current_token.type == token_type_e::mul_assign ||
        m_current_token.type == token_type_e::div_assign ||
        m_current_token.type == token_type_e::modulo_assign ||
        m_current_token.type == token_type_e::power_assign) { // Added power_assign

        token_type_e assign_type = m_current_token.type;
        token_t assign_token = m_current_token;
        eat(assign_type);
        auto rhs = expression();

        // Determine if it's a simple variable assignment, indexed assignment, or member assignment
        if (auto name_node = dynamic_cast<name_t*>(potential_lhs.get())) {
            if (assign_type == token_type_e::assign) {
                return std::make_unique<assignment_t>(name_node->name, std::move(rhs), potential_lhs->line, potential_lhs->column, rhs->end_line, rhs->end_column);
            } else {
                // Compound assignment
                std::string op_str;
                switch (assign_type) {
                    case token_type_e::plus_assign: op_str = "+="; break;
                    case token_type_e::minus_assign: op_str = "-="; break;
                    case token_type_e::mul_assign: op_str = "*="; break;
                    case token_type_e::div_assign: op_str = "/="; break;
                    case token_type_e::modulo_assign: op_str = "%="; break;
                    case token_type_e::power_assign: op_str = "**="; break; // Added power_assign
                    default: zephyr::current_error_location() = {assign_token.line, assign_token.column, 1};
                             throw zephyr::syntax_error_t("Unknown compound assignment operator.");
                }
                return std::make_unique<compound_assignment_t>(name_node->name, std::move(rhs), op_str, potential_lhs->line, potential_lhs->column, rhs->end_line, rhs->end_column);
            }
        } else if (auto index_access_node = dynamic_cast<index_access_t*>(potential_lhs.get())) {
            if (assign_type == token_type_e::assign) {
                return std::make_unique<indexed_assignment_t>(std::move(index_access_node->object), std::move(index_access_node->index), std::move(rhs), potential_lhs->line, potential_lhs->column, rhs->end_line, rhs->end_line);
            } else {
                // Compound indexed assignment
                std::string op_str;
                switch (assign_type) {
                    case token_type_e::plus_assign: op_str = "+="; break;
                    case token_type_e::minus_assign: op_str = "-="; break;
                    case token_type_e::mul_assign: op_str = "*="; break;
                    case token_type_e::div_assign: op_str = "/="; break;
                    case token_type_e::modulo_assign: op_str = "%="; break;
                    case token_type_e::power_assign: op_str = "**="; break; // Added power_assign
                    default: zephyr::current_error_location() = {assign_token.line, assign_token.column, 1};
                             throw zephyr::syntax_error_t("Unknown compound assignment operator.");
                }
                return std::make_unique<compound_indexed_assignment_t>(std::move(index_access_node->object), std::move(index_access_node->index), std::move(rhs), op_str, potential_lhs->line, potential_lhs->column, rhs->end_line, rhs->end_column);
            }
        } else if (auto member_access_node = dynamic_cast<member_access_t*>(potential_lhs.get())) {
            if (assign_type == token_type_e::assign) {
                // Handle chained member assignment like this.calc.value = 5
                auto object = std::unique_ptr<expression_t>(member_access_node->object.release());
                std::string member_name = member_access_node->member_name;
                return std::make_unique<member_assignment_t>(std::move(object), member_name, std::move(rhs), potential_lhs->line, potential_lhs->column, rhs->end_line, rhs->end_column);
            } else {
                // Compound member assignment
                std::string op_str;
                switch (assign_type) {
                    case token_type_e::plus_assign: op_str = "+="; break;
                    case token_type_e::minus_assign: op_str = "-="; break;
                    case token_type_e::mul_assign: op_str = "*="; break;
                    case token_type_e::div_assign: op_str = "/="; break;
                    case token_type_e::modulo_assign: op_str = "%="; break;
                    case token_type_e::power_assign: op_str = "**="; break; // Added power_assign
                    default: zephyr::current_error_location() = {assign_token.line, assign_token.column, 1};
                             throw zephyr::syntax_error_t("Unknown compound assignment operator.");
                }
                auto object = std::unique_ptr<expression_t>(member_access_node->object.release());
                std::string member_name = member_access_node->member_name;
                return std::make_unique<compound_member_assignment_t>(std::move(object), member_name, std::move(rhs), op_str, potential_lhs->line, potential_lhs->column, rhs->end_line, rhs->end_column);
            }
        } else {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
            throw zephyr::syntax_error_t("Invalid left-hand side for assignment.");
        }
    } else {
        // It's an expression statement (e.g., function call, method call, chained calls)
        return std::make_unique<expression_statement_t>(std::move(potential_lhs), stmt_line, stmt_column, potential_lhs->end_line, potential_lhs->end_column);
    }
}



std::unique_ptr<expression_t> parser_t::expression() {
    return pipe_expression();
}

std::unique_ptr<expression_t> parser_t::pipe_expression() {
    auto node = ternary_expression(); // Start with the next higher precedence
    while (m_current_token.type == token_type_e::pipe) {
        token_t op_token = m_current_token;
        eat(token_type_e::pipe);
        // The right-hand side of the pipe operator is typically a function call or a lambda.
        // For simplicity, we'll parse it as a general expression for now.
        // The interpreter will need to ensure it's a callable.
        node = std::make_unique<pipe_op_t>(std::move(node), ternary_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::ternary_expression() {
    auto condition = nullish_coalescing_expression();

    if (m_current_token.type == token_type_e::question) {
        eat(token_type_e::question);
        auto then_expression = expression();
        eat(token_type_e::colon);
        auto else_expression = expression();
        return std::make_unique<ternary_expression_t>(std::move(condition), std::move(then_expression), std::move(else_expression), condition->line, condition->column, else_expression->end_line, else_expression->end_column);
    }

    return condition;
}

std::unique_ptr<expression_t> parser_t::nullish_coalescing_expression() {
    auto node = logical_or_expression(); // Start with the next higher precedence
    while (m_current_token.type == token_type_e::nullish_coalescing) {
        token_t op_token = m_current_token;
        eat(token_type_e::nullish_coalescing);
        node = std::make_unique<nullish_coalescing_op_t>(std::move(node), logical_or_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::logical_or_expression() {
    auto node = logical_and_expression();
    while (m_current_token.type == token_type_e::or_token || m_current_token.type == token_type_e::or_op) {
        token_t op_token = m_current_token;
        if (m_current_token.type == token_type_e::or_token) {
            eat(token_type_e::or_token);
        } else {
            eat(token_type_e::or_op);
        }
        node = std::make_unique<logical_or_op_t>(std::move(node), logical_and_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::logical_and_expression() {
    auto node = bitwise_or_expression();
    while (m_current_token.type == token_type_e::and_token || m_current_token.type == token_type_e::and_op) {
        token_t op_token = m_current_token;
        if (m_current_token.type == token_type_e::and_token) {
            eat(token_type_e::and_token);
        } else {
            eat(token_type_e::and_op);
        }
        node = std::make_unique<logical_and_op_t>(std::move(node), bitwise_or_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

// New bitwise parsing functions start here
std::unique_ptr<expression_t> parser_t::bitwise_or_expression() {
    auto node = bitwise_xor_expression();
    while (m_current_token.type == token_type_e::bitwise_or) {
        token_t op_token = m_current_token;
        eat(token_type_e::bitwise_or);
        node = std::make_unique<bitwise_or_op_t>(std::move(node), bitwise_xor_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::bitwise_xor_expression() {
    auto node = bitwise_and_expression();
    while (m_current_token.type == token_type_e::bitwise_xor) {
        token_t op_token = m_current_token;
        eat(token_type_e::bitwise_xor);
        node = std::make_unique<bitwise_xor_op_t>(std::move(node), bitwise_and_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::bitwise_and_expression() {
    auto node = comparison();
    while (m_current_token.type == token_type_e::bitwise_and) {
        token_t op_token = m_current_token;
        eat(token_type_e::bitwise_and);
        node = std::make_unique<bitwise_and_op_t>(std::move(node), shift_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::shift_expression() {
    auto node = additive_expression(); // Calls the next higher precedence
    while (m_current_token.type == token_type_e::left_shift || m_current_token.type == token_type_e::right_shift) {
        token_t op_token = m_current_token;
        if (m_current_token.type == token_type_e::left_shift) {
            eat(token_type_e::left_shift);
            node = std::make_unique<left_shift_op_t>(std::move(node), additive_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
        } else {
            eat(token_type_e::right_shift);
            node = std::make_unique<right_shift_op_t>(std::move(node), additive_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
        }
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::comparison() {
    auto node = shift_expression(); // Changed from additive_expression()
    while (m_current_token.type == token_type_e::eq || m_current_token.type == token_type_e::ne ||
           m_current_token.type == token_type_e::lt || m_current_token.type == token_type_e::le ||
           m_current_token.type == token_type_e::gt || m_current_token.type == token_type_e::ge ||
           m_current_token.type == token_type_e::is_token || m_current_token.type == token_type_e::in) {
        token_t op_token = m_current_token;
        if (m_current_token.type == token_type_e::in) {
            eat(token_type_e::in);
            node = std::make_unique<in_expression_t>(std::move(node), shift_expression(), op_token.line, op_token.column, m_current_token.line, m_current_token.column); // Changed from additive_expression()
        } else {
            eat(m_current_token.type);
            node = std::make_unique<comparison_op_t>(std::move(node), shift_expression(), op_token.text, op_token.line, op_token.column, m_current_token.line, m_current_token.column); // Changed from additive_expression()
        }
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::additive_expression() {
    auto node = term();
    while (m_current_token.type == token_type_e::plus || m_current_token.type == token_type_e::minus) {
        token_t op_token = m_current_token;
        if (m_current_token.type == token_type_e::plus) {
            eat(token_type_e::plus);
        } else {
            eat(token_type_e::minus);
        }
        node = std::make_unique<binary_op_t>(std::move(node), term(), op_token.text[0], op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::term() {
    auto node = power();
    while (m_current_token.type == token_type_e::mul || m_current_token.type == token_type_e::div || m_current_token.type == token_type_e::modulo) {
        token_t op_token = m_current_token;
        if (m_current_token.type == token_type_e::mul) {
            eat(token_type_e::mul);
        } else if (m_current_token.type == token_type_e::div) {
            eat(token_type_e::div);
        } else {
            eat(token_type_e::modulo);
        }
        auto right_operand = power();
        int end_line = right_operand->end_line;
        int end_column = right_operand->end_column;
        node = std::make_unique<binary_op_t>(std::move(node), std::move(right_operand), op_token.text[0], op_token.line, op_token.column, end_line, end_column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::power() {
    auto node = unary();
    while (m_current_token.type == token_type_e::power) {
        token_t op_token = m_current_token;
        eat(token_type_e::power);
        node = std::make_unique<power_op_t>(std::move(node), unary(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
    }
    return node;
}

std::unique_ptr<expression_t> parser_t::unary() {
    if (m_current_token.type == token_type_e::not_token || m_current_token.type == token_type_e::not_op ||
        m_current_token.type == token_type_e::minus || m_current_token.type == token_type_e::plus ||
        m_current_token.type == token_type_e::bitwise_not)
    {
        token_t op_token = m_current_token;
        char op_char = ' ';
        if (m_current_token.type == token_type_e::not_token) {
            eat(token_type_e::not_token);
            op_char = '!'; // Represent 'not' as '!' for unary_op_t
        } else if (m_current_token.type == token_type_e::not_op) {
            eat(token_type_e::not_op);
            op_char = '!';
        } else if (m_current_token.type == token_type_e::minus) {
            eat(token_type_e::minus);
            op_char = '-';
        } else if (m_current_token.type == token_type_e::plus) {
            eat(token_type_e::plus);
            op_char = '+';
        } else if (m_current_token.type == token_type_e::bitwise_not) { // Handle bitwise_not
            eat(token_type_e::bitwise_not);
            return std::make_unique<bitwise_not_op_t>(unary(), op_token.line, op_token.column, m_current_token.line, m_current_token.column);
        }
        return std::make_unique<unary_op_t>(op_char, unary(), op_token.line, op_token.column, m_current_token.line, m_current_token.column); // Unary operators can be chained
    }
    return factor();
}

std::unique_ptr<expression_t> parser_t::factor() {
    token_t token = m_current_token;
    if (token.type == token_type_e::async) {
        // Look ahead to see if this is an async lambda expression
        token_t next_token = m_lexer.peek_next_token();
        if (next_token.type == token_type_e::name || next_token.type == token_type_e::lparen || next_token.type == token_type_e::const_token) {
            // Handle different async lambda patterns:
            // async x -> expr
            // async const x -> expr
            // async (x, y) -> expr
            // async (const a, const b) -> expr

            if (next_token.type == token_type_e::name) {
                // Pattern: async x -> expr
                token_t third_token = m_lexer.peek_after(next_token);
                if (third_token.type == token_type_e::arrow || third_token.type == token_type_e::colon) {
                    return lambda_function();
                }
            } else if (next_token.type == token_type_e::const_token) {
                // Pattern: async const x -> expr
                token_t third_token = m_lexer.peek_after(next_token);
                if (third_token.type == token_type_e::name) {
                    token_t fourth_token = m_lexer.peek_after(third_token);
                    if (fourth_token.type == token_type_e::arrow || fourth_token.type == token_type_e::colon) {
                        return lambda_function();
                    }
                }
            } else if (next_token.type == token_type_e::lparen) {
                // Pattern: async (...) -> expr
                // Use simpler logic: assume anything with parentheses followed eventually by arrow is a lambda
                return lambda_function();
            }
        }
        // If not a lambda, this is an error - async can only be used with functions or lambdas
        zephyr::current_error_location() = {token.line, token.column, 1};
        throw zephyr::syntax_error_t("'async' can only be used with function definitions or lambda expressions");
    } else if (token.type == token_type_e::await) {
        // Await expression
        token_t await_token = m_current_token;
        eat(token_type_e::await);
        auto expr = unary();
        return std::make_unique<await_expression_t>(std::move(expr), await_token.line, await_token.column, expr->end_line, expr->end_column);
    } else if (token.type == token_type_e::spawn) {
        // Spawn expression
        token_t spawn_token = m_current_token;
        eat(token_type_e::spawn);
        auto expr = unary();
        return std::make_unique<spawn_expression_t>(std::move(expr), spawn_token.line, spawn_token.column, expr->end_line, expr->end_column);
    } else if (token.type == token_type_e::number) {
        eat(token_type_e::number);
        return std::make_unique<number_t>(std::stoi(std::string(token.text)), token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::float_token) {
        eat(token_type_e::float_token);
        return std::make_unique<float_literal_t>(std::stod(std::string(token.text)), token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::string) {
        eat(token_type_e::string);
        std::string processed = process_escape_sequences(std::string(token.text));
        return std::make_unique<string_literal_t>(processed, token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::fstring) {
        eat(token_type_e::fstring);
        std::vector<std::unique_ptr<expression_t>> parts;
        std::string current_part;
        for (size_t i = 0; i < token.text.length(); ++i) {
            if (token.text[i] == '{') {
                if (!current_part.empty()) {
                    std::string processed = process_escape_sequences(current_part);
                    parts.push_back(std::make_unique<string_literal_t>(processed, token.line, token.column, token.line, token.column + current_part.length() - 1));
                    current_part.clear();
                }
                i++; // Skip '{'
                size_t expr_start = i;
                int brace_count = 1;
                while (i < token.text.length() && brace_count > 0) {
                    if (token.text[i] == '{') brace_count++;
                    if (token.text[i] == '}') brace_count--;
                    i++;
                }
                if (brace_count != 0) {
                    zephyr::current_error_location() = {token.line, token.column, 1};
                throw zephyr::syntax_error_t("Unmatched braces in f-string");
                }
                std::string expr_str(token.text.substr(expr_start, i - expr_start - 1));
                lexer_t expr_lexer(expr_str);
                parser_t expr_parser(expr_lexer);
                parts.push_back(expr_parser.expression());
                i--; // Adjust for loop increment
            } else {
                current_part += token.text[i];
            }
        }
        if (!current_part.empty()) {
            std::string processed = process_escape_sequences(current_part);
            parts.push_back(std::make_unique<string_literal_t>(processed, token.line, token.column, token.line, token.column + current_part.length() - 1));
        }
        return std::make_unique<fstring_t>(std::move(parts), token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::true_token) {
        eat(token_type_e::true_token);
        return std::make_unique<boolean_literal_t>(true, token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::false_token) {
        eat(token_type_e::false_token);
        return std::make_unique<boolean_literal_t>(false, token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::none) {
        eat(token_type_e::none);
        return std::make_unique<none_literal_t>(token.line, token.column, token.line, token.column + token.text.length() - 1);
    } else if (token.type == token_type_e::this_token) {
        eat(token_type_e::this_token);
        std::unique_ptr<expression_t> expr = std::make_unique<this_expression_t>(token.line, token.column, token.line, token.column + token.text.length() - 1);

        // Handle chained member access/method calls and indexing in any order
        while (m_current_token.type == token_type_e::dot || m_current_token.type == token_type_e::lbracket || m_current_token.type == token_type_e::question_dot || (m_current_token.type == token_type_e::question && m_lexer.peek_next_token().type == token_type_e::lbracket)) {
            if (m_current_token.type == token_type_e::dot) {
                eat(token_type_e::dot);
                std::string member_name(m_current_token.text);
                token_t member_token = m_current_token;
                eat(token_type_e::name);

                if (m_current_token.type == token_type_e::lparen) {
                    // Method call - continue the chain
                    expr = method_call(std::move(expr), member_name, member_token);
                } else {
                    // Member access - continue the chain
                    expr = member_access(std::move(expr), member_name, member_token);
                }
            } else if (m_current_token.type == token_type_e::question_dot) { // New optional chaining
                token_t optional_chain_token = m_current_token;
                eat(token_type_e::question_dot);
                if (m_current_token.type == token_type_e::name) {
                    std::string member_name(m_current_token.text);
                    token_t member_token = m_current_token;
                    eat(token_type_e::name);
                    if (m_current_token.type == token_type_e::lparen) {
                        // Optional method call
                        std::vector<std::unique_ptr<expression_t>> arguments;
                        eat(token_type_e::lparen);
                        if (m_current_token.type != token_type_e::rparen) {
                            arguments.push_back(expression());
                            while (m_current_token.type == token_type_e::comma) {
                                eat(token_type_e::comma);
                                arguments.push_back(expression());
                            }
                        }
                        token_t rparen_token = m_current_token;
                        eat(token_type_e::rparen);
                        expr = std::make_unique<optional_method_call_t>(std::move(expr), member_name, std::move(arguments), optional_chain_token.line, optional_chain_token.column, rparen_token.line, rparen_token.column);
                    } else {
                        // Optional member access
                        expr = std::make_unique<optional_member_access_t>(std::move(expr), member_name, optional_chain_token.line, optional_chain_token.column, member_token.line, member_token.column);
                    }
                } else if (m_current_token.type == token_type_e::lbracket) {
                    // Optional index access
                    token_t lbracket_token = m_current_token;
                    eat(token_type_e::lbracket);
                    auto index_expr = expression();
                    token_t rbracket_token = m_current_token;
                    eat(token_type_e::rbracket);
                    expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), optional_chain_token.line, optional_chain_token.column, rbracket_token.line, rbracket_token.column);
                } else {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw zephyr::syntax_error_t("Expected member name, method call, or index access after '?.'");
                }

            } else if (m_current_token.type == token_type_e::question_dot) { // New optional chaining
                token_t optional_chain_token = m_current_token;
                eat(token_type_e::question_dot);
                if (m_current_token.type == token_type_e::name) {
                    std::string member_name(m_current_token.text);
                    token_t member_token = m_current_token;
                    eat(token_type_e::name);
                    if (m_current_token.type == token_type_e::lparen) {
                        // Optional method call
                        std::vector<std::unique_ptr<expression_t>> arguments;
                        eat(token_type_e::lparen);
                        if (m_current_token.type != token_type_e::rparen) {
                            arguments.push_back(expression());
                            while (m_current_token.type == token_type_e::comma) {
                                eat(token_type_e::comma);
                                arguments.push_back(expression());
                            }
                        }
                        token_t rparen_token = m_current_token;
                        eat(token_type_e::rparen);
                        expr = std::make_unique<optional_method_call_t>(std::move(expr), member_name, std::move(arguments), optional_chain_token.line, optional_chain_token.column, rparen_token.line, rparen_token.column);
                    } else {
                        // Optional member access
                        expr = std::make_unique<optional_member_access_t>(std::move(expr), member_name, optional_chain_token.line, optional_chain_token.column, member_token.line, member_token.column);
                    }
                } else if (m_current_token.type == token_type_e::lbracket) {
                    // Optional index access
                    token_t lbracket_token = m_current_token;
                    eat(token_type_e::lbracket);
                    auto index_expr = expression();
                    token_t rbracket_token = m_current_token;
                    eat(token_type_e::rbracket);
                    expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), optional_chain_token.line, optional_chain_token.column, rbracket_token.line, rbracket_token.column);
                } else {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw zephyr::syntax_error_t("Expected member name, method call, or index access after '?.'");
                }
            } else if (m_current_token.type == token_type_e::question && m_lexer.peek_next_token().type == token_type_e::lbracket) { // Optional Index Access (?[])
                token_t question_token = m_current_token;
                eat(token_type_e::question); // Consume '?'
                token_t lbracket_token = m_current_token;
                eat(token_type_e::lbracket);
                auto index_expr = expression();
                token_t rbracket_token = m_current_token;
                eat(token_type_e::rbracket);
                expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), question_token.line, question_token.column, rbracket_token.line, rbracket_token.column);
            } else if (m_current_token.type == token_type_e::lbracket) {
                // Index access - continue the chain
                expr = index_access(std::move(expr));
            }
        }

        // Check for postfix increment/decrement after all chaining is complete
        if (m_current_token.type == token_type_e::increment || m_current_token.type == token_type_e::decrement) {
            bool is_increment = (m_current_token.type == token_type_e::increment);
            token_t op_token = m_current_token;
            eat(m_current_token.type);

            if (auto member_node = dynamic_cast<member_access_t*>(expr.get())) {
                // Member increment/decrement (this.member++)
                auto object = std::unique_ptr<expression_t>(member_node->object.release());
                std::string member_name = member_node->member_name;
                return std::make_unique<member_increment_decrement_t>(std::move(object), member_name, is_increment, false, token.line, token.column, op_token.line, op_token.column + 1);
            } else if (auto index_node = dynamic_cast<index_access_t*>(expr.get())) {
                // Indexed increment/decrement (this[i]++)
                auto target = std::unique_ptr<expression_t>(index_node->object.release());
                auto index = std::unique_ptr<expression_t>(index_node->index.release());
                return std::make_unique<indexed_increment_decrement_t>(std::move(target), std::move(index), is_increment, false, token.line, token.column, op_token.line, op_token.column + 1);
            } else {
                zephyr::current_error_location() = {op_token.line, op_token.column, 1};
                throw zephyr::syntax_error_t("Postfix increment/decrement not supported on this expression type.");
            }
        }

        return expr;
    } else if (token.type == token_type_e::super_token) {
        eat(token_type_e::super_token);
        std::unique_ptr<expression_t> expr = std::make_unique<super_expression_t>(token.line, token.column, token.line, token.column + token.text.length() - 1);

        // Handle chained member access/method calls and indexing in any order
        while (m_current_token.type == token_type_e::dot || m_current_token.type == token_type_e::lbracket || m_current_token.type == token_type_e::question_dot || (m_current_token.type == token_type_e::question && m_lexer.peek_next_token().type == token_type_e::lbracket)) {
            if (m_current_token.type == token_type_e::dot) {
                eat(token_type_e::dot);
                std::string member_name(m_current_token.text);
                token_t member_token = m_current_token;
                eat(token_type_e::name);

                if (m_current_token.type == token_type_e::lparen) {
                    // Method call - continue the chain
                    expr = method_call(std::move(expr), member_name, member_token);
                } else {
                    // Member access - continue the chain
                    expr = member_access(std::move(expr), member_name, member_token);
                }
            } else if (m_current_token.type == token_type_e::question_dot) { // New optional chaining
                token_t optional_chain_token = m_current_token;
                eat(token_type_e::question_dot);
                if (m_current_token.type == token_type_e::name) {
                    std::string member_name(m_current_token.text);
                    token_t member_token = m_current_token;
                    eat(token_type_e::name);
                    if (m_current_token.type == token_type_e::lparen) {
                        // Optional method call
                        std::vector<std::unique_ptr<expression_t>> arguments;
                        eat(token_type_e::lparen);
                        if (m_current_token.type != token_type_e::rparen) {
                            arguments.push_back(expression());
                            while (m_current_token.type == token_type_e::comma) {
                                eat(token_type_e::comma);
                                arguments.push_back(expression());
                            }
                        }
                        token_t rparen_token = m_current_token;
                        eat(token_type_e::rparen);
                        expr = std::make_unique<optional_method_call_t>(std::move(expr), member_name, std::move(arguments), optional_chain_token.line, optional_chain_token.column, rparen_token.line, rparen_token.column);
                    } else {
                        // Optional member access
                        expr = std::make_unique<optional_member_access_t>(std::move(expr), member_name, optional_chain_token.line, optional_chain_token.column, member_token.line, member_token.column);
                    }
                } else if (m_current_token.type == token_type_e::lbracket) {
                    // Optional index access
                    token_t lbracket_token = m_current_token;
                    eat(token_type_e::lbracket);
                    auto index_expr = expression();
                    token_t rbracket_token = m_current_token;
                    eat(token_type_e::rbracket);
                    expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), optional_chain_token.line, optional_chain_token.column, rbracket_token.line, rbracket_token.column);
                } else {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw zephyr::syntax_error_t("Expected member name, method call, or index access after '?.'");
                }
            } else if (m_current_token.type == token_type_e::question && m_lexer.peek_next_token().type == token_type_e::lbracket) { // Optional Index Access (?[])
                token_t question_token = m_current_token;
                eat(token_type_e::question); // Consume '?'
                token_t lbracket_token = m_current_token;
                eat(token_type_e::lbracket);
                auto index_expr = expression();
                token_t rbracket_token = m_current_token;
                eat(token_type_e::rbracket);
                expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), question_token.line, question_token.column, rbracket_token.line, rbracket_token.column);
            } else if (m_current_token.type == token_type_e::lbracket) {
                // Index access - continue the chain
                expr = index_access(std::move(expr));
            }
        }

        // Check for postfix increment/decrement after all chaining is complete
        if (m_current_token.type == token_type_e::increment || m_current_token.type == token_type_e::decrement) {
            bool is_increment = (m_current_token.type == token_type_e::increment);
            token_t op_token = m_current_token;
            eat(m_current_token.type);

            if (auto member_node = dynamic_cast<member_access_t*>(expr.get())) {
                // Member increment/decrement (super.member++)
                auto object = std::unique_ptr<expression_t>(member_node->object.release());
                std::string member_name = member_node->member_name;
                return std::make_unique<member_increment_decrement_t>(std::move(object), member_name, is_increment, false, token.line, token.column, op_token.line, op_token.column + 1);
            } else if (auto index_node = dynamic_cast<index_access_t*>(expr.get())) {
                // Indexed increment/decrement (super[i]++)
                auto target = std::unique_ptr<expression_t>(index_node->object.release());
                auto index = std::unique_ptr<expression_t>(index_node->index.release());
                return std::make_unique<indexed_increment_decrement_t>(std::move(target), std::move(index), is_increment, false, token.line, token.column, op_token.line, op_token.column + 1);
            } else {
                zephyr::current_error_location() = {op_token.line, op_token.column, 1};
                throw zephyr::syntax_error_t("Postfix increment/decrement not supported on this expression type.");
            }
        }

        return expr;
    } else if (token.type == token_type_e::name || token.type == token_type_e::const_token) { // Added token_type_e::const_token
        // Look ahead for arrow to detect lambda expression
        token_t next_token = m_lexer.peek_next_token();
        if (next_token.type == token_type_e::arrow || (next_token.type == token_type_e::name && m_lexer.peek_after(next_token).type == token_type_e::arrow)) { // Added check for const x ->
            return lambda_function(); // Call the new lambda_function
        }

        // Only handle name tokens here, not const_token tokens
        if (token.type != token_type_e::name) {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
            throw zephyr::syntax_error_t("Expected name token");
        }

        std::string name(token.text);
        token_t name_token = token;
        eat(token_type_e::name);

        std::unique_ptr<expression_t> expr;
        if (m_current_token.type == token_type_e::lparen) {
            expr = function_call(name, name_token);
        } else {
            expr = std::make_unique<name_t>(name, name_token.line, name_token.column, name_token.line, name_token.column + name.length() - 1);
        }

        // Handle chained member access/method calls and indexing in any order
        while (m_current_token.type == token_type_e::dot || m_current_token.type == token_type_e::lbracket || m_current_token.type == token_type_e::question_dot || (m_current_token.type == token_type_e::question && m_lexer.peek_next_token().type == token_type_e::lbracket)) {
            if (m_current_token.type == token_type_e::dot) {
                eat(token_type_e::dot);
                std::string member_name(m_current_token.text);
                token_t member_token = m_current_token;
                eat(token_type_e::name);

                if (m_current_token.type == token_type_e::lparen) {
                    // Method call - continue the chain
                    expr = method_call(std::move(expr), member_name, member_token);
                } else {
                    // Member access - continue the chain
                    expr = member_access(std::move(expr), member_name, member_token);
                }
            } else if (m_current_token.type == token_type_e::question_dot) { // New optional chaining
                token_t optional_chain_token = m_current_token;
                eat(token_type_e::question_dot);
                if (m_current_token.type == token_type_e::name) {
                    std::string member_name(m_current_token.text);
                    token_t member_token = m_current_token;
                    eat(token_type_e::name);
                    if (m_current_token.type == token_type_e::lparen) {
                        // Optional method call
                        std::vector<std::unique_ptr<expression_t>> arguments;
                        eat(token_type_e::lparen);
                        if (m_current_token.type != token_type_e::rparen) {
                            arguments.push_back(expression());
                            while (m_current_token.type == token_type_e::comma) {
                                eat(token_type_e::comma);
                                arguments.push_back(expression());
                            }
                        }
                        token_t rparen_token = m_current_token;
                        eat(token_type_e::rparen);
                        expr = std::make_unique<optional_method_call_t>(std::move(expr), member_name, std::move(arguments), optional_chain_token.line, optional_chain_token.column, rparen_token.line, rparen_token.column);
                    } else {
                        // Optional member access
                        expr = std::make_unique<optional_member_access_t>(std::move(expr), member_name, optional_chain_token.line, optional_chain_token.column, member_token.line, member_token.column);
                    }
                } else if (m_current_token.type == token_type_e::lbracket) {
                    // Optional index access
                    token_t lbracket_token = m_current_token;
                    eat(token_type_e::lbracket);
                    auto index_expr = expression();
                    token_t rbracket_token = m_current_token;
                    eat(token_type_e::rbracket);
                    expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), optional_chain_token.line, optional_chain_token.column, rbracket_token.line, rbracket_token.column);
                } else {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw zephyr::syntax_error_t("Expected member name, method call, or index access after '?.'");
                }

            } else if (m_current_token.type == token_type_e::question_dot) { // New optional chaining
                token_t optional_chain_token = m_current_token;
                eat(token_type_e::question_dot);
                if (m_current_token.type == token_type_e::name) {
                    std::string member_name(m_current_token.text);
                    token_t member_token = m_current_token;
                    eat(token_type_e::name);
                    if (m_current_token.type == token_type_e::lparen) {
                        // Optional method call
                        std::vector<std::unique_ptr<expression_t>> arguments;
                        eat(token_type_e::lparen);
                        if (m_current_token.type != token_type_e::rparen) {
                            arguments.push_back(expression());
                            while (m_current_token.type == token_type_e::comma) {
                                eat(token_type_e::comma);
                                arguments.push_back(expression());
                            }
                        }
                        token_t rparen_token = m_current_token;
                        eat(token_type_e::rparen);
                        expr = std::make_unique<optional_method_call_t>(std::move(expr), member_name, std::move(arguments), optional_chain_token.line, optional_chain_token.column, rparen_token.line, rparen_token.column);
                    } else {
                        // Optional member access
                        expr = std::make_unique<optional_member_access_t>(std::move(expr), member_name, optional_chain_token.line, optional_chain_token.column, member_token.line, member_token.column);
                    }
                } else if (m_current_token.type == token_type_e::lbracket) {
                    // Optional index access
                    token_t lbracket_token = m_current_token;
                    eat(token_type_e::lbracket);
                    auto index_expr = expression();
                    token_t rbracket_token = m_current_token;
                    eat(token_type_e::rbracket);
                    expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), optional_chain_token.line, optional_chain_token.column, rbracket_token.line, rbracket_token.column);
                } else {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw zephyr::syntax_error_t("Expected member name, method call, or index access after '?.'");
                }
            } else if (m_current_token.type == token_type_e::question && m_lexer.peek_next_token().type == token_type_e::lbracket) { // Optional Index Access (?[])
                token_t question_token = m_current_token;
                eat(token_type_e::question); // Consume '?'
                token_t lbracket_token = m_current_token;
                eat(token_type_e::lbracket);
                auto index_expr = expression();
                token_t rbracket_token = m_current_token;
                eat(token_type_e::rbracket);
                expr = std::make_unique<optional_index_access_t>(std::move(expr), std::move(index_expr), question_token.line, question_token.column, rbracket_token.line, rbracket_token.column);
            } else if (m_current_token.type == token_type_e::lbracket) {
                // Index access - continue the chain
                expr = index_access(std::move(expr));
            }
        }



        return expr;
    } else if (token.type == token_type_e::lparen) {
        // Look ahead to check if this might be a lambda expression
        token_t next_token = m_lexer.peek_next_token();
        if (next_token.type == token_type_e::rparen) {
            token_t after_rparen = m_lexer.peek_after(next_token);
            if (after_rparen.type == token_type_e::arrow) {
                return lambda_expression();
            }
        } else if (next_token.type == token_type_e::name || next_token.type == token_type_e::const_token) {
            // This could be either (expr) or (param, ...) ->
            // We'll need to check if it's followed by comma, colon (for type), or ) ->
            token_t check_token = next_token;
            if (next_token.type == token_type_e::const_token) {
                // For (const x), we need to look after the const keyword
                check_token = m_lexer.peek_after(next_token);
                if (check_token.type != token_type_e::name) {
                    // Fall through to regular parenthesized expression handling
                } else {
                    check_token = m_lexer.peek_after(check_token);
                }
            } else {
                // For (x), look after the name
                check_token = m_lexer.peek_after(next_token);
            }

            // Check for lambda patterns: (x, ...), (x : type, ...), (x) ->, (x : type) ->
            if (check_token.type == token_type_e::comma ||
                check_token.type == token_type_e::colon ||
                (check_token.type == token_type_e::rparen && m_lexer.peek_after(check_token).type == token_type_e::arrow)) {
                return lambda_expression();
            }
        }

        eat(token_type_e::lparen);
        auto node = expression();
        eat(token_type_e::rparen);
        return node;
    } else if (token.type == token_type_e::lbracket) {
        return list_literal();
    } else if (token.type == token_type_e::lbrace) {
        return dictionary_literal();
    } else {
        std::string error_msg = "Invalid factor in expression. Expected number, float_token, string, true_token, false_token, name, lparen, lbracket, lbrace, or lambda expression, but got " +
                                token_type_to_string(m_current_token.type) +
                                " ('" + std::string(m_current_token.text) + "').";
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t(error_msg);
    }
}

std::unique_ptr<dictionary_literal_t> parser_t::dictionary_literal() {
    token_t dict_token = m_current_token;
    auto dict_node = std::make_unique<dictionary_literal_t>(dict_token.line, dict_token.column, dict_token.line, dict_token.column);
    eat(token_type_e::lbrace);
    if (m_current_token.type != token_type_e::rbrace) {
        auto key = expression();
        eat(token_type_e::colon);
        auto value = expression();
        dict_node->add_element(std::move(key), std::move(value));
        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            key = expression();
            eat(token_type_e::colon);
            value = expression();
            dict_node->add_element(std::move(key), std::move(value));
        }
    }
    token_t rbrace_token = m_current_token;
    eat(token_type_e::rbrace);
    dict_node->end_line = rbrace_token.line;
    dict_node->end_column = rbrace_token.column;
    return dict_node;
}

std::unique_ptr<block_t> parser_t::block() {
    token_t brace_token = m_current_token;
    auto block_node = std::make_unique<block_t>(brace_token.line, brace_token.column, brace_token.line, brace_token.column);
    eat(token_type_e::lbrace);
    while (m_current_token.type != token_type_e::rbrace && m_current_token.type != token_type_e::end_of_file) {
        block_node->add_statement(statement());
    }
    token_t rbrace_token = m_current_token;
    eat(token_type_e::rbrace);
    block_node->end_line = rbrace_token.line;
    block_node->end_column = rbrace_token.column;
    return block_node;
}

std::unique_ptr<if_statement_t> parser_t::if_statement() {
    token_t if_token = m_current_token;
    eat(token_type_e::if_token);
    auto condition = expression();
    auto then_block = block();

    std::unique_ptr<block_t> else_block = nullptr;
    if (m_current_token.type == token_type_e::else_token) {
        eat(token_type_e::else_token);
        // Check if this is an "else if"
        if (m_current_token.type == token_type_e::if_token) {
            // Create a new block containing the nested if statement
            auto nested_if = if_statement();
            else_block = std::make_unique<block_t>();
            else_block->statements.push_back(std::move(nested_if));
        } else {
            // Regular else block
            else_block = block();
        }
    }
    return std::make_unique<if_statement_t>(std::move(condition), std::move(then_block), std::move(else_block), if_token.line, if_token.column, m_current_token.line, m_current_token.column);
}

std::unique_ptr<while_statement_t> parser_t::while_statement() {
    token_t while_token = m_current_token;
    eat(token_type_e::while_token);
    auto condition = expression();
    auto body = block();
    return std::make_unique<while_statement_t>(std::move(condition), std::move(body), while_token.line, while_token.column, m_current_token.line, m_current_token.column);
}

std::unique_ptr<do_while_statement_t> parser_t::do_while_statement() {
    // Logic moved to statement()
    throw internal_error_t("do_while_statement should not be called directly.");
}

std::unique_ptr<do_until_statement_t> parser_t::do_until_statement() {
    // Logic moved to statement()
    throw internal_error_t("do_until_statement should not be called directly.");
}

std::unique_ptr<for_statement_t> parser_t::for_statement() {
    token_t for_token = m_current_token;
    eat(token_type_e::for_token);
    bool has_lparen = false;
    if (m_current_token.type == token_type_e::lparen) {
        eat(token_type_e::lparen);
        has_lparen = true;
    }

    std::unique_ptr<statement_t> initialization = nullptr;
    if (m_current_token.type != token_type_e::semicolon) {
        initialization = statement();
    }
    eat(token_type_e::semicolon);

    std::unique_ptr<expression_t> condition = nullptr;
    if (m_current_token.type != token_type_e::semicolon) {
        condition = expression();
    }
    eat(token_type_e::semicolon);

    std::unique_ptr<statement_t> increment = nullptr;
    if (m_current_token.type != token_type_e::rparen && m_current_token.type != token_type_e::lbrace) {
        increment = statement();
    }
    if (has_lparen) {
        eat(token_type_e::rparen);
    }
    auto body = block();
    return std::make_unique<for_statement_t>(std::move(initialization), std::move(condition), std::move(increment), std::move(body), for_token.line, for_token.column, m_current_token.line, m_current_token.column);
}

std::unique_ptr<parser_t::for_each_head_struct_t> parser_t::for_each_head() {
    auto head = std::make_unique<for_each_head_struct_t>();
    if (m_current_token.type == token_type_e::lparen) {
        eat(token_type_e::lparen);
        if (m_current_token.type == token_type_e::name || m_current_token.type == token_type_e::const_token) {
            // Parse first variable with optional const and type
            bool is_const = false;
            if (m_current_token.type == token_type_e::const_token) {
                is_const = true;
                eat(token_type_e::const_token);
            }

            std::string var_name(m_current_token.text);
            eat(token_type_e::name);

            std::string type_name = "";
            bool has_explicit_type = false;
            if (m_current_token.type == token_type_e::colon) {
                eat(token_type_e::colon);
                type_name = m_current_token.text;
                has_explicit_type = true;
                eat(token_type_e::name);
            }

            head->variables.push_back(for_each_variable_t(var_name, type_name, has_explicit_type, is_const));

            while (m_current_token.type == token_type_e::comma) {
                eat(token_type_e::comma);

                bool is_const_loop = false;
                if (m_current_token.type == token_type_e::const_token) {
                    is_const_loop = true;
                    eat(token_type_e::const_token);
                }

                var_name = m_current_token.text;
                eat(token_type_e::name);

                type_name = "";
                has_explicit_type = false;
                if (m_current_token.type == token_type_e::colon) {
                    eat(token_type_e::colon);
                    type_name = m_current_token.text;
                    has_explicit_type = true;
                    eat(token_type_e::name);
                }

                head->variables.push_back(for_each_variable_t(var_name, type_name, has_explicit_type, is_const_loop));
            }
        }
        eat(token_type_e::rparen);
    } else if (m_current_token.type == token_type_e::name || m_current_token.type == token_type_e::const_token) {
        bool is_const = false;
        if (m_current_token.type == token_type_e::const_token) {
            is_const = true;
            eat(token_type_e::const_token);
        }

        std::string var_name(m_current_token.text);
        eat(token_type_e::name);

        std::string type_name = "";
        bool has_explicit_type = false;
        if (m_current_token.type == token_type_e::colon) {
            eat(token_type_e::colon);
            type_name = m_current_token.text;
            has_explicit_type = true;
            eat(token_type_e::name);
        }

        head->variables.push_back(for_each_variable_t(var_name, type_name, has_explicit_type, is_const));
    } else {
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t("Invalid for-each loop head.");
    }
    return head;
}

std::unique_ptr<for_each_statement_t> parser_t::for_each_statement() {
    token_t for_token = m_current_token;
    eat(token_type_e::for_token);

    bool has_lparen = false;
    if (m_current_token.type == token_type_e::lparen) {
        token_t next = m_lexer.peek_next_token();
        if (next.type == token_type_e::name) {
            token_t after_name = m_lexer.peek_after(next);
            if (after_name.type == token_type_e::in) {
                has_lparen = true;
                eat(token_type_e::lparen);
            }
        }
    }

    auto head = for_each_head();

    eat(token_type_e::in);

    auto collection = expression();

    std::unique_ptr<expression_t> where_clause = nullptr;
    if (m_current_token.type == token_type_e::where) {
        eat(token_type_e::where);
        where_clause = expression();
    }

    if (has_lparen) {
        eat(token_type_e::rparen);
    }

    auto body = block();

    return std::make_unique<for_each_statement_t>(std::move(head->variables), std::move(collection), std::move(body), std::move(where_clause), for_token.line, for_token.column, m_current_token.line, m_current_token.column);
}

std::unique_ptr<loop_statement_t> parser_t::loop_statement() {
    token_t loop_token = m_current_token;
    eat(token_type_e::loop);
    auto body = block();
    return std::make_unique<loop_statement_t>(std::move(body), loop_token.line, loop_token.column, body->end_line, body->end_column);
}

std::unique_ptr<list_literal_t> parser_t::list_literal() {
    token_t list_token = m_current_token;
    auto list_node = std::make_unique<list_literal_t>(list_token.line, list_token.column, list_token.line, list_token.column);
    eat(token_type_e::lbracket);
    if (m_current_token.type != token_type_e::rbracket) {
        list_node->add_element(expression());
        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            list_node->add_element(expression());
        }
    }
    token_t rbracket_token = m_current_token;
    eat(token_type_e::rbracket);
    list_node->end_line = rbracket_token.line;
    list_node->end_column = rbracket_token.column;
    return list_node;
}

std::unique_ptr<index_access_t> parser_t::index_access(std::unique_ptr<expression_t> list_expr) {
    token_t bracket_token = m_current_token;
    eat(token_type_e::lbracket);
    auto index_expr = expression();
    token_t rbracket_token = m_current_token;
    eat(token_type_e::rbracket);
    return std::make_unique<index_access_t>(std::move(list_expr), std::move(index_expr), bracket_token.line, bracket_token.column, rbracket_token.line, rbracket_token.column);
}

std::unique_ptr<function_definition_t> parser_t::function_definition() {
    token_t func_token = m_current_token;
    bool async = false;
    bool is_abstract = false;
    
    // Check for abstract modifier
    if (m_current_token.type == token_type_e::abstract_token) {
        is_abstract = true;
        eat(token_type_e::abstract_token);
    }
    
    // Check for async modifier
    if (m_current_token.type == token_type_e::async) {
        async = true;
        eat(token_type_e::async);
    }
    
    eat(token_type_e::func);
    token_t name_token = m_current_token;
    eat(token_type_e::name);

    if (name_token.text == "init" && m_current_token.type == token_type_e::colon) {
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t("init method cannot have an explicit return type");
    }

    eat(token_type_e::lparen);
    std::vector<parameter_t> parameters;
    if (m_current_token.type != token_type_e::rparen) {
        bool is_const = false;
        if (m_current_token.type == token_type_e::const_token) {
            is_const = true;
            eat(token_type_e::const_token);
        }
        std::string param_name(m_current_token.text);
        eat(token_type_e::name);
        std::string type_name = "";
        bool has_explicit_type = false;
        if (m_current_token.type == token_type_e::colon) {
            eat(token_type_e::colon);
            type_name = m_current_token.text;
            has_explicit_type = true;
            eat(token_type_e::name);
        }
        parameters.push_back({param_name, is_const, type_name, has_explicit_type});

        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            is_const = false;
            if (m_current_token.type == token_type_e::const_token) {
                is_const = true;
                eat(token_type_e::const_token);
            }
            param_name = m_current_token.text;
            eat(token_type_e::name);
            type_name = "";
            has_explicit_type = false;
            if (m_current_token.type == token_type_e::colon) {
                eat(token_type_e::colon);
                type_name = m_current_token.text;
                has_explicit_type = true;
                eat(token_type_e::name);
            }
            parameters.push_back({param_name, is_const, type_name, has_explicit_type});
        }
    }
    eat(token_type_e::rparen);

    std::string return_type_name = "";
    bool explicit_return_type = false;
    if (m_current_token.type == token_type_e::colon) {
        eat(token_type_e::colon);
        return_type_name = m_current_token.text;
        explicit_return_type = true;
        eat(token_type_e::name);
    }

    // Abstract functions don't have a body
    std::unique_ptr<block_t> body = nullptr;
    if (!is_abstract) {
        body = block();
    }
    
    return std::make_unique<function_definition_t>(name_token.text, std::move(parameters), std::move(body), return_type_name, explicit_return_type, async, false, is_abstract,
                                               func_token.line, func_token.column, m_current_token.line, m_current_token.column);
}

std::unique_ptr<return_statement_t> parser_t::return_statement() {
    token_t return_token = m_current_token;
    eat(token_type_e::return_token);
    std::unique_ptr<expression_t> return_value = nullptr;
    // Check if there's an expression to return
    if (m_current_token.type != token_type_e::semicolon && m_current_token.type != token_type_e::rbrace) {
        return_value = expression();
    }
    return std::make_unique<return_statement_t>(std::move(return_value), return_token.line, return_token.column, m_current_token.line, m_current_token.column);
}

std::unique_ptr<function_call_t> parser_t::function_call(std::string name, token_t name_token) {
    token_t lparen_token = m_current_token;
    eat(token_type_e::lparen);
    std::vector<std::unique_ptr<expression_t>> arguments;
    if (m_current_token.type != token_type_e::rparen) {
        arguments.push_back(expression());
        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            arguments.push_back(expression());
        }
    }
    token_t rparen_token = m_current_token;
    eat(token_type_e::rparen);
    return std::make_unique<function_call_t>(name, std::move(arguments), name_token.line, name_token.column, rparen_token.line, rparen_token.column);
}

std::unique_ptr<list_destructuring_assignment_t> parser_t::list_destructuring_assignment() {
    token_t lbracket_token = m_current_token;
    eat(token_type_e::lbracket);

    std::vector<std::string> variable_names;
    if (m_current_token.type == token_type_e::name) {
        variable_names.push_back(std::string(m_current_token.text));
        eat(token_type_e::name);
        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            variable_names.push_back(std::string(m_current_token.text));
            eat(token_type_e::name);
        }
    }
    eat(token_type_e::rbracket);

    eat(token_type_e::assign);

    auto assigned_expression = expression();

    return std::make_unique<list_destructuring_assignment_t>(variable_names, std::move(assigned_expression), lbracket_token.line, lbracket_token.column, assigned_expression->end_line, assigned_expression->end_column);
}

bool parser_t::is_list_destructuring_assignment_lookahead() {
    if (m_current_token.type != token_type_e::lbracket) {
        return false;
    }

    // Peek tokens without consuming them
    token_t peek_token = m_lexer.peek_next_token(); // Should be name or rbracket
    int offset = 1; // Offset for peeking

    // Handle empty list destructuring: [] = ...
    if (peek_token.type == token_type_e::rbracket) {
        peek_token = m_lexer.peek_after(peek_token); // Should be assign
        if (peek_token.type == token_type_e::assign) {
            return true;
        }
        return false;
    }

    // Handle non-empty list destructuring: [a, b, c] = ...
    if (peek_token.type == token_type_e::name) {
        offset++;
        peek_token = m_lexer.peek_after(peek_token); // Should be comma or rbracket

        while (peek_token.type == token_type_e::comma) {
            offset++;
            peek_token = m_lexer.peek_after(peek_token); // Should be name
            if (peek_token.type != token_type_e::name) {
                return false; // Expected name after comma
            }
            offset++;
            peek_token = m_lexer.peek_after(peek_token); // Should be comma or rbracket
        }

        if (peek_token.type == token_type_e::rbracket) {
            offset++;
            peek_token = m_lexer.peek_after(peek_token); // Should be assign
            if (peek_token.type == token_type_e::assign) {
                return true;
            }
        }
    }
    return false;
}

std::unique_ptr<switch_statement_t> parser_t::switch_statement() {
    token_t switch_token = m_current_token;
    eat(token_type_e::switch_token);

    auto expr = expression();

    eat(token_type_e::lbrace);

    std::vector<std::unique_ptr<case_statement_t>> cases;
    std::vector<std::unique_ptr<statement_t>> default_statements;
    bool has_default_case = false;

    while (m_current_token.type != token_type_e::rbrace && m_current_token.type != token_type_e::end_of_file) {
        if (m_current_token.type == token_type_e::case_token) {
            cases.push_back(case_statement());
        } else if (m_current_token.type == token_type_e::default_token) {
            if (has_default_case) {
                zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                throw zephyr::syntax_error_t("Duplicate default case in switch statement.");
            }
            has_default_case = true;
            eat(token_type_e::default_token);
            eat(token_type_e::colon);

            if (m_current_token.type == token_type_e::lbrace) {
                // Explicit block syntax: default: { ... }
                auto body_block = block();
                default_statements = std::move(body_block->statements);
            } else {
                // Implicit block syntax: default: statement1 statement2 ...
                while (m_current_token.type != token_type_e::case_token &&
                       m_current_token.type != token_type_e::rbrace &&
                       m_current_token.type != token_type_e::end_of_file) {
                    default_statements.push_back(statement());
                }
            }
        } else {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
            throw zephyr::syntax_error_t("Expected case_token or default_token in switch statement.");
        }
    }

    eat(token_type_e::rbrace);

    return std::make_unique<switch_statement_t>(std::move(expr), std::move(cases),
                                           std::move(default_statements), has_default_case,
                                           switch_token.line, switch_token.column,
                                           m_current_token.line, m_current_token.column);
}

std::unique_ptr<case_statement_t> parser_t::case_statement() {
    token_t case_token = m_current_token;
    eat(token_type_e::case_token);
    auto value = expression();
    eat(token_type_e::colon);

    std::vector<std::unique_ptr<statement_t>> statements;
    if (m_current_token.type == token_type_e::lbrace) {
        // Explicit block syntax: case X: { ... }
        auto body_block = block();
        statements = std::move(body_block->statements);
    } else {
        // Implicit block syntax: case X: statement1 statement2 ...
        while (m_current_token.type != token_type_e::case_token &&
               m_current_token.type != token_type_e::default_token &&
               m_current_token.type != token_type_e::rbrace &&
               m_current_token.type != token_type_e::end_of_file) {
            statements.push_back(statement());
        }
    }
    // Determine end_line and end_column based on the last statement or the colon token
    int end_line = case_token.line;
    int end_column = case_token.column;
    if (!statements.empty()) {
        end_line = statements.back()->end_line;
        end_column = statements.back()->end_column;
    } else {
        // If no statements, use the end of the colon token
        end_line = value->end_line;
        end_column = value->end_column + 1; // Adjust based on actual colon token position
    }

    return std::make_unique<case_statement_t>(std::move(value), std::move(statements),
                                           case_token.line, case_token.column,
                                           end_line, end_column);
}

std::unique_ptr<break_statement_t> parser_t::break_statement() {
    token_t break_token = m_current_token;
    eat(token_type_e::break_token);
    return std::make_unique<break_statement_t>(break_token.line, break_token.column, break_token.line, break_token.column + break_token.text.length() - 1);
}

std::unique_ptr<continue_statement_t> parser_t::continue_statement() {
    token_t continue_token = m_current_token;
    eat(token_type_e::continue_token);
    return std::make_unique<continue_statement_t>(continue_token.line, continue_token.column, continue_token.line, continue_token.column + continue_token.text.length() - 1);
}

std::unique_ptr<try_catch_statement_t> parser_t::try_catch_statement() {
    token_t try_token = m_current_token;
    eat(token_type_e::try_token);

    auto try_block = block();

    eat(token_type_e::catch_token);

    bool has_lparen = false;
    if (m_current_token.type == token_type_e::lparen) {
        eat(token_type_e::lparen);
        has_lparen = true;
    }

    std::string exception_variable_name(m_current_token.text);
    eat(token_type_e::name);

    if (has_lparen) {
        eat(token_type_e::rparen);
    }

    auto catch_block = block();

    return std::make_unique<try_catch_statement_t>(
        std::move(try_block),
        exception_variable_name,
        std::move(catch_block),
        try_token.line,
        try_token.column,
        catch_block->end_line,
        catch_block->end_column
    );
}

std::vector<parameter_t> parser_t::parse_parameter_list() {
    std::vector<parameter_t> parameters;

    if (m_current_token.type == token_type_e::name || m_current_token.type == token_type_e::const_token) {
        // Single parameter case: (const_token)? x (:type)?
        bool is_const = false;
        if (m_current_token.type == token_type_e::const_token) {
            is_const = true;
            eat(token_type_e::const_token); // m_current_token is now the name
        }

        std::string param_name(m_current_token.text);
        eat(token_type_e::name); // Consume the name

        // Check for type annotation
        std::string type_name = "";
        bool has_explicit_type = false;
        if (m_current_token.type == token_type_e::colon) {
            eat(token_type_e::colon);
            type_name = m_current_token.text;
            has_explicit_type = true;
            eat(token_type_e::name);
        }

        parameters.push_back(parameter_t(param_name, is_const, type_name, has_explicit_type));
    } else if (m_current_token.type == token_type_e::lparen) {
        // Multiple parameters or no parameters: () or ( (const_token)? x (:type)?, (const_token)? y (:type)?)
        eat(token_type_e::lparen); // Consumes '('
        if (m_current_token.type != token_type_e::rparen) { // If not ')' immediately
            // Parse first parameter
            bool is_const = false;
            if (m_current_token.type == token_type_e::const_token) {
                is_const = true;
                eat(token_type_e::const_token); // m_current_token is now the name
            }

            std::string param_name(m_current_token.text);
            eat(token_type_e::name); // Consume the name

            // Check for type annotation
            std::string type_name = "";
            bool has_explicit_type = false;
            if (m_current_token.type == token_type_e::colon) {
                eat(token_type_e::colon);
                type_name = m_current_token.text;
                has_explicit_type = true;
                eat(token_type_e::name);
            }

            parameters.push_back(parameter_t(param_name, is_const, type_name, has_explicit_type));

            // Parse remaining parameters
            while (m_current_token.type == token_type_e::comma) {
                eat(token_type_e::comma);
                is_const = false;
                if (m_current_token.type == token_type_e::const_token) {
                    is_const = true;
                    eat(token_type_e::const_token); // m_current_token is now the name
                }

                param_name = m_current_token.text;
                eat(token_type_e::name); // Consume the name

                // Check for type annotation
                type_name = "";
                has_explicit_type = false;
                if (m_current_token.type == token_type_e::colon) {
                    eat(token_type_e::colon);
                    type_name = m_current_token.text;
                    has_explicit_type = true;
                    eat(token_type_e::name);
                }

                parameters.push_back(parameter_t(param_name, is_const, type_name, has_explicit_type));
            }
        }
        eat(token_type_e::rparen);
    } else {
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw zephyr::syntax_error_t("Expected parameter list in lambda expression");
    }
    return parameters;
}

bool parser_t::is_typed_declaration_lookahead() {
    if (m_current_token.type == token_type_e::const_token) {
        // For const declarations: const x : type = value
        token_t next_token = m_lexer.peek_next_token(); // Should be name
        if (next_token.type != token_type_e::name) {
            return false;
        }
        token_t after_name = m_lexer.peek_after(next_token); // Should be colon
        if (after_name.type == token_type_e::colon) {
            token_t after_colon = m_lexer.peek_after(after_name);
            if (after_colon.type == token_type_e::name) {
                token_t after_type = m_lexer.peek_after(after_colon);
                return after_type.type == token_type_e::assign; // Must have assignment
            }
        }
        return false;
    } else if (m_current_token.type == token_type_e::name) {
        // For regular declarations: x : type = value
        token_t next_token = m_lexer.peek_next_token(); // Should be colon
        if (next_token.type == token_type_e::colon) {
            token_t after_colon = m_lexer.peek_after(next_token);
            if (after_colon.type == token_type_e::name) {
                token_t after_type = m_lexer.peek_after(after_colon);
                return after_type.type == token_type_e::assign; // Must have assignment
            }
        }
        return false;
    }
    return false;
}

std::unique_ptr<typed_declaration_t> parser_t::typed_declaration() {
    token_t name_token = m_current_token;
    eat(token_type_e::name);
    eat(token_type_e::colon);
    token_t type_token = m_current_token;
    eat(token_type_e::name);
    eat(token_type_e::assign);
    auto value_expr = expression();
    return std::make_unique<typed_declaration_t>(name_token.text, type_token.text, std::move(value_expr),
                                              name_token.line, name_token.column, value_expr->end_line, value_expr->end_column);
}

std::unique_ptr<typed_const_declaration_t> parser_t::typed_const_declaration() {
    token_t const_token = m_current_token;
    eat(token_type_e::const_token);
    token_t name_token = m_current_token;
    eat(token_type_e::name);
    eat(token_type_e::colon);
    token_t type_token = m_current_token;
    eat(token_type_e::name);
    eat(token_type_e::assign);
    auto value_expr = expression();
    return std::make_unique<typed_const_declaration_t>(name_token.text, type_token.text, std::move(value_expr),
                                                   const_token.line, const_token.column, value_expr->end_line, value_expr->end_column);
}

std::unique_ptr<member_variable_declaration_t> parser_t::member_variable_declaration() {
    bool is_const = false;
    token_t start_token = m_current_token;
    if (m_current_token.type == token_type_e::const_token) {
        is_const = true;
        eat(token_type_e::const_token);
    }

    token_t name_token = m_current_token;
    eat(token_type_e::name);

    std::string type_name = "";
    bool has_explicit_type = false;
    std::unique_ptr<expression_t> value = nullptr;
    bool has_default_value = false;

    // Check for type annotation
    if (m_current_token.type == token_type_e::colon) {
        eat(token_type_e::colon);
        token_t type_token = m_current_token;
        type_name = type_token.text;
        has_explicit_type = true;
        eat(token_type_e::name);
    }

    // Check for default value assignment
    if (m_current_token.type == token_type_e::assign) {
        eat(token_type_e::assign);
        value = expression();
        has_default_value = true;
    }

    return std::make_unique<member_variable_declaration_t>(
        std::string(name_token.text), type_name, std::move(value),
        has_explicit_type, has_default_value, is_const,
        start_token.line, start_token.column,
        has_default_value ? value->end_line : (has_explicit_type ? m_current_token.line : name_token.line),
        has_default_value ? value->end_column : (has_explicit_type ? m_current_token.column : name_token.column + name_token.text.length() - 1)
    );
}

bool parser_t::is_member_variable_declaration_lookahead() {
    if (m_current_token.type == token_type_e::const_token) {
        token_t next = m_lexer.peek_next_token();
        if (next.type == token_type_e::name) {
            return true;
        }
    }
    if (m_current_token.type == token_type_e::name) {
        token_t next_token = m_lexer.peek_next_token();
        // Member variable if followed by colon (typed), assignment (untyped with default),
        // or end of statement markers (untyped, defaults to none)
        return next_token.type == token_type_e::colon ||
               next_token.type == token_type_e::assign ||
               next_token.type == token_type_e::semicolon ||
               next_token.type == token_type_e::rbrace ||
               next_token.type == token_type_e::name ||  // Next member variable
               next_token.type == token_type_e::func ||
               next_token.type == token_type_e::async;
    }
    return false;
}

bool parser_t::is_empty_declaration_lookahead() {
    if (m_current_token.type == token_type_e::name) {
        token_t next_token = m_lexer.peek_next_token();

        if (next_token.type == token_type_e::colon) {
            // x : type (check if there's no assignment after type)
            token_t after_colon = m_lexer.peek_after(next_token);

            if (after_colon.type == token_type_e::name) {
                token_t after_type = m_lexer.peek_after(after_colon);

                // Should not be followed by assignment
                return after_type.type != token_type_e::assign;
            }
        }
    }
    return false;
}

std::unique_ptr<empty_typed_declaration_t> parser_t::empty_typed_declaration() {
    token_t name_token = m_current_token;
    eat(token_type_e::name);
    eat(token_type_e::colon);
    token_t type_token = m_current_token;
    eat(token_type_e::name);
    return std::make_unique<empty_typed_declaration_t>(name_token.text, type_token.text,
                                                   name_token.line, name_token.column, type_token.line, type_token.column + type_token.text.length() - 1);
}

std::unique_ptr<empty_declaration_t> parser_t::empty_declaration() {
    token_t name_token = m_current_token;
    eat(token_type_e::name);
    return std::make_unique<empty_declaration_t>(name_token.text,
                                              name_token.line, name_token.column, name_token.line, name_token.column + name_token.text.length() - 1);
}

std::unique_ptr<lambda_expression_t> parser_t::lambda_function() {
    token_t start_token = m_current_token;
    bool async = false;

    if (m_current_token.type == token_type_e::async) {
        async = true;
        eat(token_type_e::async);
    }

    std::vector<parameter_t> parameters = parse_parameter_list();

    std::string return_type_name = "";
    bool explicit_return_type = false;
    if (m_current_token.type == token_type_e::colon) {
        eat(token_type_e::colon);
        return_type_name = m_current_token.text;
        explicit_return_type = true;
        eat(token_type_e::name);
    }

    eat(token_type_e::arrow);

    if (m_current_token.type == token_type_e::lbrace) {
        auto body_block = block();
        return std::make_unique<lambda_expression_t>(
            std::move(parameters),
            std::move(body_block),
            return_type_name, explicit_return_type, async,
            start_token.line,
            start_token.column,
            body_block->end_line,
            body_block->end_column
        );
    } else {
        auto body_expr = expression();
        return std::make_unique<lambda_expression_t>(
            std::move(parameters),
            std::move(body_expr),
            return_type_name, explicit_return_type, async,
            start_token.line,
            start_token.column,
            body_expr->end_line,
            body_expr->end_column
        );
    }
}

std::unique_ptr<lambda_expression_t> parser_t::lambda_expression() {
    return lambda_function();
}

std::unique_ptr<class_definition_t> parser_t::class_definition() {
    token_t class_token = m_current_token;
    
    // Check for class modifiers (final, abstract)
    bool is_final = false;
    bool is_abstract = false;
    
    if (m_current_token.type == token_type_e::final_token) {
        is_final = true;
        eat(token_type_e::final_token);
    } else if (m_current_token.type == token_type_e::abstract_token) {
        is_abstract = true;
        eat(token_type_e::abstract_token);
    }
    
    eat(token_type_e::class_token);

    std::string class_name(m_current_token.text);
    eat(token_type_e::name);

    // Parse parent class and/or interfaces after colon
    std::string parent_class;
    std::vector<std::string> interfaces;
    
    if (m_current_token.type == token_type_e::colon) {
        eat(token_type_e::colon);
        
        // Collect all inherited items
        std::vector<std::string> inherited_items;
        inherited_items.push_back(std::string(m_current_token.text));
        eat(token_type_e::name);
        
        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            inherited_items.push_back(std::string(m_current_token.text));
            eat(token_type_e::name);
        }
        
        // Now we need to determine which are classes and which are interfaces
        // This will be checked at interpretation time when we can look up the symbols
        // For now, store all items and let the interpreter validate
        if (inherited_items.size() == 1) {
            // Single inheritance - could be class or interface
            parent_class = inherited_items[0];
        } else {
            // Multiple inheritance - need to check at runtime
            // Store first as potential parent, rest as potential interfaces
            parent_class = inherited_items[0];
            for (size_t i = 1; i < inherited_items.size(); ++i) {
                interfaces.push_back(inherited_items[i]);
            }
        }
    }

    eat(token_type_e::lbrace);

    // Parse member variable declarations first
    std::vector<std::unique_ptr<member_variable_declaration_t>> member_variables;
    while (m_current_token.type == token_type_e::name || m_current_token.type == token_type_e::const_token) {
        // Check if this is a method definition
        if (m_current_token.type == token_type_e::func || m_current_token.type == token_type_e::async) {
            break;
        }

        // Check if current token is "func" or "async" keywords (should not happen with proper lexer, but safety check)
        if (m_current_token.text == "func" || m_current_token.text == "async") {
            break;
        }

        // Use the lookahead function to determine if this is a member variable
        if (!is_member_variable_declaration_lookahead()) {
            break;
        }

        // Parse as member variable declaration
        auto member_var = member_variable_declaration();
        member_variables.push_back(std::unique_ptr<member_variable_declaration_t>(static_cast<member_variable_declaration_t*>(member_var.release())));
    }

    // Parse method definitions
    std::vector<std::unique_ptr<function_definition_t>> methods;
    while (m_current_token.type == token_type_e::func || 
           m_current_token.type == token_type_e::async ||
           m_current_token.type == token_type_e::abstract_token) {
        auto method = function_definition();
        methods.push_back(std::unique_ptr<function_definition_t>(static_cast<function_definition_t*>(method.release())));
    }

    token_t end_token = m_current_token;
    eat(token_type_e::rbrace);

    return std::make_unique<class_definition_t>(
        class_name,
        parent_class,
        std::move(interfaces),
        std::move(member_variables),
        std::move(methods),
        false,
        is_final,
        is_abstract,
        class_token.line,
        class_token.column,
        end_token.line,
        end_token.column
    );
}

std::unique_ptr<interface_definition_t> parser_t::interface_definition() {
    token_t interface_token = m_current_token;
    eat(token_type_e::interface_token);

    std::string interface_name(m_current_token.text);
    eat(token_type_e::name);

    eat(token_type_e::lbrace);

    std::vector<function_signature_t> methods;
    while (m_current_token.type == token_type_e::func) {
        methods.push_back(function_signature());
    }

    token_t end_token = m_current_token;
    eat(token_type_e::rbrace);

    return std::make_unique<interface_definition_t>(
        interface_name,
        std::move(methods),
        interface_token.line,
        interface_token.column,
        end_token.line,
        end_token.column
    );
}

function_signature_t parser_t::function_signature() {
    eat(token_type_e::func);
    token_t name_token = m_current_token;
    eat(token_type_e::name);

    eat(token_type_e::lparen);
    std::vector<parameter_t> parameters;
    if (m_current_token.type != token_type_e::rparen) {
        bool is_const = false;
        if (m_current_token.type == token_type_e::const_token) {
            is_const = true;
            eat(token_type_e::const_token);
        }
        std::string param_name(m_current_token.text);
        eat(token_type_e::name);
        std::string type_name = "";
        bool has_explicit_type = false;
        if (m_current_token.type == token_type_e::colon) {
            eat(token_type_e::colon);
            type_name = m_current_token.text;
            has_explicit_type = true;
            eat(token_type_e::name);
        }
        parameters.push_back({param_name, is_const, type_name, has_explicit_type});

        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            is_const = false;
            if (m_current_token.type == token_type_e::const_token) {
                is_const = true;
                eat(token_type_e::const_token);
            }
            param_name = m_current_token.text;
            eat(token_type_e::name);
            type_name = "";
            has_explicit_type = false;
            if (m_current_token.type == token_type_e::colon) {
                eat(token_type_e::colon);
                type_name = m_current_token.text;
                has_explicit_type = true;
                eat(token_type_e::name);
            }
            parameters.push_back({param_name, is_const, type_name, has_explicit_type});
        }
    }
    eat(token_type_e::rparen);

    std::string return_type_name = "";
    bool has_return_type = false;
    if (m_current_token.type == token_type_e::colon) {
        eat(token_type_e::colon);
        return_type_name = m_current_token.text;
        has_return_type = true;
        eat(token_type_e::name);
    }

    return function_signature_t{
        std::string(name_token.text),
        std::move(parameters),
        return_type_name,
        has_return_type
    };
}

std::unique_ptr<method_call_t> parser_t::method_call(std::unique_ptr<expression_t> object, std::string method_name, token_t method_token) {
    eat(token_type_e::lparen);

    std::vector<std::unique_ptr<expression_t>> arguments;
    if (m_current_token.type != token_type_e::rparen) {
        arguments.push_back(expression());
        while (m_current_token.type == token_type_e::comma) {
            eat(token_type_e::comma);
            arguments.push_back(expression());
        }
    }

    token_t end_token = m_current_token;
    eat(token_type_e::rparen);

    return std::make_unique<method_call_t>(
        std::move(object),
        method_name,
        std::move(arguments),
        method_token.line,
        method_token.column,
        end_token.end_line,
        end_token.end_column
    );
}

std::unique_ptr<member_access_t> parser_t::member_access(std::unique_ptr<expression_t> object, std::string member_name, token_t member_token) {
    return std::make_unique<member_access_t>(
        std::move(object),
        member_name,
        member_token.line,
        member_token.column,
        member_token.end_line,
        member_token.end_column
    );
}

std::unique_ptr<member_assignment_t> parser_t::member_assignment() {
    token_t start_token = m_current_token;
    std::unique_ptr<expression_t> object;

    if (m_current_token.type == token_type_e::this_token) {
        eat(token_type_e::this_token);
        object = std::make_unique<this_expression_t>(start_token.line, start_token.column, start_token.end_line, start_token.end_column);
    } else if (m_current_token.type == token_type_e::super_token) {
        eat(token_type_e::super_token);
        object = std::make_unique<super_expression_t>(start_token.line, start_token.column, start_token.end_line, start_token.end_column);
    } else {
        std::string name(m_current_token.text);
        eat(token_type_e::name);
        object = std::make_unique<name_t>(name, start_token.line, start_token.column, start_token.end_line, start_token.end_column);
    }

    eat(token_type_e::dot);

    std::string member_name(m_current_token.text);
    token_t member_token = m_current_token;
    eat(token_type_e::name);

    eat(token_type_e::assign);

    auto value = expression();

    return std::make_unique<member_assignment_t>(
        std::move(object),
        member_name,
        std::move(value),
        start_token.line,
        start_token.column,
        value->end_line,
        value->end_column
    );
}

auto parser_t::import_statement() -> std::unique_ptr<import_statement_t>
{
    token_t import_token = m_current_token;
    eat(token_type_e::import_token);
    

    // Check for string import: import "./lib/math.zephyr" [as alias]
    if (m_current_token.type == token_type_e::string)
    {
        std::string path = std::string(m_current_token.text);
        // Remove quotes from string literal
        if (path.length() >= 2 && path.front() == '"' && path.back() == '"')
        {
            path = path.substr(1, path.length() - 2);
        }
        eat(token_type_e::string);
        
        std::string alias_name;
        if (m_current_token.type == token_type_e::as_token)
        {
            eat(token_type_e::as_token);
            if (m_current_token.type != token_type_e::name)
            {
                zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                throw syntax_error_t("Expected alias name after 'as'");
            }
            alias_name = std::string(m_current_token.text);
            eat(token_type_e::name);
        }
        
        return std::make_unique<import_statement_t>(
            import_statement_t::import_type_e::string_import,
            std::vector<std::string>(),  // No specific symbols
            std::move(path),
            std::move(alias_name),
            true,  // is_path_based
            import_token.line, import_token.column, m_current_token.line, m_current_token.column
        );
    }
    
    // Check for named import with 'from': import add, PI from math [as alias]
    if (m_current_token.type == token_type_e::name)
    {
        // Look ahead to see if there's a 'from' keyword
        std::vector<std::string> temp_symbols;
        
        // Parse potential symbol list
        temp_symbols.push_back(std::string(m_current_token.text));
        auto temp_token = m_current_token;
        eat(token_type_e::name);
        
        // Check for comma-separated list
        while (m_current_token.type == token_type_e::comma)
        {
            eat(token_type_e::comma);
            if (m_current_token.type != token_type_e::name)
            {
                zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                throw syntax_error_t("Expected symbol name after comma");
            }
            temp_symbols.push_back(std::string(m_current_token.text));
            eat(token_type_e::name);
        }
        
        // Check if 'from' follows
        if (m_current_token.type == token_type_e::from_token)
        {
            eat(token_type_e::from_token);
            
            // Parse module specifier
            auto [module_specifier, is_path_based] = parse_import_specifier();
            
            // Check for alias after module specifier
            std::string alias_name;
            if (m_current_token.type == token_type_e::as_token)
            {
                eat(token_type_e::as_token);
                if (m_current_token.type != token_type_e::name)
                {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw syntax_error_t("Expected alias name after 'as'");
                }
                alias_name = std::string(m_current_token.text);
                eat(token_type_e::name);
            }
            
            return std::make_unique<import_statement_t>(
                import_statement_t::import_type_e::named_import,
                std::move(temp_symbols),
                std::move(module_specifier),
                std::move(alias_name),
                is_path_based,
                import_token.line, import_token.column, m_current_token.line, m_current_token.column
            );
        }
        else
        {
            // It's a lazy import: import math [as alias] or import math.advanced [as alias]
            if (temp_symbols.size() > 1)
            {
                zephyr::current_error_location() = {temp_token.line, temp_token.column, 1};
                throw syntax_error_t("Lazy import can only specify one module name");
            }
            
            // Parse dotted module name: math.advanced
            std::string module_specifier = temp_symbols[0];
            while (m_current_token.type == token_type_e::dot)
            {
                eat(token_type_e::dot);
                if (m_current_token.type != token_type_e::name)
                {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw syntax_error_t("Expected module name after '.' in import specifier");
                }
                module_specifier += "." + std::string(m_current_token.text);
                eat(token_type_e::name);
            }
            
            std::string alias_name;
            
            // Check for 'as' clause
            if (m_current_token.type == token_type_e::as_token)
            {
                eat(token_type_e::as_token);
                if (m_current_token.type != token_type_e::name)
                {
                    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                    throw syntax_error_t("Expected alias name after 'as'");
                }
                alias_name = std::string(m_current_token.text);
                eat(token_type_e::name);
            }
            
            return std::make_unique<import_statement_t>(
                import_statement_t::import_type_e::lazy_import,
                std::vector<std::string>(),  // No specific symbols
                std::move(module_specifier),
                std::move(alias_name),
                false,  // not path_based
                import_token.line, import_token.column, m_current_token.line, m_current_token.column
            );
        }
    }
    
    zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
    throw syntax_error_t("Invalid import statement syntax");
}

auto parser_t::parse_imported_symbols() -> std::vector<std::string>
{
    std::vector<std::string> symbols;
    
    if (m_current_token.type != token_type_e::name)
    {
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw syntax_error_t("Expected symbol name in import statement");
    }
    
    symbols.push_back(std::string(m_current_token.text));
    eat(token_type_e::name);
    
    // Parse additional symbols separated by commas
    while (m_current_token.type == token_type_e::comma)
    {
        eat(token_type_e::comma);
        
        if (m_current_token.type != token_type_e::name)
        {
            zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
            throw syntax_error_t("Expected symbol name after comma in import statement");
        }
        
        symbols.push_back(std::string(m_current_token.text));
        eat(token_type_e::name);
    }
    
    return symbols;
}

auto parser_t::parse_import_specifier() -> std::pair<std::string, bool>
{
    if (m_current_token.type == token_type_e::string)
    {
        // Path-based import: from "./math.zephyr"
        std::string specifier = std::string(m_current_token.text);
        // Remove quotes from string literal
        if (specifier.length() >= 2 && specifier.front() == '"' && specifier.back() == '"')
        {
            specifier = specifier.substr(1, specifier.length() - 2);
        }
        eat(token_type_e::string);
        return {specifier, true};
    }
    else if (m_current_token.type == token_type_e::name)
    {
        // Name-based import: from math or from math.advanced
        std::string specifier = std::string(m_current_token.text);
        eat(token_type_e::name);
        
        // Handle dotted module names: math.advanced
        while (m_current_token.type == token_type_e::dot)
        {
            eat(token_type_e::dot);
            if (m_current_token.type != token_type_e::name)
            {
                zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
                throw syntax_error_t("Expected module name after '.' in import specifier");
            }
            specifier += "." + std::string(m_current_token.text);
            eat(token_type_e::name);
        }
        
        return {specifier, false};
    }
    else
    {
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw syntax_error_t("Expected module name or path in import statement");
    }
}

auto parser_t::internal_declaration() -> std::unique_ptr<statement_t>
{
    eat(token_type_e::internal_token);
    
    // Parse the declaration that follows 'internal'
    if (m_current_token.type == token_type_e::func)
    {
        // Internal function
        auto func_def = function_definition();
        func_def->is_internal = true;
        return func_def;
    }
    else if (m_current_token.type == token_type_e::async)
    {
        // Internal async function
        auto func_def = function_definition();
        func_def->is_internal = true;
        return func_def;
    }
    else if (m_current_token.type == token_type_e::class_token)
    {
        // Internal class
        auto class_def = class_definition();
        class_def->is_internal = true;
        return class_def;
    }
    else if (m_current_token.type == token_type_e::final_token || 
             m_current_token.type == token_type_e::abstract_token)
    {
        // Internal class with modifiers
        auto class_def = class_definition();
        class_def->is_internal = true;
        return class_def;
    }
    else if (m_current_token.type == token_type_e::const_token)
    {
        // Internal const declaration
        auto const_def = const_declaration();
        const_def->is_internal = true;
        return const_def;
    }
    else
    {
        zephyr::current_error_location() = {m_current_token.line, m_current_token.column, 1};
        throw syntax_error_t("'internal' can only be used with func, async, class, or const declarations");
    }
}

auto parser_t::is_import_statement_lookahead() -> bool
{
    return m_current_token.type == token_type_e::import_token;
}

auto parser_t::is_internal_declaration_lookahead() -> bool
{
    return m_current_token.type == token_type_e::internal_token;
}

auto parser_t::process_escape_sequences(const std::string& str) -> std::string
{
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '\\' && i + 1 < str.length())
        {
            char next = str[i + 1];
            switch (next)
            {
                case 'n':
                    result += '\n';
                    i++; // Skip the escaped character
                    break;
                case 't':
                    result += '\t';
                    i++;
                    break;
                case 'r':
                    result += '\r';
                    i++;
                    break;
                case '\\':
                    result += '\\';
                    i++;
                    break;
                case '"':
                    result += '"';
                    i++;
                    break;
                case '\'':
                    result += '\'';
                    i++;
                    break;
                case 'b':
                    result += '\b';
                    i++;
                    break;
                case 'f':
                    result += '\f';
                    i++;
                    break;
                case 'v':
                    result += '\v';
                    i++;
                    break;
                case '0':
                    result += '\0';
                    i++;
                    break;
                case 'x':
                    // Hex escape sequence \xHH
                    if (i + 3 < str.length() && std::isxdigit(str[i + 2]) && std::isxdigit(str[i + 3]))
                    {
                        std::string hex_str = str.substr(i + 2, 2);
                        char hex_char = static_cast<char>(std::stoi(hex_str, nullptr, 16));
                        result += hex_char;
                        i += 3; // Skip \xHH
                    }
                    else
                    {
                        // Invalid hex escape, keep as-is
                        result += '\\';
                        result += 'x';
                        i++;
                    }
                    break;
                case 'u':
                    // Unicode escape sequence \uHHHH (4 hex digits)
                    if (i + 5 < str.length())
                    {
                        bool valid = true;
                        for (size_t j = 2; j <= 5; ++j)
                        {
                            if (!std::isxdigit(str[i + j]))
                            {
                                valid = false;
                                break;
                            }
                        }
                        if (valid)
                        {
                            std::string hex_str = str.substr(i + 2, 4);
                            unsigned int code_point = std::stoi(hex_str, nullptr, 16);
                            
                            // Convert Unicode code point to UTF-8
                            if (code_point <= 0x7F)
                            {
                                result += static_cast<char>(code_point);
                            }
                            else if (code_point <= 0x7FF)
                            {
                                result += static_cast<char>(0xC0 | (code_point >> 6));
                                result += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else
                            {
                                result += static_cast<char>(0xE0 | (code_point >> 12));
                                result += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                                result += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            i += 5; // Skip \uHHHH
                        }
                        else
                        {
                            // Invalid unicode escape, keep as-is
                            result += '\\';
                            result += 'u';
                            i++;
                        }
                    }
                    else
                    {
                        result += '\\';
                        result += 'u';
                        i++;
                    }
                    break;
                case 'U':
                    // Unicode escape sequence \UHHHHHHHH (8 hex digits)
                    if (i + 9 < str.length())
                    {
                        bool valid = true;
                        for (size_t j = 2; j <= 9; ++j)
                        {
                            if (!std::isxdigit(str[i + j]))
                            {
                                valid = false;
                                break;
                            }
                        }
                        if (valid)
                        {
                            std::string hex_str = str.substr(i + 2, 8);
                            unsigned int code_point = std::stoul(hex_str, nullptr, 16);
                            
                            // Convert Unicode code point to UTF-8
                            if (code_point <= 0x7F)
                            {
                                result += static_cast<char>(code_point);
                            }
                            else if (code_point <= 0x7FF)
                            {
                                result += static_cast<char>(0xC0 | (code_point >> 6));
                                result += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else if (code_point <= 0xFFFF)
                            {
                                result += static_cast<char>(0xE0 | (code_point >> 12));
                                result += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                                result += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else if (code_point <= 0x10FFFF)
                            {
                                result += static_cast<char>(0xF0 | (code_point >> 18));
                                result += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
                                result += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                                result += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else
                            {
                                // Invalid Unicode code point, keep as-is
                                result += '\\';
                                result += 'U';
                                i++;
                            }
                            i += 9; // Skip \UHHHHHHHH
                        }
                        else
                        {
                            // Invalid unicode escape, keep as-is
                            result += '\\';
                            result += 'U';
                            i++;
                        }
                    }
                    else
                    {
                        result += '\\';
                        result += 'U';
                        i++;
                    }
                    break;
                default:
                    // Unknown escape sequence, keep the backslash and character as-is
                    result += '\\';
                    result += next;
                    i++;
                    break;
            }
        }
        else
        {
            result += str[i];
        }
    }
    
    return result;
}

}
