#include "ast.hpp"
#include "interpreter.hpp"

namespace zephyr
{

auto number_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto float_literal_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto string_literal_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto fstring_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto boolean_literal_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto none_literal_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto list_literal_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto dictionary_literal_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto name_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto binary_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto comparison_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto logical_and_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto logical_or_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto nullish_coalescing_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto pipe_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto logical_not_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto unary_op_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto index_access_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto const_declaration_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto typed_declaration_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto typed_const_declaration_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto empty_declaration_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto empty_typed_declaration_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto member_variable_declaration_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto compound_assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto increment_decrement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto compound_member_assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto compound_indexed_assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto increment_decrement_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto member_increment_decrement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto indexed_increment_decrement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto indexed_assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto expression_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto block_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto if_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto while_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto do_while_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto do_until_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto for_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto for_each_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto loop_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto function_definition_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto function_call_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto return_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto break_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto continue_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto try_catch_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto program_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto list_destructuring_assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto switch_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto case_statement_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto ternary_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto lambda_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto class_definition_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto method_call_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto member_access_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto this_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto member_assignment_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto in_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto await_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto spawn_expression_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto optional_member_access_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto optional_method_call_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

auto optional_index_access_t::accept(ast_visitor_t& visitor) -> void 
{
    visitor.visit(*this);
}

} // namespace zephyr