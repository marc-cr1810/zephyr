#include "zephyr/ast.hpp"
#include "zephyr/interpreter.hpp"

namespace zephyr
{

auto number_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto number_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<number_t>(value, line, column, end_line, end_column);
}

auto sized_number_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto sized_number_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<sized_number_t>(value, suffix, line, column, end_line, end_column);
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

auto power_op_t::accept(ast_visitor_t& visitor) -> void
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

auto slice_expression_t::accept(ast_visitor_t& visitor) -> void
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

auto with_statement_t::accept(ast_visitor_t& visitor) -> void
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

auto super_expression_t::accept(ast_visitor_t& visitor) -> void
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

auto interface_definition_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto bitwise_and_op_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto bitwise_or_op_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto bitwise_xor_op_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto bitwise_not_op_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto left_shift_op_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto right_shift_op_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}



auto float_literal_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<float_literal_t>(*this);
}

auto string_literal_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<string_literal_t>(*this);
}

auto fstring_t::clone() const -> std::unique_ptr<ast_node_t>
{
    std::vector<std::unique_ptr<expression_t>> new_parts;
    for(const auto& part : parts)
    {
        new_parts.push_back(part ? std::unique_ptr<expression_t>(static_cast<expression_t*>(part->clone().release())) : nullptr);
    }
    return std::make_unique<fstring_t>(std::move(new_parts), line, column, end_line, end_column);
}

auto boolean_literal_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<boolean_literal_t>(*this);
}

auto none_literal_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<none_literal_t>(*this);
}

auto list_literal_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_list = std::make_unique<list_literal_t>(line, column, end_line, end_column);
    for (const auto& element : elements)
    {
        new_list->add_element(element ? std::unique_ptr<expression_t>(static_cast<expression_t*>(element->clone().release())) : nullptr);
    }
    return new_list;
}

auto dictionary_literal_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_dict = std::make_unique<dictionary_literal_t>(line, column, end_line, end_column);
    for (const auto& pair : key_value_pairs)
    {
        auto key_clone = pair.first ? std::unique_ptr<expression_t>(static_cast<expression_t*>(pair.first->clone().release())) : nullptr;
        auto value_clone = pair.second ? std::unique_ptr<expression_t>(static_cast<expression_t*>(pair.second->clone().release())) : nullptr;
        new_dict->add_element(std::move(key_clone), std::move(value_clone));
    }
    return new_dict;
}

auto name_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<name_t>(*this);
}

auto binary_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<binary_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        operator_token, line, column, end_line, end_column);
}

auto power_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<power_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto comparison_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<comparison_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        operator_token, line, column, end_line, end_column);
}

auto logical_and_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<logical_and_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto logical_or_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<logical_or_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto nullish_coalescing_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<nullish_coalescing_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto pipe_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<pipe_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto logical_not_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<logical_not_op_t>(
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto unary_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<unary_op_t>(
        operator_token,
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto index_access_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<index_access_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        index ? std::unique_ptr<expression_t>(static_cast<expression_t*>(index->clone().release())) : nullptr,
        line, column, end_line, end_column
    );
}

auto slice_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<slice_expression_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        start ? std::unique_ptr<expression_t>(static_cast<expression_t*>(start->clone().release())) : nullptr,
        end ? std::unique_ptr<expression_t>(static_cast<expression_t*>(end->clone().release())) : nullptr,
        step ? std::unique_ptr<expression_t>(static_cast<expression_t*>(step->clone().release())) : nullptr,
        line, column, end_line, end_column
    );
}

auto optional_index_access_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<optional_index_access_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        index ? std::unique_ptr<expression_t>(static_cast<expression_t*>(index->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto member_access_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<member_access_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        member_name, line, column, end_line, end_column);
}

auto optional_member_access_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<optional_member_access_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        member_name, line, column, end_line, end_column);
}

