#pragma once

#include <string>
#include <vector>
#include <memory>

namespace zephyr
{
// Forward declarations
class ast_visitor_t;
class switch_statement_t;
class case_statement_t;
class ternary_expression_t;
class lambda_expression_t;
class class_definition_t;
class interface_definition_t;
class method_call_t;
class member_access_t;
class optional_method_call_t;
class optional_member_access_t;
class optional_index_access_t;
class this_expression_t;
class await_expression_t;
class spawn_expression_t;
class typed_declaration_t;
class typed_const_declaration_t;
class empty_declaration_t;
class empty_typed_declaration_t;
class member_variable_declaration_t;

// AST Node Forward Declarations
class number_t;
class float_literal_t;
class string_literal_t;
class fstring_t;
class boolean_literal_t;
class none_literal_t;
class list_literal_t;
class dictionary_literal_t;
class name_t;
class binary_op_t;
class power_op_t;
class comparison_op_t;
class logical_and_op_t;
class logical_or_op_t;
class nullish_coalescing_op_t;
class pipe_op_t;
class logical_not_op_t;
class unary_op_t;
class index_access_t;
class assignment_t;
class const_declaration_t;
class compound_assignment_t;
class compound_member_assignment_t;
class compound_indexed_assignment_t;
class increment_decrement_t;
class increment_decrement_expression_t;
class member_increment_decrement_t;
class indexed_increment_decrement_t;
class member_assignment_t;
class indexed_assignment_t;
class expression_statement_t;
class block_t;
class if_statement_t;
class while_statement_t;
class do_while_statement_t;
class do_until_statement_t;
class for_statement_t;
class for_each_statement_t;
class loop_statement_t;
class function_definition_t;
class function_call_t;
class return_statement_t;
class list_destructuring_assignment_t;
class program_t;
class break_statement_t;
class continue_statement_t;
class try_catch_statement_t;
class in_expression_t;

class bitwise_and_op_t;
class bitwise_or_op_t;
class bitwise_xor_op_t;
class bitwise_not_op_t;
class left_shift_op_t;
class right_shift_op_t;

// Visitor pattern for AST traversal
class ast_visitor_t
{
public:
    virtual ~ast_visitor_t() = default;

