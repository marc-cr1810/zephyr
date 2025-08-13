#pragma once

#include "ast.hpp"
#include "objects/objects.hpp"
#include <map>
#include <string>
#include <memory>
#include <set>

namespace zephyr
{

// Value is now a shared pointer to an object_t
using value_t = std::shared_ptr<object_t>;

class interpreter_t : public ast_visitor_t
{
public:
    // Constructors and methods
    interpreter_t(const std::string& filename = "<script>", const std::string& source_code = "");

    auto interpret(program_t& program) -> void;
    auto get_current_result() const -> value_t;

    static auto get_builtins() -> const std::map<std::string, value_t>&;

    // AST visitor methods
    auto visit(number_t& node) -> void override;
    auto visit(float_literal_t& node) -> void override;
    auto visit(string_literal_t& node) -> void override;
    auto visit(fstring_t& node) -> void override;
    auto visit(boolean_literal_t& node) -> void override;
    auto visit(none_literal_t& node) -> void override;
    auto visit(list_literal_t& node) -> void override;
    auto visit(dictionary_literal_t& node) -> void override;
    auto visit(name_t& node) -> void override;
    auto visit(binary_op_t& node) -> void override;
    auto visit(comparison_op_t& node) -> void override;
    auto visit(logical_and_op_t& node) -> void override;
    auto visit(logical_or_op_t& node) -> void override;
    auto visit(nullish_coalescing_op_t& node) -> void override;
    auto visit(pipe_op_t& node) -> void override;
    auto visit(logical_not_op_t& node) -> void override;
    auto visit(unary_op_t& node) -> void override;
    auto visit(index_access_t& node) -> void override;
    auto visit(optional_index_access_t& node) -> void override;
    auto visit(optional_member_access_t& node) -> void override;
    auto visit(optional_method_call_t& node) -> void override;
    auto visit(ternary_expression_t& node) -> void override;
    auto visit(lambda_expression_t& node) -> void override;
    auto visit(class_definition_t& node) -> void override;
    auto visit(method_call_t& node) -> void override;
    auto visit(member_access_t& node) -> void override;
    auto visit(this_expression_t& node) -> void override;
    auto visit(member_assignment_t& node) -> void override;
    auto visit(assignment_t& node) -> void override;
    auto visit(const_declaration_t& node) -> void override;
    auto visit(typed_declaration_t& node) -> void override;
    auto visit(typed_const_declaration_t& node) -> void override;
    auto visit(empty_declaration_t& node) -> void override;
    auto visit(empty_typed_declaration_t& node) -> void override;
    auto visit(member_variable_declaration_t& node) -> void override;
    auto visit(compound_assignment_t& node) -> void override;
    auto visit(compound_member_assignment_t& node) -> void override;
    auto visit(compound_indexed_assignment_t& node) -> void override;
    auto visit(increment_decrement_t& node) -> void override;
    auto visit(increment_decrement_expression_t& node) -> void override;
    auto visit(member_increment_decrement_t& node) -> void override;
    auto visit(indexed_increment_decrement_t& node) -> void override;
    auto visit(indexed_assignment_t& node) -> void override;
    auto visit(expression_statement_t& node) -> void override;
    auto visit(block_t& node) -> void override;
    auto visit(if_statement_t& node) -> void override;
    auto visit(while_statement_t& node) -> void override;
    auto visit(do_while_statement_t& node) -> void override;
    auto visit(do_until_statement_t& node) -> void override;
    auto visit(for_statement_t& node) -> void override;
    auto visit(for_each_statement_t& node) -> void override;
    auto visit(loop_statement_t& node) -> void override;
    auto visit(function_definition_t& node) -> void override;
    auto visit(function_call_t& node) -> void override;
    auto visit(return_statement_t& node) -> void override;
    auto visit(break_statement_t& node) -> void override;
    auto visit(continue_statement_t& node) -> void override;
    auto visit(try_catch_statement_t& node) -> void override;
    auto visit(list_destructuring_assignment_t& node) -> void override;
    auto visit(switch_statement_t& node) -> void override;
    auto visit(case_statement_t& node) -> void override;
    auto visit(program_t& node) -> void override;
    auto visit(in_expression_t& node) -> void override;
    auto visit(await_expression_t& node) -> void override;
    auto visit(spawn_expression_t& node) -> void override;

public:
    // Public member variables
    bool debug_mode;
    std::string current_filename;

private:
    // Private methods
    auto create_scope() -> void;
    auto destroy_scope() -> void;
    auto validate_type_constraint(const std::string& variable_name, value_t value) -> void;
    auto handle_runtime_error(const std::string& message, int line, int column) -> void;
    auto resolve_variable(const std::string& variable_name) -> value_t;
    auto set_variable(const std::string& variable_name, value_t value) -> void;
    auto handle_const_assignment_error(const std::string& variable_name, int line, int column) -> void;
    auto handle_type_error(const std::string& variable_name, const std::string& expected_type, const std::string& actual_type, int line, int column) -> void;
    auto clone_expression(expression_t* expr) -> std::unique_ptr<expression_t>;
    auto clone_statement(statement_t* stmt) -> std::unique_ptr<statement_t>;
    auto clone_block(block_t* block) -> std::unique_ptr<block_t>;

private:
    // Private member variables
    std::vector<std::map<std::string, value_t>> m_scope_stack;
    value_t m_current_result;
    std::string m_filename;
    std::string m_source_code;
    std::map<std::string, std::string> m_type_constraints;
    std::set<std::string> m_const_variables;

protected:
    // Protected methods
    auto push_scope(const std::map<std::string, value_t>& scope) -> void;
    auto pop_scope() -> std::map<std::string, value_t>;
    auto enter_function_scope() -> void;
    auto exit_function_scope() -> void;

protected:
    // Protected member variables
    std::shared_ptr<object_t> m_current_instance;
    std::map<std::string, value_t> m_global_scope;

private:
    // Static members
    static std::map<std::string, value_t> s_builtin_functions;
    static bool s_builtins_initialized;
};

// Exception classes
class return_value_t : public std::runtime_error
{
public:
    return_value_t(value_t val)
        : std::runtime_error("Return value"), value(val)
    {
    }

public:
    value_t value;
};

class break_exception_t : public std::runtime_error
{
public:
    break_exception_t()
        : std::runtime_error("Break statement")
    {
    }
};

class continue_exception_t : public std::runtime_error
{
public:
    continue_exception_t()
        : std::runtime_error("Continue statement")
    {
    }
};

class break_loop_t : public std::runtime_error
{
public:
    break_loop_t()
        : std::runtime_error("Break loop")
    {
    }
};

class continue_loop_t : public std::runtime_error
{
public:
    continue_loop_t()
        : std::runtime_error("Continue loop")
    {
    }
};

class runtime_error_with_location_t : public std::runtime_error
{
public:
    runtime_error_with_location_t(const std::string& message, int line, int column, int length)
        : std::runtime_error(message), line(line), column(column), length(length)
    {
    }

public:
    int line;
    int column;
    int length;
};

}