auto method_call_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_args = std::vector<std::unique_ptr<expression_t>>();
    for(const auto& arg : arguments)
    {
        new_args.push_back(arg ? std::unique_ptr<expression_t>(static_cast<expression_t*>(arg->clone().release())) : nullptr);
    }
    return std::make_unique<method_call_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        method_name, std::move(new_args), line, column, end_line, end_column);
}

auto optional_method_call_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_args = std::vector<std::unique_ptr<expression_t>>();
    for(const auto& arg : arguments)
    {
        new_args.push_back(arg ? std::unique_ptr<expression_t>(static_cast<expression_t*>(arg->clone().release())) : nullptr);
    }
    return std::make_unique<optional_method_call_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        method_name, std::move(new_args), line, column, end_line, end_column);
}

auto function_call_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_args = std::vector<std::unique_ptr<expression_t>>();
    for(const auto& arg : arguments)
    {
        new_args.push_back(arg ? std::unique_ptr<expression_t>(static_cast<expression_t*>(arg->clone().release())) : nullptr);
    }
    return std::make_unique<function_call_t>(function_name, std::move(new_args), line, column, end_line, end_column);
}

auto this_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<this_expression_t>(*this);
}

auto super_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<super_expression_t>(*this);
}

auto ternary_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<ternary_expression_t>(
        condition ? std::unique_ptr<expression_t>(static_cast<expression_t*>(condition->clone().release())) : nullptr,
        true_expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(true_expression->clone().release())) : nullptr,
        false_expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(false_expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto await_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<await_expression_t>(
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto spawn_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<spawn_expression_t>(
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto in_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<in_expression_t>(
        element ? std::unique_ptr<expression_t>(static_cast<expression_t*>(element->clone().release())) : nullptr,
        container_expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(container_expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto bitwise_and_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<bitwise_and_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto bitwise_or_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<bitwise_or_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto bitwise_xor_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<bitwise_xor_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto bitwise_not_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<bitwise_not_op_t>(
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto left_shift_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<left_shift_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto right_shift_op_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<right_shift_op_t>(
        left ? std::unique_ptr<expression_t>(static_cast<expression_t*>(left->clone().release())) : nullptr,
        right ? std::unique_ptr<expression_t>(static_cast<expression_t*>(right->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<assignment_t>(
        variable_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto member_assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<member_assignment_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        member_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto indexed_assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<indexed_assignment_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        index ? std::unique_ptr<expression_t>(static_cast<expression_t*>(index->clone().release())) : nullptr,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto const_declaration_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<const_declaration_t>(
        variable_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        is_internal,
        line, column, end_line, end_column);
}

auto typed_declaration_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<typed_declaration_t>(
        variable_name,
        type_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto typed_const_declaration_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<typed_const_declaration_t>(
        variable_name,
        type_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto empty_declaration_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<empty_declaration_t>(*this);
}

auto empty_typed_declaration_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<empty_typed_declaration_t>(*this);
}

auto member_variable_declaration_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<member_variable_declaration_t>(
        variable_name,
        type_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        has_explicit_type,
        has_default_value,
        is_const,
        line, column, end_line, end_column);
}

auto compound_assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<compound_assignment_t>(
        variable_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        operator_token,
        line, column, end_line, end_column);
}

auto compound_member_assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<compound_member_assignment_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        member_name,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        operator_token,
        line, column, end_line, end_column);
}

auto compound_indexed_assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<compound_indexed_assignment_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        index ? std::unique_ptr<expression_t>(static_cast<expression_t*>(index->clone().release())) : nullptr,
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        operator_token,
        line, column, end_line, end_column);
}

auto increment_decrement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<increment_decrement_t>(*this);
}

auto increment_decrement_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<increment_decrement_expression_t>(*this);
}

auto member_increment_decrement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<member_increment_decrement_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        member_name, is_increment, is_prefix, line, column, end_line, end_column);
}

auto indexed_increment_decrement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<indexed_increment_decrement_t>(
        object ? std::unique_ptr<expression_t>(static_cast<expression_t*>(object->clone().release())) : nullptr,
        index ? std::unique_ptr<expression_t>(static_cast<expression_t*>(index->clone().release())) : nullptr,
        is_increment, is_prefix, line, column, end_line, end_column);
}