    virtual auto visit(number_t& node) -> void = 0;
    virtual auto visit(float_literal_t& node) -> void = 0;
    virtual auto visit(string_literal_t& node) -> void = 0;
    virtual auto visit(fstring_t& node) -> void = 0;
    virtual auto visit(boolean_literal_t& node) -> void = 0;
    virtual auto visit(none_literal_t& node) -> void = 0;
    virtual auto visit(list_literal_t& node) -> void = 0;
    virtual auto visit(dictionary_literal_t& node) -> void = 0;
    virtual auto visit(name_t& node) -> void = 0;
    virtual auto visit(binary_op_t& node) -> void = 0;
    virtual auto visit(power_op_t& node) -> void = 0;
    virtual auto visit(comparison_op_t& node) -> void = 0;
    virtual auto visit(logical_and_op_t& node) -> void = 0;
    virtual auto visit(logical_or_op_t& node) -> void = 0;
    virtual auto visit(nullish_coalescing_op_t& node) -> void = 0;
    virtual auto visit(pipe_op_t& node) -> void = 0;
    virtual auto visit(logical_not_op_t& node) -> void = 0;
    virtual auto visit(unary_op_t& node) -> void = 0;
    virtual auto visit(index_access_t& node) -> void = 0;
    virtual auto visit(optional_index_access_t& node) -> void = 0;
    virtual auto visit(optional_member_access_t& node) -> void = 0;
    virtual auto visit(optional_method_call_t& node) -> void = 0;
    virtual auto visit(ternary_expression_t& node) -> void = 0;
    virtual auto visit(assignment_t& node) -> void = 0;
    virtual auto visit(const_declaration_t& node) -> void = 0;
    virtual auto visit(typed_declaration_t& node) -> void = 0;
    virtual auto visit(typed_const_declaration_t& node) -> void = 0;
    virtual auto visit(empty_declaration_t& node) -> void = 0;
    virtual auto visit(empty_typed_declaration_t& node) -> void = 0;
    virtual auto visit(member_variable_declaration_t& node) -> void = 0;
    virtual auto visit(compound_assignment_t& node) -> void = 0;
    virtual auto visit(compound_member_assignment_t& node) -> void = 0;
    virtual auto visit(compound_indexed_assignment_t& node) -> void = 0;
    virtual auto visit(increment_decrement_t& node) -> void = 0;
    virtual auto visit(increment_decrement_expression_t& node) -> void = 0;
    virtual auto visit(member_increment_decrement_t& node) -> void = 0;
    virtual auto visit(indexed_increment_decrement_t& node) -> void = 0;
    virtual auto visit(indexed_assignment_t& node) -> void = 0;
    virtual auto visit(expression_statement_t& node) -> void = 0;
    virtual auto visit(block_t& node) -> void = 0;
    virtual auto visit(if_statement_t& node) -> void = 0;
    virtual auto visit(while_statement_t& node) -> void = 0;
    virtual auto visit(do_while_statement_t& node) -> void = 0;
    virtual auto visit(do_until_statement_t& node) -> void = 0;
    virtual auto visit(for_statement_t& node) -> void = 0;
    virtual auto visit(for_each_statement_t& node) -> void = 0;
    virtual auto visit(loop_statement_t& node) -> void = 0;
    virtual auto visit(function_definition_t& node) -> void = 0;
    virtual auto visit(function_call_t& node) -> void = 0;
    virtual auto visit(return_statement_t& node) -> void = 0;
    virtual auto visit(break_statement_t& node) -> void = 0;
    virtual auto visit(continue_statement_t& node) -> void = 0;
    virtual auto visit(try_catch_statement_t& node) -> void = 0;
    virtual auto visit(list_destructuring_assignment_t& node) -> void = 0;
    virtual auto visit(switch_statement_t& node) -> void = 0;
    virtual auto visit(case_statement_t& node) -> void = 0;
    virtual auto visit(program_t& node) -> void = 0;
    virtual auto visit(lambda_expression_t& node) -> void = 0;
    virtual auto visit(class_definition_t& node) -> void = 0;
    virtual auto visit(interface_definition_t& node) -> void = 0;
    virtual auto visit(method_call_t& node) -> void = 0;
    virtual auto visit(member_access_t& node) -> void = 0;
    virtual auto visit(this_expression_t& node) -> void = 0;
    virtual auto visit(member_assignment_t& node) -> void = 0;
    virtual auto visit(in_expression_t& node) -> void = 0;
    virtual auto visit(await_expression_t& node) -> void = 0;
    virtual auto visit(spawn_expression_t& node) -> void = 0;
    virtual auto visit(bitwise_and_op_t& node) -> void = 0;
    virtual auto visit(bitwise_or_op_t& node) -> void = 0;
    virtual auto visit(bitwise_xor_op_t& node) -> void = 0;
    virtual auto visit(bitwise_not_op_t& node) -> void = 0;
    virtual auto visit(left_shift_op_t& node) -> void = 0;
    virtual auto visit(right_shift_op_t& node) -> void = 0;
};

// Base class for all AST nodes
class ast_node_t
{
public:
    ast_node_t(int line = 0, int column = 0, int end_line = 0, int end_column = 0)
        : line(line), column(column), end_line(end_line), end_column(end_column)
    {
    }

    virtual ~ast_node_t() = default;

    virtual auto accept(ast_visitor_t& visitor) -> void = 0;

public:
    int line;
    int column;
    int end_line;
    int end_column;
};

// Base expression class
class expression_t : public ast_node_t
{
public:
    expression_t(int line = 0, int column = 0, int end_line = 0, int end_column = 0)
        : ast_node_t(line, column, end_line, end_column)
    {
    }

    virtual ~expression_t() = default;
};

// Base statement class
class statement_t : public ast_node_t
{
public:
    statement_t(int line = 0, int column = 0, int end_line = 0, int end_column = 0)
        : ast_node_t(line, column, end_line, end_column)
    {
    }