auto expression_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<expression_statement_t>(
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto block_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_block = std::make_unique<block_t>(line, column, end_line, end_column);
    for (const auto& stmt : statements)
    {
        new_block->add_statement(stmt ? std::unique_ptr<statement_t>(static_cast<statement_t*>(stmt->clone().release())) : nullptr);
    }
    return new_block;
}

auto if_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<if_statement_t>(
        condition ? std::unique_ptr<expression_t>(static_cast<expression_t*>(condition->clone().release())) : nullptr,
        then_block ? std::unique_ptr<block_t>(static_cast<block_t*>(then_block->clone().release())) : nullptr,
        else_block ? std::unique_ptr<block_t>(static_cast<block_t*>(else_block->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto while_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<while_statement_t>(
        condition ? std::unique_ptr<expression_t>(static_cast<expression_t*>(condition->clone().release())) : nullptr,
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto do_while_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<do_while_statement_t>(
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        condition ? std::unique_ptr<expression_t>(static_cast<expression_t*>(condition->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto do_until_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<do_until_statement_t>(
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        condition ? std::unique_ptr<expression_t>(static_cast<expression_t*>(condition->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto for_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<for_statement_t>(
        initialization ? std::unique_ptr<statement_t>(static_cast<statement_t*>(initialization->clone().release())) : nullptr,
        condition ? std::unique_ptr<expression_t>(static_cast<expression_t*>(condition->clone().release())) : nullptr,
        increment ? std::unique_ptr<statement_t>(static_cast<statement_t*>(increment->clone().release())) : nullptr,
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto for_each_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<for_each_statement_t>(
        variables,
        collection ? std::unique_ptr<expression_t>(static_cast<expression_t*>(collection->clone().release())) : nullptr,
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        where_clause ? std::unique_ptr<expression_t>(static_cast<expression_t*>(where_clause->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto loop_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<loop_statement_t>(
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto break_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<break_statement_t>(*this);
}

auto continue_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<continue_statement_t>(*this);
}

auto return_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<return_statement_t>(
        return_value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(return_value->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto try_catch_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<try_catch_statement_t>(
        try_block ? std::unique_ptr<block_t>(static_cast<block_t*>(try_block->clone().release())) : nullptr,
        exception_variable_name,
        catch_block ? std::unique_ptr<block_t>(static_cast<block_t*>(catch_block->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto list_destructuring_assignment_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<list_destructuring_assignment_t>(
        variable_names,
        assigned_expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(assigned_expression->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto case_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_stmts = std::vector<std::unique_ptr<statement_t>>();
    for(const auto& stmt : statements)
    {
        new_stmts.push_back(stmt ? std::unique_ptr<statement_t>(static_cast<statement_t*>(stmt->clone().release())) : nullptr);
    }
    return std::make_unique<case_statement_t>(
        value ? std::unique_ptr<expression_t>(static_cast<expression_t*>(value->clone().release())) : nullptr,
        std::move(new_stmts),
        line, column, end_line, end_column);
}

auto switch_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_cases = std::vector<std::unique_ptr<case_statement_t>>();
    for(const auto& c : cases)
    {
        new_cases.push_back(c ? std::unique_ptr<case_statement_t>(static_cast<case_statement_t*>(c->clone().release())) : nullptr);
    }
    auto new_defaults = std::vector<std::unique_ptr<statement_t>>();
    for(const auto& s : default_statements)
    {
        new_defaults.push_back(s ? std::unique_ptr<statement_t>(static_cast<statement_t*>(s->clone().release())) : nullptr);
    }
    return std::make_unique<switch_statement_t>(
        expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(expression->clone().release())) : nullptr,
        std::move(new_cases),
        std::move(new_defaults),
        has_default_case,
        line, column, end_line, end_column);
}

auto function_definition_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<function_definition_t>(
        function_name,
        parameters,
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        return_type_name,
        explicit_return_type,
        async,
        is_internal,
        is_abstract,
        line, column, end_line, end_column);
}

auto with_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<with_statement_t>(
        context_expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(context_expression->clone().release())) : nullptr,
        variable_name,
        body ? std::unique_ptr<block_t>(static_cast<block_t*>(body->clone().release())) : nullptr,
        line, column, end_line, end_column);
}

auto lambda_expression_t::clone() const -> std::unique_ptr<ast_node_t>
{
    if (is_block_body)
    {
        return std::make_unique<lambda_expression_t>(
            parameters,
            body_block ? std::unique_ptr<block_t>(static_cast<block_t*>(body_block->clone().release())) : nullptr,
            return_type_name,
            explicit_return_type,
            async,
            line, column, end_line, end_column);
    }
    else
    {
        return std::make_unique<lambda_expression_t>(
            parameters,
            body_expression ? std::unique_ptr<expression_t>(static_cast<expression_t*>(body_expression->clone().release())) : nullptr,
            return_type_name,
            explicit_return_type,
            async,
            line, column, end_line, end_column);
    }
}

auto interface_definition_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<interface_definition_t>(interface_name, methods, line, column, end_line, end_column);
}

auto class_definition_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_members = std::vector<std::unique_ptr<member_variable_declaration_t>>();
    for(const auto& m : member_variables)
    {
        new_members.push_back(m ? std::unique_ptr<member_variable_declaration_t>(static_cast<member_variable_declaration_t*>(m->clone().release())) : nullptr);
    }
    auto new_methods = std::vector<std::unique_ptr<function_definition_t>>();
    for(const auto& m : methods)
    {
        new_methods.push_back(m ? std::unique_ptr<function_definition_t>(static_cast<function_definition_t*>(m->clone().release())) : nullptr);
    }
    return std::make_unique<class_definition_t>(class_name, parent_class, interfaces, std::move(new_members), std::move(new_methods), is_internal, is_final, is_abstract, line, column, end_line, end_column);
}

auto program_t::clone() const -> std::unique_ptr<ast_node_t>
{
    auto new_program = std::make_unique<program_t>(line, column, end_line, end_column);
    for (const auto& stmt : statements)
    {
        new_program->add_statement(stmt ? std::unique_ptr<statement_t>(static_cast<statement_t*>(stmt->clone().release())) : nullptr);
    }
    return new_program;
}

import_statement_t::import_statement_t(import_type_e import_type,
                                      std::vector<std::string> imported_symbols, 
                                      std::string module_specifier,
                                      std::string alias_name,
                                      bool is_path_based,
                                      int line, int column, int end_line, int end_column)
    : statement_t(line, column, end_line, end_column), 
      m_import_type(import_type),
      m_imported_symbols(std::move(imported_symbols)), 
      m_module_specifier(std::move(module_specifier)),
      m_alias_name(std::move(alias_name)), 
      m_is_path_based(is_path_based)
{
}

auto import_statement_t::accept(ast_visitor_t& visitor) -> void
{
    visitor.visit(*this);
}

auto import_statement_t::clone() const -> std::unique_ptr<ast_node_t>
{
    return std::make_unique<import_statement_t>(m_import_type, m_imported_symbols, m_module_specifier, m_alias_name, m_is_path_based, line, column, end_line, end_column);
}

auto import_statement_t::get_import_type() const -> import_type_e
{
    return m_import_type;
}

auto import_statement_t::get_imported_symbols() const -> const std::vector<std::string>&
{
    return m_imported_symbols;
}

auto import_statement_t::get_module_specifier() const -> const std::string&
{
    return m_module_specifier;
}

auto import_statement_t::get_alias_name() const -> const std::string&
{
    return m_alias_name;
}

auto import_statement_t::is_path_based() const -> bool
{
    return m_is_path_based;
}

auto import_statement_t::has_alias() const -> bool
{
    return !m_alias_name.empty();
}

} // namespace zephyr