    virtual ~statement_t() = default;
};

// Helper structs
struct parameter_t
{
    std::string name;
    bool is_const;
    std::string type_name;
    bool has_explicit_type;

    parameter_t(const std::string& param_name, bool const_param = false, const std::string& type = "", bool has_type = false)
        : name(param_name), is_const(const_param), type_name(type), has_explicit_type(has_type)
    {
    }
};

struct for_each_variable_t
{
    std::string name;
    std::string type_name;
    bool has_explicit_type;
    bool is_const;

    for_each_variable_t(const std::string& variable_name, const std::string& type = "", bool has_type = false, bool const_variable = false)
        : name(variable_name), type_name(type), has_explicit_type(has_type), is_const(const_variable)
    {
    }
};

// Literal expressions
class number_t : public expression_t
{
public:
    number_t(int value, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), value(value)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    int value;
};

class float_literal_t : public expression_t
{
public:
    float_literal_t(double value, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), value(value)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    double value;
};

class string_literal_t : public expression_t
{
public:
    string_literal_t(const std::string& value, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), value(value)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string value;
};

class fstring_t : public expression_t
{
public:
    std::vector<std::unique_ptr<expression_t>> parts;
    fstring_t(std::vector<std::unique_ptr<expression_t>> parts, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), parts(std::move(parts))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;
};

class boolean_literal_t : public expression_t
{
public:
    boolean_literal_t(bool value, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), value(value)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    bool value;
};

class none_literal_t : public expression_t
{
public:
    none_literal_t(int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;
};

class list_literal_t : public expression_t
{
public:
    list_literal_t(int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column)
    {
    }

    auto add_element(std::unique_ptr<expression_t> element) -> void
    {
        elements.push_back(std::move(element));
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<std::unique_ptr<expression_t>> elements;
};

class dictionary_literal_t : public expression_t
{
public:
    dictionary_literal_t(int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column)
    {
    }

    auto add_element(std::unique_ptr<expression_t> key, std::unique_ptr<expression_t> value) -> void
    {
        key_value_pairs.push_back(std::make_pair(std::move(key), std::move(value)));
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<std::pair<std::unique_ptr<expression_t>, std::unique_ptr<expression_t>>> key_value_pairs;
};

class name_t : public expression_t
{
public:
    name_t(const std::string& name, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), name(name)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string name;
};

// Binary operations
class binary_op_t : public expression_t
{
public:
    binary_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, char operator_token, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right)), operator_token(operator_token)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
    char operator_token;
};

class power_op_t : public expression_t
{
public:
    power_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class comparison_op_t : public expression_t
{
public:
    comparison_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, const std::string& operator_token, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right)), operator_token(operator_token)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
    std::string operator_token;
};

class logical_and_op_t : public expression_t
{
public:
    logical_and_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class logical_or_op_t : public expression_t
{
public:
    logical_or_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class nullish_coalescing_op_t : public expression_t
{
public:
    nullish_coalescing_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class pipe_op_t : public expression_t
{
public:
    pipe_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class logical_not_op_t : public expression_t
{
public:
    logical_not_op_t(std::unique_ptr<expression_t> expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), expression(std::move(expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
};

class unary_op_t : public expression_t
{
public:
    unary_op_t(char operator_token, std::unique_ptr<expression_t> expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), expression(std::move(expression)), operator_token(operator_token)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
    char operator_token;
};

// Access operations
class index_access_t : public expression_t
{
public:
    index_access_t(std::unique_ptr<expression_t> object, std::unique_ptr<expression_t> index, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), index(std::move(index))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::unique_ptr<expression_t> index;
};

class optional_index_access_t : public expression_t
{
public:
    optional_index_access_t(std::unique_ptr<expression_t> object, std::unique_ptr<expression_t> index, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), index(std::move(index))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::unique_ptr<expression_t> index;
};

class member_access_t : public expression_t
{
public:
    member_access_t(std::unique_ptr<expression_t> object, const std::string& member_name, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), member_name(member_name)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string member_name;
};

class optional_member_access_t : public expression_t
{
public:
    optional_member_access_t(std::unique_ptr<expression_t> object, const std::string& member_name, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), member_name(member_name)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string member_name;
};

class method_call_t : public expression_t
{
public:
    method_call_t(std::unique_ptr<expression_t> object, const std::string& method_name, std::vector<std::unique_ptr<expression_t>> arguments, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), method_name(method_name), arguments(std::move(arguments))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string method_name;
    std::vector<std::unique_ptr<expression_t>> arguments;
};

class optional_method_call_t : public expression_t
{
public:
    optional_method_call_t(std::unique_ptr<expression_t> object, const std::string& method_name, std::vector<std::unique_ptr<expression_t>> arguments, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), method_name(method_name), arguments(std::move(arguments))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string method_name;
    std::vector<std::unique_ptr<expression_t>> arguments;
};

class function_call_t : public expression_t
{
public:
    function_call_t(const std::string& function_name, std::vector<std::unique_ptr<expression_t>> arguments, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), function_name(function_name), arguments(std::move(arguments))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string function_name;
    std::vector<std::unique_ptr<expression_t>> arguments;
};

class this_expression_t : public expression_t
{
public:
    this_expression_t(int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;
};

class ternary_expression_t : public expression_t
{
public:
    ternary_expression_t(std::unique_ptr<expression_t> condition, std::unique_ptr<expression_t> true_expression, std::unique_ptr<expression_t> false_expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), condition(std::move(condition)), true_expression(std::move(true_expression)), false_expression(std::move(false_expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> condition;
    std::unique_ptr<expression_t> true_expression;
    std::unique_ptr<expression_t> false_expression;
};

class await_expression_t : public expression_t
{
public:
    await_expression_t(std::unique_ptr<expression_t> expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), expression(std::move(expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
};

class spawn_expression_t : public expression_t
{
public:
    spawn_expression_t(std::unique_ptr<expression_t> expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), expression(std::move(expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
};

class in_expression_t : public expression_t
{
public:
    in_expression_t(std::unique_ptr<expression_t> element, std::unique_ptr<expression_t> container_expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), element(std::move(element)), container_expression(std::move(container_expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> element;
    std::unique_ptr<expression_t> container_expression;
};

// Bitwise operations
class bitwise_and_op_t : public expression_t
{
public:
    bitwise_and_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class bitwise_or_op_t : public expression_t
{
public:
    bitwise_or_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class bitwise_xor_op_t : public expression_t
{
public:
    bitwise_xor_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class bitwise_not_op_t : public expression_t
{
public:
    bitwise_not_op_t(std::unique_ptr<expression_t> expression, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), expression(std::move(expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
};

class left_shift_op_t : public expression_t
{
public:
    left_shift_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

class right_shift_op_t : public expression_t
{
public:
    right_shift_op_t(std::unique_ptr<expression_t> left, std::unique_ptr<expression_t> right, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), left(std::move(left)), right(std::move(right))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> left;
    std::unique_ptr<expression_t> right;
};

// Assignment operations
class assignment_t : public statement_t
{
public:
    assignment_t(const std::string& variable_name, std::unique_ptr<expression_t> value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), value(std::move(value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::unique_ptr<expression_t> value;
};

class member_assignment_t : public statement_t
{
public:
    member_assignment_t(std::unique_ptr<expression_t> object, const std::string& member_name, std::unique_ptr<expression_t> value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), object(std::move(object)), member_name(member_name), value(std::move(value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string member_name;
    std::unique_ptr<expression_t> value;
};

class indexed_assignment_t : public statement_t
{
public:
    indexed_assignment_t(std::unique_ptr<expression_t> object, std::unique_ptr<expression_t> index, std::unique_ptr<expression_t> value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), object(std::move(object)), index(std::move(index)), value(std::move(value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::unique_ptr<expression_t> index;
    std::unique_ptr<expression_t> value;
};

// Declaration statements
class const_declaration_t : public statement_t
{
public:
    const_declaration_t(const std::string& variable_name, std::unique_ptr<expression_t> value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), value(std::move(value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::unique_ptr<expression_t> value;
};

class typed_declaration_t : public statement_t
{
public:
    typed_declaration_t(const std::string& variable_name, const std::string& type_name, std::unique_ptr<expression_t> value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), type_name(type_name), value(std::move(value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::string type_name;
    std::unique_ptr<expression_t> value;
};

class typed_const_declaration_t : public statement_t
{
public:
    typed_const_declaration_t(const std::string& variable_name, const std::string& type_name, std::unique_ptr<expression_t> value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), type_name(type_name), value(std::move(value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::string type_name;
    std::unique_ptr<expression_t> value;
};

class empty_declaration_t : public statement_t
{
public:
    empty_declaration_t(const std::string& variable_name, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
};

class empty_typed_declaration_t : public statement_t
{
public:
    empty_typed_declaration_t(const std::string& variable_name, const std::string& type_name, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), type_name(type_name)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::string type_name;
};

class member_variable_declaration_t : public statement_t
{
public:
    member_variable_declaration_t(const std::string& variable_name, const std::string& type_name, std::unique_ptr<expression_t> value, bool has_explicit_type, bool has_default_value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), type_name(type_name), value(std::move(value)), has_explicit_type(has_explicit_type), has_default_value(has_default_value)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::string type_name;
    std::unique_ptr<expression_t> value;
    bool has_explicit_type;
    bool has_default_value;
};

// Compound operations
class compound_assignment_t : public statement_t
{
public:
    compound_assignment_t(const std::string& variable_name, std::unique_ptr<expression_t> value, const std::string& operator_token, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), value(std::move(value)), operator_token(operator_token)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    std::unique_ptr<expression_t> value;
    std::string operator_token;
};

class compound_member_assignment_t : public statement_t
{
public:
    compound_member_assignment_t(std::unique_ptr<expression_t> object, const std::string& member_name, std::unique_ptr<expression_t> value, const std::string& operator_token, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), object(std::move(object)), member_name(member_name), value(std::move(value)), operator_token(operator_token)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string member_name;
    std::unique_ptr<expression_t> value;
    std::string operator_token;
};

class compound_indexed_assignment_t : public statement_t
{
public:
    compound_indexed_assignment_t(std::unique_ptr<expression_t> object, std::unique_ptr<expression_t> index, std::unique_ptr<expression_t> value, const std::string& operator_token, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), object(std::move(object)), index(std::move(index)), value(std::move(value)), operator_token(operator_token)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::unique_ptr<expression_t> index;
    std::unique_ptr<expression_t> value;
    std::string operator_token;
};

// Increment/Decrement operations
class increment_decrement_t : public statement_t
{
public:
    increment_decrement_t(const std::string& variable_name, bool is_increment, bool is_prefix, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_name(variable_name), is_increment(is_increment), is_prefix(is_prefix)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    bool is_increment;
    bool is_prefix;
};

class increment_decrement_expression_t : public expression_t
{
public:
    increment_decrement_expression_t(const std::string& variable_name, bool is_increment, bool is_prefix, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), variable_name(variable_name), is_increment(is_increment), is_prefix(is_prefix)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string variable_name;
    bool is_increment;
    bool is_prefix;
};

class member_increment_decrement_t : public expression_t
{
public:
    member_increment_decrement_t(std::unique_ptr<expression_t> object, const std::string& member_name, bool is_increment, bool is_prefix, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), member_name(member_name), is_increment(is_increment), is_prefix(is_prefix)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::string member_name;
    bool is_increment;
    bool is_prefix;
};

class indexed_increment_decrement_t : public expression_t
{
public:
    indexed_increment_decrement_t(std::unique_ptr<expression_t> object, std::unique_ptr<expression_t> index, bool is_increment, bool is_prefix, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), object(std::move(object)), index(std::move(index)), is_increment(is_increment), is_prefix(is_prefix)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> object;
    std::unique_ptr<expression_t> index;
    bool is_increment;
    bool is_prefix;
};

// Control flow statements
class expression_statement_t : public statement_t
{
public:
    expression_statement_t(std::unique_ptr<expression_t> expression, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), expression(std::move(expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
};

class block_t : public ast_node_t
{
public:
    block_t(int line = 0, int column = 0, int end_line = 0, int end_column = 0)
        : ast_node_t(line, column, end_line, end_column)
    {
    }

    auto add_statement(std::unique_ptr<statement_t> statement) -> void
    {
        statements.push_back(std::move(statement));
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<std::unique_ptr<statement_t>> statements;
};

class if_statement_t : public statement_t
{
public:
    if_statement_t(std::unique_ptr<expression_t> condition, std::unique_ptr<block_t> then_block, std::unique_ptr<block_t> else_block, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), condition(std::move(condition)), then_block(std::move(then_block)), else_block(std::move(else_block))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> condition;
    std::unique_ptr<block_t> then_block;
    std::unique_ptr<block_t> else_block;
};

class while_statement_t : public statement_t
{
public:
    while_statement_t(std::unique_ptr<expression_t> condition, std::unique_ptr<block_t> body, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), condition(std::move(condition)), body(std::move(body))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> condition;
    std::unique_ptr<block_t> body;
};

class do_while_statement_t : public statement_t
{
public:
    do_while_statement_t(std::unique_ptr<block_t> body, std::unique_ptr<expression_t> condition, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), body(std::move(body)), condition(std::move(condition))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<block_t> body;
    std::unique_ptr<expression_t> condition;
};

class do_until_statement_t : public statement_t
{
public:
    do_until_statement_t(std::unique_ptr<block_t> body, std::unique_ptr<expression_t> condition, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), body(std::move(body)), condition(std::move(condition))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<block_t> body;
    std::unique_ptr<expression_t> condition;
};

class for_statement_t : public statement_t
{
public:
    for_statement_t(std::unique_ptr<statement_t> initialization, std::unique_ptr<expression_t> condition, std::unique_ptr<statement_t> increment, std::unique_ptr<block_t> body, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), initialization(std::move(initialization)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<statement_t> initialization;
    std::unique_ptr<expression_t> condition;
    std::unique_ptr<statement_t> increment;
    std::unique_ptr<block_t> body;
};

class for_each_statement_t : public statement_t
{
public:
    for_each_statement_t(std::vector<for_each_variable_t> variables, std::unique_ptr<expression_t> collection, std::unique_ptr<block_t> body, std::unique_ptr<expression_t> where_clause, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variables(std::move(variables)), collection(std::move(collection)), body(std::move(body)), where_clause(std::move(where_clause))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<for_each_variable_t> variables;
    std::unique_ptr<expression_t> collection;
    std::unique_ptr<block_t> body;
    std::unique_ptr<expression_t> where_clause;
};

class loop_statement_t : public statement_t
{
public:
    loop_statement_t(std::unique_ptr<block_t> body, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), body(std::move(body))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<block_t> body;
};

class break_statement_t : public statement_t
{
public:
    break_statement_t(int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;
};

class continue_statement_t : public statement_t
{
public:
    continue_statement_t(int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;
};

class return_statement_t : public statement_t
{
public:
    return_statement_t(std::unique_ptr<expression_t> return_value, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), return_value(std::move(return_value))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> return_value;
};

class try_catch_statement_t : public statement_t
{
public:
    try_catch_statement_t(std::unique_ptr<block_t> try_block, const std::string& exception_variable_name, std::unique_ptr<block_t> catch_block, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), try_block(std::move(try_block)), exception_variable_name(exception_variable_name), catch_block(std::move(catch_block))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<block_t> try_block;
    std::string exception_variable_name;
    std::unique_ptr<block_t> catch_block;
};

class list_destructuring_assignment_t : public statement_t
{
public:
    list_destructuring_assignment_t(const std::vector<std::string>& variable_names, std::unique_ptr<expression_t> assigned_expression, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), variable_names(variable_names), assigned_expression(std::move(assigned_expression))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<std::string> variable_names;
    std::unique_ptr<expression_t> assigned_expression;
};

class case_statement_t : public statement_t
{
public:
    case_statement_t(std::unique_ptr<expression_t> value, std::vector<std::unique_ptr<statement_t>> statements, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), value(std::move(value)), statements(std::move(statements))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> value;
    std::vector<std::unique_ptr<statement_t>> statements;
};

class switch_statement_t : public statement_t
{
public:
    switch_statement_t(std::unique_ptr<expression_t> expression, std::vector<std::unique_ptr<case_statement_t>> cases, std::vector<std::unique_ptr<statement_t>> default_statements, bool has_default_case, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), expression(std::move(expression)), cases(std::move(cases)), default_statements(std::move(default_statements)), has_default_case(has_default_case)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::unique_ptr<expression_t> expression;
    std::vector<std::unique_ptr<case_statement_t>> cases;
    std::vector<std::unique_ptr<statement_t>> default_statements;
    bool has_default_case;
};

class function_definition_t : public statement_t
{
public:
    function_definition_t(const std::string& function_name, std::vector<parameter_t> parameters, std::unique_ptr<block_t> body, const std::string& return_type_name, bool has_explicit_return_type, bool is_async, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), function_name(function_name), parameters(std::move(parameters)), body(std::move(body)), return_type_name(return_type_name), has_explicit_return_type(has_explicit_return_type), is_async(is_async)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string function_name;
    std::vector<parameter_t> parameters;
    std::unique_ptr<block_t> body;
    std::string return_type_name;
    bool has_explicit_return_type;
    bool is_async;
};

class lambda_expression_t : public expression_t
{
public:
    lambda_expression_t(std::vector<parameter_t> parameters, std::unique_ptr<expression_t> body_expression, const std::string& return_type_name, bool has_explicit_return_type, bool is_async, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), parameters(std::move(parameters)), body_expression(std::move(body_expression)), body_block(nullptr), return_type_name(return_type_name), has_explicit_return_type(has_explicit_return_type), is_block_body(false), is_async(is_async)
    {
    }

    lambda_expression_t(std::vector<parameter_t> parameters, std::unique_ptr<block_t> body_block, const std::string& return_type_name, bool has_explicit_return_type, bool is_async, int line, int column, int end_line, int end_column)
        : expression_t(line, column, end_line, end_column), parameters(std::move(parameters)), body_expression(nullptr), body_block(std::move(body_block)), return_type_name(return_type_name), has_explicit_return_type(has_explicit_return_type), is_block_body(true), is_async(is_async)
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<parameter_t> parameters;
    std::unique_ptr<expression_t> body_expression;
    std::unique_ptr<block_t> body_block;
    std::string return_type_name;
    bool has_explicit_return_type;
    bool is_block_body;
    bool is_async;
};

struct function_signature_t
{
    std::string name;
    std::vector<parameter_t> parameters;
    std::string return_type_name;
    bool has_explicit_return_type;
};

class interface_definition_t : public statement_t
{
public:
    interface_definition_t(const std::string& interface_name, std::vector<function_signature_t> methods, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), interface_name(interface_name), methods(std::move(methods))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string interface_name;
    std::vector<function_signature_t> methods;
};

class class_definition_t : public statement_t
{
public:
    class_definition_t(const std::string& class_name, std::vector<std::string> interfaces, std::vector<std::unique_ptr<member_variable_declaration_t>> member_variables, std::vector<std::unique_ptr<function_definition_t>> methods, int line, int column, int end_line, int end_column)
        : statement_t(line, column, end_line, end_column), class_name(class_name), interfaces(std::move(interfaces)), member_variables(std::move(member_variables)), methods(std::move(methods))
    {
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::string class_name;
    std::vector<std::string> interfaces;
    std::vector<std::unique_ptr<member_variable_declaration_t>> member_variables;
    std::vector<std::unique_ptr<function_definition_t>> methods;
};

// Program
class program_t : public ast_node_t
{
public:
    program_t(int line = 0, int column = 0, int end_line = 0, int end_column = 0)
        : ast_node_t(line, column, end_line, end_column)
    {
    }

    auto add_statement(std::unique_ptr<statement_t> statement) -> void
    {
        statements.push_back(std::move(statement));
    }

    auto accept(ast_visitor_t& visitor) -> void override;

public:
    std::vector<std::unique_ptr<statement_t>> statements;
};

}
