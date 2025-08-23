#include "interpreter.hpp"
#include "objects/objects.hpp"
#include "objects/function_object.hpp"
#include "objects/lambda_object.hpp"
#include "objects/class_object.hpp"
#include "objects/interface_object.hpp"
#include "objects/class_instance_object.hpp"
#include "async_scheduler.hpp"
#include "errors.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <cmath>

namespace zephyr
{

// Built-in print function
auto print_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << args[i]->to_string();
        if (i < args.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    return std::make_shared<none_object_t>();
}

// Built-in len function
auto len_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("len() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto obj = args[0];
    auto type_name = obj->get_type()->get_name();

    if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(str_obj->get_value().length()));
    } else if (type_name == "list") {
        auto list_obj = std::static_pointer_cast<list_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(list_obj->get_elements().size()));
    } else if (type_name == "dictionary") {
        auto dict_obj = std::static_pointer_cast<dictionary_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(dict_obj->get_elements().size()));
    } else {
        throw type_error_t("len() argument must be a string, list, or dictionary, not '" + type_name + "'", 0, 0, 1);
    }
}

// Built-in input function
auto input_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    std::string prompt = "";
    if (args.size() == 1) {
        prompt = args[0]->to_string();
    } else if (args.size() > 1) {
                        throw type_error_t("input() takes at most 1 argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return std::make_shared<string_object_t>(line);
}

// Built-in str function
auto str_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("str() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    return std::make_shared<string_object_t>(args[0]->to_string());
}

// Built-in int function
auto int_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("int() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto obj = args[0];
    auto type_name = obj->get_type()->get_name();

    if (type_name == "int") {
        return obj; // Already an int
    } else if (type_name == "float") {
        auto float_obj = std::static_pointer_cast<float_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(float_obj->get_value()));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int value = std::stoi(str_obj->get_value());
            return std::make_shared<int_object_t>(value);
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for int(): '" + str_obj->get_value() + "'", 0, 0, 1);
        }
    } else if (type_name == "bool") {
        auto bool_obj = std::static_pointer_cast<boolean_object_t>(obj);
        return std::make_shared<int_object_t>(bool_obj->m_value ? 1 : 0);
    } else {
        throw type_error_t("int() argument must be a string, number, or bool, not '" + type_name + "'", 0, 0, 1);
    }
}

// Built-in float function
auto float_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
                throw type_error_t("float() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto obj = args[0];
    auto type_name = obj->get_type()->get_name();

    if (type_name == "float") {
        return obj; // Already a float
    } else if (type_name == "int") {
        auto int_obj = std::static_pointer_cast<int_object_t>(obj);
        return std::make_shared<float_object_t>(static_cast<double>(int_obj->get_value()));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            double value = std::stod(str_obj->get_value());
            return std::make_shared<float_object_t>(value);
        } catch (const std::exception&) {
            throw value_error_t("could not convert string to float: '" + str_obj->get_value() + "'", 0, 0, 1);
        }
    } else if (type_name == "bool") {
        auto bool_obj = std::static_pointer_cast<boolean_object_t>(obj);
        return std::make_shared<float_object_t>(bool_obj->m_value ? 1.0 : 0.0);
    } else {
        throw type_error_t("float() argument must be a string, number, or bool, not '" + type_name + "'", 0, 0, 1);
    }
}

// Built-in append function
auto append_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 2) {
        throw type_error_t("append() takes exactly 2 arguments (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    auto item = args[1];

    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("append() first argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    list->get_elements_mutable().push_back(item);

    return std::make_shared<none_object_t>();
}

// Built-in pop function
auto pop_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() < 1 || args.size() > 2) {
        throw type_error_t("pop() takes 1 or 2 arguments (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("pop() first argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    if (list->get_elements().empty()) {
        throw index_error_t("pop from empty list", 0, 0, 1);
    }

    int index = static_cast<int>(list->get_elements().size()) - 1; // Default to last element

    if (args.size() == 2) {
        auto index_obj = args[1];
        if (index_obj->get_type()->get_name() != "int") {
            throw type_error_t("pop() index must be an integer, not '" + index_obj->get_type()->get_name() + "'", 0, 0, 1);
        }
        auto int_obj = std::static_pointer_cast<int_object_t>(index_obj);
        index = int_obj->get_value();

        // Handle negative indices
        if (index < 0) {
            index += static_cast<int>(list->get_elements().size());
        }

        if (index < 0 || index >= static_cast<int>(list->get_elements().size())) {
            throw index_error_t("pop index out of range", 0, 0, 1);
        }
    }

    auto result = list->get_elements()[index];
    list->get_elements_mutable().erase(list->get_elements_mutable().begin() + index);
    return result;
}

// Built-in keys function
auto keys_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("keys() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto dict_obj = args[0];
    if (dict_obj->get_type()->get_name() != "dictionary") {
        throw type_error_t("keys() argument must be a dictionary, not '" + dict_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto dict = std::static_pointer_cast<dictionary_object_t>(dict_obj);
    std::vector<std::shared_ptr<object_t>> keys;

    for (const auto& pair : dict->get_elements()) {
        keys.push_back(std::make_shared<string_object_t>(pair.first));
    }

    return std::make_shared<list_object_t>(keys);
}

// Built-in values function
auto values_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("values() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto dict_obj = args[0];
    if (dict_obj->get_type()->get_name() != "dictionary") {
        throw type_error_t("values() argument must be a dictionary, not '" + dict_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto dict = std::static_pointer_cast<dictionary_object_t>(dict_obj);
    std::vector<std::shared_ptr<object_t>> values;

    for (const auto& pair : dict->get_elements()) {
        values.push_back(pair.second);
    }

    return std::make_shared<list_object_t>(values);
}

// Built-in type function
auto type_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("type() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    return std::make_shared<string_object_t>(args[0]->get_type()->get_name());
}

// Built-in map function
auto map_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 2) {
        throw type_error_t("map() takes exactly 2 arguments (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    auto func_obj = args[1];

    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("map() first argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto func_type = func_obj->get_type()->get_name();
    if (func_type != "function" && func_type != "lambda" && func_type != "builtin_function") {
        throw type_error_t("map() second argument must be a function, not '" + func_type + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<object_t>> result_elements;

    // Note: Actual function calling would require interpreter context
    // This is a placeholder implementation
    for (const auto& element : list->get_elements()) {
        // In a real implementation, we would call the function with the element
        result_elements.push_back(element);
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in filter function
auto filter_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 2) {
        throw type_error_t("filter() takes exactly 2 arguments (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    auto func_obj = args[1];

    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("filter() first argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto func_type = func_obj->get_type()->get_name();
    if (func_type != "function" && func_type != "lambda" && func_type != "builtin_function") {
        throw type_error_t("filter() second argument must be a function, not '" + func_type + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<object_t>> result_elements;

    // Note: Actual function calling would require interpreter context
    // This is a placeholder implementation
    for (const auto& element : list->get_elements()) {
        // In a real implementation, we would call the function with the element
        // and check if the result is truthy
        result_elements.push_back(element);
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in reduce function
auto reduce_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() < 2 || args.size() > 3) {
        throw type_error_t("reduce() takes 2 or 3 arguments (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    auto func_obj = args[1];

    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("reduce() first argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto func_type = func_obj->get_type()->get_name();
    if (func_type != "function" && func_type != "lambda" && func_type != "builtin_function") {
        throw type_error_t("reduce() second argument must be a function, not '" + func_type + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    if (list->get_elements().empty() && args.size() < 3) {
        throw value_error_t("reduce() of empty sequence with no initial value", 0, 0, 1);
    }

    std::shared_ptr<object_t> accumulator;
    size_t start_index = 0;

    if (args.size() == 3) {
        accumulator = args[2];
    } else {
        accumulator = list->get_elements()[0];
        start_index = 1;
    }

    // Note: Actual function calling would require interpreter context
    // This is a placeholder implementation that just returns the accumulator
    return accumulator;
}

// Built-in enumerate function
auto enumerate_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
                throw type_error_t("enumerate() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("enumerate() argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<object_t>> result_elements;

    for (size_t i = 0; i < list->get_elements().size(); ++i) {
        std::vector<std::shared_ptr<object_t>> pair = {
            std::make_shared<int_object_t>(static_cast<int>(i)),
            list->get_elements()[i]
        };
        result_elements.push_back(std::make_shared<list_object_t>(pair));
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in zip function
auto zip_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() < 2) {
        throw type_error_t("zip() requires at least 2 arguments (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    std::vector<std::shared_ptr<list_object_t>> lists;
    size_t min_length = SIZE_MAX;

    // Validate all arguments are lists and find minimum length
    for (const auto& arg : args) {
        if (arg->get_type()->get_name() != "list") {
            throw type_error_t("zip() arguments must be lists, not '" + arg->get_type()->get_name() + "'", 0, 0, 1);
        }

        auto list = std::static_pointer_cast<list_object_t>(arg);
        lists.push_back(list);
        min_length = std::min(min_length, list->get_elements().size());
    }

    std::vector<std::shared_ptr<object_t>> result_elements;

    for (size_t i = 0; i < min_length; ++i) {
        std::vector<std::shared_ptr<object_t>> tuple;
        for (const auto& list : lists) {
            tuple.push_back(list->get_elements()[i]);
        }
        result_elements.push_back(std::make_shared<list_object_t>(tuple));
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in all function for promises
auto all_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("all() takes exactly one argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    auto list_obj = args[0];
    if (list_obj->get_type()->get_name() != "list") {
        throw type_error_t("all() argument must be a list, not '" + list_obj->get_type()->get_name() + "'", 0, 0, 1);
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<promise_object_t>> promises;

    // Extract promises from the list
    for (const auto& element : list->get_elements()) {
        if (element->get_type()->get_name() == "promise") {
            auto promise = std::static_pointer_cast<promise_object_t>(element);
            promises.push_back(promise);
        } else {
            throw type_error_t("all() list must contain only Promise objects", 0, 0, 1);
        }
    }

    // Use the async scheduler to handle Promise.all
    auto& scheduler = async_scheduler_t::instance();
    return scheduler.all(promises);
}

// Built-in exit function
auto exit_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    int exit_code = 0;

    if (args.size() == 1) {
        auto code_obj = args[0];
        if (code_obj->get_type()->get_name() == "int") {
            auto int_obj = std::static_pointer_cast<int_object_t>(code_obj);
            exit_code = int_obj->get_value();
        } else {
            throw type_error_t("exit() argument must be an integer, not '" + code_obj->get_type()->get_name() + "'", 0, 0, 1);
        }
    } else if (args.size() > 1) {
        throw type_error_t("exit() takes at most 1 argument (" + std::to_string(args.size()) + " given)", 0, 0, 1);
    }

    std::exit(exit_code);
    return std::make_shared<none_object_t>(); // This line will never be reached
}

interpreter_t::interpreter_t(const std::string& filename, const std::string& source_code)
    : m_filename(filename), m_source_code(source_code), m_current_result(nullptr)
{
    // Initialize global scope with built-in functions
    if (!s_builtins_initialized)
    {
        s_builtin_functions["print"] = std::make_shared<builtin_function_object_t>(print_builtin, "print");
        s_builtin_functions["len"] = std::make_shared<builtin_function_object_t>(len_builtin, "len");
        s_builtin_functions["input"] = std::make_shared<builtin_function_object_t>(input_builtin, "input");
        s_builtin_functions["str"] = std::make_shared<builtin_function_object_t>(str_builtin, "str");
        s_builtin_functions["int"] = std::make_shared<builtin_function_object_t>(int_builtin, "int");
        s_builtin_functions["float"] = std::make_shared<builtin_function_object_t>(float_builtin, "float");
        s_builtin_functions["append"] = std::make_shared<builtin_function_object_t>(append_builtin, "append");
        s_builtin_functions["pop"] = std::make_shared<builtin_function_object_t>(pop_builtin, "pop");
        s_builtin_functions["keys"] = std::make_shared<builtin_function_object_t>(keys_builtin, "keys");
        s_builtin_functions["values"] = std::make_shared<builtin_function_object_t>(values_builtin, "values");
        s_builtin_functions["type"] = std::make_shared<builtin_function_object_t>(type_builtin, "type");
        s_builtin_functions["map"] = std::make_shared<builtin_function_object_t>(map_builtin, "map");
        s_builtin_functions["filter"] = std::make_shared<builtin_function_object_t>(filter_builtin, "filter");
        s_builtin_functions["reduce"] = std::make_shared<builtin_function_object_t>(reduce_builtin, "reduce");
        s_builtin_functions["enumerate"] = std::make_shared<builtin_function_object_t>(enumerate_builtin, "enumerate");
        s_builtin_functions["zip"] = std::make_shared<builtin_function_object_t>(zip_builtin, "zip");
        s_builtin_functions["all"] = std::make_shared<builtin_function_object_t>(all_builtin, "all");
        s_builtin_functions["exit"] = std::make_shared<builtin_function_object_t>(exit_builtin, "exit");
        s_builtins_initialized = true;
    }

    m_global_scope = s_builtin_functions;
    m_scope_stack.push_back(m_global_scope);
}

auto interpreter_t::interpret(program_t& program) -> void
{
    try
    {
        program.accept(*this);
    }
    catch (const return_value_t& return_val)
    {
        // Program-level return (not in function)
        m_current_result = return_val.value;
    }
}

auto interpreter_t::get_current_result() const -> value_t
{
    return m_current_result;
}

auto interpreter_t::get_builtins() -> const std::map<std::string, value_t>&
{
    return s_builtin_functions;
}

// AST Visitor Methods Implementation
auto interpreter_t::visit(number_t& node) -> void
{
    m_current_result = std::make_shared<int_object_t>(node.value);
}

auto interpreter_t::visit(float_literal_t& node) -> void
{
    m_current_result = std::make_shared<float_object_t>(node.value);
}

auto interpreter_t::visit(string_literal_t& node) -> void
{
    m_current_result = std::make_shared<string_object_t>(node.value);
}

auto interpreter_t::visit(fstring_t& node) -> void
{
    std::string result;
    for (auto& part : node.parts)
    {
        part->accept(*this);
        auto part_result = m_current_result;
        if (part_result)
        {
            result += part_result->to_string();
        }
    }
    m_current_result = std::make_shared<string_object_t>(result);
}

auto interpreter_t::visit(boolean_literal_t& node) -> void
{
    m_current_result = std::make_shared<boolean_object_t>(node.value);
}

auto interpreter_t::visit(none_literal_t& node) -> void
{
    m_current_result = std::make_shared<none_object_t>();
}

auto interpreter_t::visit(list_literal_t& node) -> void
{
    auto list_obj = std::make_shared<list_object_t>();
    for (auto& element : node.elements)
    {
        element->accept(*this);
        list_obj->get_elements_mutable().push_back(m_current_result);
    }
    m_current_result = list_obj;
}

auto interpreter_t::visit(dictionary_literal_t& node) -> void
{
    auto dict_obj = std::make_shared<dictionary_object_t>();
    for (auto& pair : node.key_value_pairs)
    {
        pair.first->accept(*this);
        auto key = m_current_result;
        pair.second->accept(*this);
        auto value = m_current_result;
        dict_obj->set_item(key, value);
    }
    m_current_result = dict_obj;
}

auto interpreter_t::visit(name_t& node) -> void
{
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(node.name) != scope.end())
        {
            m_current_result = scope[node.name];
            return;
        }
    }
    throw name_error_t("Undefined variable: " + node.name, node.line, node.column, node.name.length());
}

auto interpreter_t::visit(binary_op_t& node) -> void
{
    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    if (node.operator_token == '+')
    {
        m_current_result = left->add(right);
    }
    else if (node.operator_token == '-')
    {
        m_current_result = left->subtract(right);
    }
    else if (node.operator_token == '*')
    {
        m_current_result = left->multiply(right);
    }
    else if (node.operator_token == '/')
    {
        m_current_result = left->divide(right);
    }
    else if (node.operator_token == '%')
    {
        m_current_result = left->modulo(right);
    }
}

auto interpreter_t::visit(comparison_op_t& node) -> void
{
    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    bool result = false;

    // Basic type-based comparison implementation
    auto left_type = left->get_type()->get_name();
    auto right_type = right->get_type()->get_name();

    if (node.operator_token == "==")
    {
        if (left_type != right_type)
        {
            result = false;
        }
        else if (left_type == "int")
        {
            auto left_int = std::static_pointer_cast<int_object_t>(left);
            auto right_int = std::static_pointer_cast<int_object_t>(right);
            result = left_int->get_value() == right_int->get_value();
        }
        else if (left_type == "float")
        {
            auto left_float = std::static_pointer_cast<float_object_t>(left);
            auto right_float = std::static_pointer_cast<float_object_t>(right);
            result = left_float->get_value() == right_float->get_value();
        }
        else if (left_type == "string")
        {
            auto left_str = std::static_pointer_cast<string_object_t>(left);
            auto right_str = std::static_pointer_cast<string_object_t>(right);
            result = left_str->get_value() == right_str->get_value();
        }
        else if (left_type == "bool")
        {
            auto left_bool = std::static_pointer_cast<boolean_object_t>(left);
            auto right_bool = std::static_pointer_cast<boolean_object_t>(right);
            result = left_bool->m_value == right_bool->m_value;
        }
        else if (left_type == "none")
        {
            result = true; // none == none
        }
        else
        {
            result = left.get() == right.get(); // Pointer comparison for objects
        }
    }
    else if (node.operator_token == "!=")
    {
        // Reuse == logic and negate
        if (left_type != right_type)
        {
            result = true;
        }
        else if (left_type == "int")
        {
            auto left_int = std::static_pointer_cast<int_object_t>(left);
            auto right_int = std::static_pointer_cast<int_object_t>(right);
            result = left_int->get_value() != right_int->get_value();
        }
        else if (left_type == "float")
        {
            auto left_float = std::static_pointer_cast<float_object_t>(left);
            auto right_float = std::static_pointer_cast<float_object_t>(right);
            result = left_float->get_value() != right_float->get_value();
        }
        else if (left_type == "string")
        {
            auto left_str = std::static_pointer_cast<string_object_t>(left);
            auto right_str = std::static_pointer_cast<string_object_t>(right);
            result = left_str->get_value() != right_str->get_value();
        }
        else if (left_type == "bool")
        {
            auto left_bool = std::static_pointer_cast<boolean_object_t>(left);
            auto right_bool = std::static_pointer_cast<boolean_object_t>(right);
            result = left_bool->m_value != right_bool->m_value;
        }
        else if (left_type == "none")
        {
            result = false; // none != none is false
        }
        else
        {
            result = left.get() != right.get();
        }
    }
    else if (node.operator_token == "<" || node.operator_token == "<=" ||
             node.operator_token == ">" || node.operator_token == ">=")
    {
        if (left_type == "int" && right_type == "int")
        {
            auto left_int = std::static_pointer_cast<int_object_t>(left);
            auto right_int = std::static_pointer_cast<int_object_t>(right);
            int left_val = left_int->get_value();
            int right_val = right_int->get_value();

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else if (left_type == "float" && right_type == "float")
        {
            auto left_float = std::static_pointer_cast<float_object_t>(left);
            auto right_float = std::static_pointer_cast<float_object_t>(right);
            double left_val = left_float->get_value();
            double right_val = right_float->get_value();

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else if (left_type == "string" && right_type == "string")
        {
            auto left_str = std::static_pointer_cast<string_object_t>(left);
            auto right_str = std::static_pointer_cast<string_object_t>(right);
            std::string left_val = left_str->get_value();
            std::string right_val = right_str->get_value();

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else if ((left_type == "int" && right_type == "float") || (left_type == "float" && right_type == "int"))
        {
            // Handle mixed int/float comparisons by converting to float
            double left_val, right_val;

            if (left_type == "int")
            {
                auto left_int = std::static_pointer_cast<int_object_t>(left);
                left_val = static_cast<double>(left_int->get_value());
            }
            else
            {
                auto left_float = std::static_pointer_cast<float_object_t>(left);
                left_val = left_float->get_value();
            }

            if (right_type == "int")
            {
                auto right_int = std::static_pointer_cast<int_object_t>(right);
                right_val = static_cast<double>(right_int->get_value());
            }
            else
            {
                auto right_float = std::static_pointer_cast<float_object_t>(right);
                right_val = right_float->get_value();
            }

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else
        {
            throw type_error_t("Cannot compare objects of different types or unsupported types", node.line, node.column, 1);
        }
    }

    m_current_result = std::make_shared<boolean_object_t>(result);
}

auto interpreter_t::visit(logical_and_op_t& node) -> void
{
    node.left->accept(*this);
    auto left = m_current_result;

    if (!left->is_truthy())
    {
        m_current_result = left;
        return;
    }

    node.right->accept(*this);
}

auto interpreter_t::visit(logical_or_op_t& node) -> void
{
    node.left->accept(*this);
    auto left = m_current_result;

    if (left->is_truthy())
    {
        m_current_result = left;
        return;
    }

    node.right->accept(*this);
}

auto interpreter_t::visit(nullish_coalescing_op_t& node) -> void
{
    node.left->accept(*this);
    auto left = m_current_result;

    if (left->get_type()->get_name() != "none")
    {
        m_current_result = left;
        return;
    }

    node.right->accept(*this);
}

auto interpreter_t::visit(pipe_op_t& node) -> void
{
    // Placeholder implementation
    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;
    m_current_result = right;
}

auto interpreter_t::visit(logical_not_op_t& node) -> void
{
    node.expression->accept(*this);
    auto operand = m_current_result;
    m_current_result = std::make_shared<boolean_object_t>(!operand->is_truthy());
}

auto interpreter_t::visit(unary_op_t& node) -> void
{
    node.expression->accept(*this);
    auto operand = m_current_result;

    if (node.operator_token == '-')
    {
        // Negate using subtraction from zero
        auto zero = std::make_shared<int_object_t>(0);
        m_current_result = zero->subtract(operand);
    }
    else if (node.operator_token == '+')
    {
        m_current_result = operand;
    }
    else if (node.operator_token == '!')
    {
        bool is_truthy = true;
        auto type_name = operand->get_type()->get_name();
        if (type_name == "none")
        {
            is_truthy = false;
        }
        else if (type_name == "bool")
        {
            auto bool_obj = std::static_pointer_cast<boolean_object_t>(operand);
            is_truthy = bool_obj->m_value;
        }
        else if (type_name == "int")
        {
            auto int_obj = std::static_pointer_cast<int_object_t>(operand);
            is_truthy = int_obj->get_value() != 0;
        }
        else if (type_name == "string")
        {
            auto str_obj = std::static_pointer_cast<string_object_t>(operand);
            is_truthy = !str_obj->get_value().empty();
        }

        m_current_result = std::make_shared<boolean_object_t>(!is_truthy);
    }
}

auto interpreter_t::visit(index_access_t& node) -> void
{
    node.object->accept(*this);
    auto list_obj = m_current_result;
    node.index->accept(*this);
    auto index_obj = m_current_result;

    m_current_result = list_obj->get_item(index_obj);
}

auto interpreter_t::visit(optional_index_access_t& node) -> void
{
    node.object->accept(*this);
    auto list_obj = m_current_result;

    if (list_obj->get_type()->get_name() == "none")
    {
        m_current_result = std::make_shared<none_object_t>();
        return;
    }

    node.index->accept(*this);
    auto index_obj = m_current_result;

    try {
        m_current_result = list_obj->get_item(index_obj);
    }
    catch (const std::runtime_error& e)
    {
        // For optional access, return none instead of error
        m_current_result = std::make_shared<none_object_t>();
    }
}

auto interpreter_t::visit(optional_member_access_t& node) -> void
{
    node.object->accept(*this);
    auto obj = m_current_result;

    if (obj->get_type()->get_name() == "none")
    {
        m_current_result = std::make_shared<none_object_t>();
        return;
    }

    try {
        m_current_result = obj->get_type()->get_member(obj, node.member_name);
    } catch (const std::runtime_error& e) {
        // For optional access, try object-based member access, but return none if it fails
        try {
            m_current_result = obj->get_member(node.member_name);
        } catch (const std::runtime_error& member_error) {
            // For optional chaining, member not found should return none
            m_current_result = std::make_shared<none_object_t>();
        }
    }
}

auto interpreter_t::visit(optional_method_call_t& node) -> void
{
    node.object->accept(*this);
    auto obj = m_current_result;

    if (obj->get_type()->get_name() == "none")
    {
        m_current_result = std::make_shared<none_object_t>();
        return;
    }

    std::vector<std::shared_ptr<object_t>> args;
    for (auto& arg : node.arguments)
    {
        arg->accept(*this);
        args.push_back(m_current_result);
    }

    // Handle class instance method calls
    auto class_instance = std::dynamic_pointer_cast<class_instance_t>(obj);
    if (class_instance)
    {
        // Check if the method exists in the class
        if (!class_instance->has_method(node.method_name))
        {
            // For optional calls, if method doesn't exist, return none instead of error
            m_current_result = std::make_shared<none_object_t>();
            return;
        }

        auto method = class_instance->get_method(node.method_name);
        if (!method)
        {
            m_current_result = std::make_shared<none_object_t>();
            return;
        }

        // Check parameter count
        if (args.size() != method->parameters.size())
        {
            // For optional calls, parameter mismatch returns none instead of error
            m_current_result = std::make_shared<none_object_t>();
            return;
        }

        // Create new scope for method
        std::map<std::string, value_t> method_scope;

        // Add 'this' to the scope
        method_scope["this"] = class_instance;

        // Bind parameters to arguments with error protection
        try {
            for (size_t i = 0; i < method->parameters.size(); ++i)
            {
                const auto& param = method->parameters[i];

                // Type checking for explicitly typed parameters
                if (param.has_explicit_type && args[i]->get_type()->get_name() != "none")
                {
                    std::string expected_type = param.type_name;
                    std::string actual_type = args[i]->get_type()->get_name();

                    // Normalize type names for common aliases
                    if (expected_type == "int" && actual_type == "number") {
                        actual_type = "int";
                    } else if (expected_type == "float" && actual_type == "number") {
                        // Allow int to float conversion
                        args[i] = std::make_shared<float_object_t>(
                            static_cast<double>(std::static_pointer_cast<int_object_t>(args[i])->get_value())
                        );
                        actual_type = "float";
                    } else if (expected_type == "string" && actual_type == "string_literal") {
                        actual_type = "string";
                    }

                    if (actual_type != expected_type)
                    {
                        // For optional calls, type mismatch returns none instead of error
                        m_current_result = std::make_shared<none_object_t>();
                        return;
                    }
                }

                method_scope[param.name] = args[i];

                // Mark const parameters for protection
                if (param.is_const)
                {
                    m_const_variables.insert(param.name);
                }
            }

            // Push the method scope
            m_scope_stack.push_back(method_scope);

            try
            {
                // Execute the method with proper return value handling
                method->body->accept(*this);

                // If no return statement, return none
                m_current_result = std::make_shared<none_object_t>();
            }
            catch (const return_value_t& return_val)
            {
                // Method returned a value
                m_current_result = return_val.value;
            }

            // Pop the method scope
            m_scope_stack.pop_back();

            // Clean up const variables for this method call
            for (const auto& param : method->parameters)
            {
                if (param.is_const)
                {
                    m_const_variables.erase(param.name);
                }
            }

        } catch (const std::exception& e) {
            // For optional calls, any error returns none
            m_current_result = std::make_shared<none_object_t>();

            // Ensure scope cleanup on error
            if (!m_scope_stack.empty()) {
                m_scope_stack.pop_back();
            }

            // Clean up const variables on error
            for (const auto& param : method->parameters)
            {
                if (param.is_const)
                {
                    m_const_variables.erase(param.name);
                }
            }
        }

        return;  // Important: return here for class instances, don't fall through
    }

    // Fallback to regular method call for other object types with error protection
    try {
        m_current_result = obj->call_method(node.method_name, args);
    } catch (const std::exception& e) {
        // For optional calls, any error returns none
        m_current_result = std::make_shared<none_object_t>();
    }
}

auto interpreter_t::visit(ternary_expression_t& node) -> void
{
    node.condition->accept(*this);
    auto condition = m_current_result;

    if (condition->is_truthy())
    {
        node.true_expression->accept(*this);
    }
    else
    {
        node.false_expression->accept(*this);
    }
}

auto interpreter_t::visit(assignment_t& node) -> void
{
    // Check if variable is const before assignment
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
    }

    node.value->accept(*this);
    auto value = m_current_result;

    // Validate type constraint before assignment
    validate_type_constraint(node.variable_name, value);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
}

auto interpreter_t::visit(const_declaration_t& node) -> void
{
    node.value->accept(*this);
    auto value = m_current_result;

    // Mark as const for protection
    m_const_variables.insert(node.variable_name);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
}

auto interpreter_t::visit(typed_declaration_t& node) -> void
{
    node.value->accept(*this);
    auto value = m_current_result;

    // Store the type constraint
    m_type_constraints[node.variable_name] = node.type_name;

    // Validate the initial assignment against the type constraint
    validate_type_constraint(node.variable_name, value);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
}

auto interpreter_t::visit(typed_const_declaration_t& node) -> void
{
    node.value->accept(*this);
    auto value = m_current_result;

    // Store the type constraint
    m_type_constraints[node.variable_name] = node.type_name;

    // Mark as const
    m_const_variables.insert(node.variable_name);

    // Validate the initial assignment against the type constraint
    validate_type_constraint(node.variable_name, value);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
}

auto interpreter_t::visit(empty_declaration_t& node) -> void
{
    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = std::make_shared<none_object_t>();
    m_current_result = std::make_shared<none_object_t>();
}

auto interpreter_t::visit(empty_typed_declaration_t& node) -> void
{
    // Store the type constraint
    m_type_constraints[node.variable_name] = node.type_name;

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = std::make_shared<none_object_t>();
    m_current_result = std::make_shared<none_object_t>();
}

auto interpreter_t::visit(member_variable_declaration_t& node) -> void
{
    if (node.value)
    {
        node.value->accept(*this);
    }
    else
    {
        m_current_result = std::make_shared<none_object_t>();
    }
}

auto interpreter_t::visit(compound_assignment_t& node) -> void
{
    // Check if variable is const before assignment
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
    }

    // Get current value
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(node.variable_name) != scope.end())
        {
            auto current_value = scope[node.variable_name];
            node.value->accept(*this);
            auto new_value = m_current_result;

            std::shared_ptr<object_t> result_value;

            if (node.operator_token == "+=")
            {
                result_value = current_value->add(new_value);
            }
            else if (node.operator_token == "-=")
            {
                result_value = current_value->subtract(new_value);
            }
            else if (node.operator_token == "*=")
            {
                result_value = current_value->multiply(new_value);
            }
            else if (node.operator_token == "/=")
            {
                result_value = current_value->divide(new_value);
            }
            else if (node.operator_token == "%=")
            {
                result_value = current_value->modulo(new_value);
            }

            // Validate type constraint before assignment
            validate_type_constraint(node.variable_name, result_value);

            scope[node.variable_name] = result_value;
            m_current_result = result_value;
            return;
        }
    }
    throw name_error_t("Undefined variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
}

auto interpreter_t::visit(compound_member_assignment_t& node) -> void
{
    // Placeholder implementation
    node.object->accept(*this);
    node.value->accept(*this);
}

auto interpreter_t::visit(compound_indexed_assignment_t& node) -> void
{
    // Placeholder implementation
    node.object->accept(*this);
    node.index->accept(*this);
    node.value->accept(*this);
}

auto interpreter_t::visit(increment_decrement_t& node) -> void
{
    // Check if variable is const before increment/decrement
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
    }

    // Find variable in scope
    std::shared_ptr<object_t>* var_ptr = nullptr;
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(node.variable_name) != scope.end())
        {
            var_ptr = &scope[node.variable_name];
            break;
        }
    }

    if (!var_ptr)
    {
        throw name_error_t("Undefined variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
    }

    auto current_value = *var_ptr;

    // Check if it's a numeric type
    auto int_obj = std::dynamic_pointer_cast<int_object_t>(current_value);
    auto float_obj = std::dynamic_pointer_cast<float_object_t>(current_value);

    if (int_obj)
    {
        int new_value = int_obj->get_value();
        if (node.is_increment)
        {
            new_value++;
        }
        else
        {
            new_value--;
        }
        *var_ptr = std::make_shared<int_object_t>(new_value);
        m_current_result = *var_ptr;
    }
    else if (float_obj)
    {
        double new_value = float_obj->get_value();
        if (node.is_increment)
        {
            new_value += 1.0;
        }
        else
        {
            new_value -= 1.0;
        }
        *var_ptr = std::make_shared<float_object_t>(new_value);
        m_current_result = *var_ptr;
    }
    else
    {
        throw type_error_t("Cannot apply increment/decrement to non-numeric type", node.line, node.column, 1);
    }
}

auto interpreter_t::visit(increment_decrement_expression_t& node) -> void
{
    // Check if variable is const before increment/decrement
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
    }

    // Find variable in scope
    std::shared_ptr<object_t>* var_ptr = nullptr;
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(node.variable_name) != scope.end())
        {
            var_ptr = &scope[node.variable_name];
            break;
        }
    }

    if (!var_ptr)
    {
        throw name_error_t("Undefined variable: " + node.variable_name, node.line, node.column, node.variable_name.length());
    }

    auto current_value = *var_ptr;

    // Check if it's a numeric type
    auto int_obj = std::dynamic_pointer_cast<int_object_t>(current_value);
    auto float_obj = std::dynamic_pointer_cast<float_object_t>(current_value);

    if (int_obj)
    {
        int old_value = int_obj->get_value();
        int new_value = old_value;

        if (node.is_increment)
        {
            new_value++;
        }
        else
        {
            new_value--;
        }

        // Update the variable
        *var_ptr = std::make_shared<int_object_t>(new_value);

        // Return old value for postfix, new value for prefix
        if (node.is_prefix)
        {
            m_current_result = std::make_shared<int_object_t>(new_value);
        }
        else
        {
            m_current_result = std::make_shared<int_object_t>(old_value);
        }
    }
    else if (float_obj)
    {
        double old_value = float_obj->get_value();
        double new_value = old_value;

        if (node.is_increment)
        {
            new_value += 1.0;
        }
        else
        {
            new_value -= 1.0;
        }

        // Update the variable
        *var_ptr = std::make_shared<float_object_t>(new_value);

        // Return old value for postfix, new value for prefix
        if (node.is_prefix)
        {
            m_current_result = std::make_shared<float_object_t>(new_value);
        }
        else
        {
            m_current_result = std::make_shared<float_object_t>(old_value);
        }
    }
    else
    {
        throw type_error_t("Cannot apply increment/decrement to non-numeric type", node.line, node.column, 1);
    }
}

auto interpreter_t::visit(member_increment_decrement_t& node) -> void
{
    // Placeholder implementation
    m_current_result = std::make_shared<none_object_t>();
}

auto interpreter_t::visit(indexed_increment_decrement_t& node) -> void
{
    // Placeholder implementation
    m_current_result = std::make_shared<none_object_t>();
}

auto interpreter_t::visit(indexed_assignment_t& node) -> void
{
    node.object->accept(*this);
    auto list_obj = m_current_result;
    node.index->accept(*this);
    auto index_obj = m_current_result;
    node.value->accept(*this);
    auto value_obj = m_current_result;

    list_obj->set_item(index_obj, value_obj);
    m_current_result = value_obj;
}

auto interpreter_t::visit(expression_statement_t& node) -> void
{
    node.expression->accept(*this);
}

auto interpreter_t::visit(block_t& node) -> void
{
    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }
}

auto interpreter_t::visit(if_statement_t& node) -> void
{
    node.condition->accept(*this);
    auto condition = m_current_result;

    if (condition->get_type()->is_truthy(condition))
    {
        node.then_block->accept(*this);
    }
    else if (node.else_block)
    {
        node.else_block->accept(*this);
    }
}

auto interpreter_t::visit(while_statement_t& node) -> void
{
    while (true)
    {
        node.condition->accept(*this);
        auto condition = m_current_result;

        if (!condition->is_truthy())
        {
            break;
        }

        try
        {
            node.body->accept(*this);
        }
        catch (const break_exception_t&)
        {
            break;
        }
        catch (const continue_exception_t&)
        {
            continue;
        }
    }
}

auto interpreter_t::visit(do_while_statement_t& node) -> void
{
    do
    {
        try
        {
            node.body->accept(*this);
        }
        catch (const break_exception_t&)
        {
            break;
        }
        catch (const continue_exception_t&)
        {
            // Continue to condition check
        }

        node.condition->accept(*this);
        auto condition = m_current_result;

        if (!condition->is_truthy())
        {
            break;
        }
    } while (true);
}

auto interpreter_t::visit(do_until_statement_t& node) -> void
{
    do
    {
        try
        {
            node.body->accept(*this);
        }
        catch (const break_exception_t&)
        {
            break;
        }
        catch (const continue_exception_t&)
        {
            // Continue to condition check
        }

        node.condition->accept(*this);
        auto condition = m_current_result;

        if (condition->is_truthy())
        {
            break;
        }
    } while (true);
}

auto interpreter_t::visit(for_statement_t& node) -> void
{
    // Create new scope for loop
    m_scope_stack.push_back(std::map<std::string, value_t>());

    try
    {
        if (node.initialization)
        {
            node.initialization->accept(*this);
        }

        while (true)
        {
            if (node.condition)
            {
                node.condition->accept(*this);
                auto condition = m_current_result;
                if (!condition->is_truthy())
                {
                    break;
                }
            }

            try
            {
                node.body->accept(*this);
            }
            catch (const break_exception_t&)
            {
                break;
            }
            catch (const continue_exception_t&)
            {
                // Continue to increment
            }

            if (node.increment)
            {
                node.increment->accept(*this);
            }
        }
    }
    catch (...)
    {
        m_scope_stack.pop_back();
        throw;
    }

    m_scope_stack.pop_back();
}

auto interpreter_t::visit(for_each_statement_t& node) -> void
{
    node.collection->accept(*this);
    auto collection = m_current_result;

    m_scope_stack.push_back(std::map<std::string, value_t>());

    try
    {
        auto list_obj = std::dynamic_pointer_cast<list_object_t>(collection);
        if (list_obj)
        {
            for (auto& element : list_obj->get_elements())
            {
                auto& current_scope = m_scope_stack.back();
                if (node.variables.size() == 1)
                {
                    current_scope[node.variables[0].name] = element;
                }
                else if (node.variables.size() > 1)
                {
                    // Handle destructuring assignment for multiple variables
                    auto element_list = std::dynamic_pointer_cast<list_object_t>(element);
                    if (element_list && element_list->get_elements().size() >= node.variables.size())
                    {
                        for (size_t i = 0; i < node.variables.size(); ++i)
                        {
                            current_scope[node.variables[i].name] = element_list->get_elements()[i];
                        }
                    }
                    else
                    {
                        // If destructuring fails, assign none to all variables
                        for (const auto& var : node.variables)
                        {
                            current_scope[var.name] = std::make_shared<none_object_t>();
                        }
                    }
                }

                if (node.where_clause)
                {
                    node.where_clause->accept(*this);
                    auto condition = m_current_result;
                    if (!condition->is_truthy())
                    {
                        continue;
                    }
                }

                try
                {
                    node.body->accept(*this);
                }
                catch (const break_exception_t&)
                {
                    break;
                }
                catch (const continue_exception_t&)
                {
                    continue;
                }
            }
        }

        // Handle dictionary iteration
        auto dict_obj = std::dynamic_pointer_cast<dictionary_object_t>(collection);
        if (dict_obj)
        {
            for (const auto& pair : dict_obj->get_elements())
            {
                auto& current_scope = m_scope_stack.back();
                if (node.variables.size() == 1)
                {
                    // Single variable gets the key
                    current_scope[node.variables[0].name] = std::make_shared<string_object_t>(pair.first);
                }
                else if (node.variables.size() >= 2)
                {
                    // Two variables get key and value
                    current_scope[node.variables[0].name] = std::make_shared<string_object_t>(pair.first);
                    current_scope[node.variables[1].name] = pair.second;
                }

                if (node.where_clause)
                {
                    node.where_clause->accept(*this);
                    auto condition = m_current_result;
                    if (!condition->is_truthy())
                    {
                        continue;
                    }
                }

                try
                {
                    node.body->accept(*this);
                }
                catch (const break_exception_t&)
                {
                    break;
                }
                catch (const continue_exception_t&)
                {
                    continue;
                }
            }
        }
    }
    catch (...)
    {
        m_scope_stack.pop_back();
        throw;
    }

    m_scope_stack.pop_back();
}

auto interpreter_t::visit(loop_statement_t& node) -> void
{
    while (true)
    {
        try
        {
            node.body->accept(*this);
        }
        catch (const break_exception_t&)
        {
            break;
        }
        catch (const continue_exception_t&)
        {
            continue;
        }
    }
}

auto interpreter_t::visit(function_definition_t& node) -> void
{
    // Note: function_object_t constructor expects a unique_ptr to body
    auto func_obj = std::make_shared<function_object_t>(node.parameters, std::move(node.body), node.return_type_name, node.has_explicit_return_type, node.is_async);
    auto& current_scope = m_scope_stack.back();
    current_scope[node.function_name] = func_obj;
    m_current_result = func_obj;
}

auto interpreter_t::visit(function_call_t& node) -> void
{
    // Find function in scope
    std::shared_ptr<object_t> func_obj = nullptr;
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(node.function_name) != scope.end())
        {
            func_obj = scope[node.function_name];
            break;
        }
    }

    if (!func_obj)
    {
        throw name_error_t("Undefined function: " + node.function_name, node.line, node.column, node.function_name.length());
    }

    std::vector<std::shared_ptr<object_t>> args;
    for (auto& arg : node.arguments)
    {
        arg->accept(*this);
        args.push_back(m_current_result);
    }

    // Check if it's a builtin function
    if (func_obj->get_type()->get_name() == "builtin_function")
    {
        m_current_result = func_obj->call(args);
        return;
    }

    // Handle user-defined functions
    auto func = std::dynamic_pointer_cast<function_object_t>(func_obj);
    if (func)
    {
        // Check parameter count
        if (args.size() != func->m_parameters.size())
        {
            throw type_error_t("Function '" + node.function_name + "' expects " +
                                   std::to_string(func->m_parameters.size()) + " arguments, got " +
                                   std::to_string(args.size()), node.line, node.column, node.function_name.length());
        }

        // Create new scope for function
        std::map<std::string, value_t> function_scope;

        // Bind parameters to arguments
        for (size_t i = 0; i < func->m_parameters.size(); ++i)
        {
            const auto& param = func->m_parameters[i];

            // Type checking for explicitly typed parameters
            if (param.has_explicit_type && args[i]->get_type()->get_name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->get_type()->get_name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    bool is_interface = false;
                    if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(args[i])) {
                        auto class_obj = class_instance->m_class_obj;
                        for (const auto& interface_name : class_obj->get_interfaces()) {
                            if (interface_name == expected_type) {
                                is_interface = true;
                                break;
                            }
                        }
                    }
                    if (!is_interface) {
                        throw type_error_t("Type mismatch for parameter '" + param.name +
                                               "': expected " + param.type_name +
                                               ", got " + args[i]->get_type()->get_name(), node.line, node.column, node.function_name.length());
                    }
                }
            }

            function_scope[param.name] = args[i];

            // Track const parameters
            if (param.is_const)
            {
                m_const_variables.insert(param.name);
            }
        }

        // Push function scope
        m_scope_stack.push_back(function_scope);
        m_expected_return_types.push_back(func->m_return_type_name);

        try
        {
            // Execute function body
            func->m_body->accept(*this);

            // If no return statement, return none
            m_current_result = std::make_shared<none_object_t>();
        }
        catch (const return_value_t& return_val)
        {
            // Function returned a value
            m_current_result = return_val.value;
        }
        catch (const std::runtime_error& e)
        {
            // Pop function scope and re-throw
            m_scope_stack.pop_back();
            m_expected_return_types.pop_back();
            for (const auto& param : func->m_parameters)
            {
                if (param.is_const)
                {
                    m_const_variables.erase(param.name);
                }
            }
            throw;
        }

        // Pop function scope
        m_scope_stack.pop_back();
        m_expected_return_types.pop_back();

        // Remove const parameters from tracking when leaving function scope
        for (const auto& param : func->m_parameters)
        {
            if (param.is_const)
            {
                m_const_variables.erase(param.name);
            }
        }
        return;
    }

    // Handle lambda functions
    auto lambda = std::dynamic_pointer_cast<lambda_object_t>(func_obj);
    if (lambda)
    {
        // Check parameter count
        if (args.size() != lambda->m_parameters.size())
        {
            throw type_error_t("Lambda function expects " +
                                   std::to_string(lambda->m_parameters.size()) + " arguments, got " +
                                   std::to_string(args.size()), node.line, node.column, 1);
        }

        // Create new scope for lambda
        std::map<std::string, value_t> lambda_scope;

        // Add captured variables
        for (const auto& captured : lambda->m_captured_variables)
        {
            lambda_scope[captured.first] = captured.second;
        }

        // Bind parameters to arguments
        for (size_t i = 0; i < lambda->m_parameters.size(); ++i)
        {
            const auto& param = lambda->m_parameters[i];

            // Type checking for explicitly typed parameters
            if (param.has_explicit_type && args[i]->get_type()->get_name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->get_type()->get_name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    throw type_error_t("Type mismatch for parameter '" + param.name +
                                           "': expected " + param.type_name +
                                           ", got " + args[i]->get_type()->get_name(), node.line, node.column, 1);
                }
            }

            lambda_scope[param.name] = args[i];

            // Track const parameters
            if (param.is_const)
            {
                m_const_variables.insert(param.name);
            }
        }

        // Push lambda scope
        m_scope_stack.push_back(lambda_scope);

        try
        {
            if (lambda->m_is_block_body)
            {
                // Execute lambda block body
                lambda->m_body_block->accept(*this);

                // If no return statement, return none
                m_current_result = std::make_shared<none_object_t>();
            }
            else
            {
                // Execute lambda expression body
                if (lambda->m_body_expression)
                {
                    lambda->m_body_expression->accept(*this);
                    // Result is already in m_current_result
                }
                else
                {
                    throw runtime_error_with_location_t("Lambda body expression is null - possible AST corruption", 0, 0, 1, "InternalError");
                }
            }
        }
        catch (const return_value_t& return_val)
        {
            // Lambda returned a value
            m_current_result = return_val.value;
        }

        // Pop lambda scope
        m_scope_stack.pop_back();

        // Remove const parameters from tracking when leaving lambda scope
        for (const auto& param : lambda->m_parameters)
        {
            if (param.is_const)
            {
                m_const_variables.erase(param.name);
            }
        }
        return;
    }

    // Handle class objects (class instantiation)
    auto class_obj = std::dynamic_pointer_cast<class_object_t>(func_obj);
    if (class_obj)
    {
        if (class_obj->m_has_invalid_init) {
            throw type_error_t("init method cannot return a value.", node.line, node.column, 1);
        }

        // Create a new instance of this class
        auto instance = std::make_shared<class_instance_t>(class_obj);

        // Check if the class has an 'init' method
        if (class_obj->has_method("init"))
        {
            auto init_method = class_obj->get_method("init");
            if (init_method && contains_return_with_value(init_method->body.get())) {
                throw type_error_t("init method cannot return a value.", node.line, node.column, 1);
            }

            if (init_method)
            {
                // Check parameter count
                if (args.size() != init_method->parameters.size())
                {
                    throw type_error_t("Constructor for class '" + class_obj->m_class_name + "' expects " +
                                           std::to_string(init_method->parameters.size()) + " arguments, got " +
                                           std::to_string(args.size()), node.line, node.column, 1);
                }

                // Create new scope for init method
                std::map<std::string, value_t> init_scope;

                // Add 'this' to the scope
                init_scope["this"] = instance;

                // Bind parameters to arguments
                for (size_t i = 0; i < init_method->parameters.size(); ++i)
                {
                    const auto& param = init_method->parameters[i];

                    // Type checking for explicitly typed parameters
                    if (param.has_explicit_type && args[i]->get_type()->get_name() != "none")
                    {
                        std::string expected_type = param.type_name;
                        std::string actual_type = args[i]->get_type()->get_name();

                        // Normalize type names for common aliases
                        if (expected_type == "dict") expected_type = "dictionary";
                        if (actual_type == "dict") actual_type = "dictionary";

                        if (actual_type != expected_type)
                        {
                            throw type_error_t("Type mismatch for parameter '" + param.name +
                                                   "': expected " + param.type_name +
                                                   ", got " + args[i]->get_type()->get_name(), node.line, node.column, 1);
                        }
                    }

                    init_scope[param.name] = args[i];

                    // Track const parameters
                    if (param.is_const)
                    {
                        m_const_variables.insert(param.name);
                    }
                }

                // Push init scope
                m_scope_stack.push_back(init_scope);

                try
                {
                    // Execute init method body
                    init_method->body->accept(*this);
                }
                catch (const return_value_t& return_val)
                {
                    // Init method returned a value (not typical but allowed)
                }

                // Pop init scope
                m_scope_stack.pop_back();

                // Remove const parameters from tracking when leaving init scope
                for (const auto& param : init_method->parameters)
                {
                    if (param.is_const)
                    {
                        m_const_variables.erase(param.name);
                    }
                }
            }
        }
        else if (!args.empty())
        {
            // No init method but arguments provided
            throw type_error_t("Class '" + class_obj->m_class_name + "' has no constructor but " +
                                    std::to_string(args.size()) + " arguments were provided", node.line, node.column, 1);
        }

        // Return the created instance
        m_current_result = instance;
        return;
    }

    // If we get here, it's not a callable object
    throw type_error_t("Object is not callable", node.line, node.column, 1);
}

auto interpreter_t::visit(return_statement_t& node) -> void
{
    if (node.return_value)
    {
        node.return_value->accept(*this);
        if (!m_expected_return_types.empty()) {
            std::string expected_type = m_expected_return_types.back();
            if (!expected_type.empty()) {
                std::string actual_type = m_current_result->get_type()->get_name();
                if (actual_type != expected_type) {
                    // Allow returning none from a function with a return type
                    if (actual_type == "none") {
                        throw return_value_t{m_current_result};
                    }
                    throw type_error_t("Type mismatch in return statement: expected " + expected_type + ", got " + actual_type, node.line, node.column, 1);
                }
            }
        }
        throw return_value_t{m_current_result};
    }
    else
    {
        if (!m_expected_return_types.empty()) {
            std::string expected_type = m_expected_return_types.back();
            if (!expected_type.empty() && expected_type != "none") {
                throw type_error_t("Type mismatch in return statement: expected " + expected_type + ", got none", node.line, node.column, 1);
            }
        }
        throw return_value_t{std::make_shared<none_object_t>()};
    }
}

auto interpreter_t::visit(break_statement_t& node) -> void
{
    throw break_exception_t{};
}

auto interpreter_t::visit(continue_statement_t& node) -> void
{
    throw continue_exception_t{};
}

auto interpreter_t::visit(try_catch_statement_t& node) -> void
{
    try
    {
        node.try_block->accept(*this);
    }
    catch (const std::runtime_error& e)
    {
        auto& current_scope = m_scope_stack.back();
        current_scope[node.exception_variable_name] = std::make_shared<string_object_t>(e.what());
        node.catch_block->accept(*this);
    }
}

auto interpreter_t::visit(list_destructuring_assignment_t& node) -> void
{
    node.assigned_expression->accept(*this);
    auto list_obj = std::dynamic_pointer_cast<list_object_t>(m_current_result);

    if (!list_obj)
    {
        throw value_error_t("Cannot destructure non-list value", node.line, node.column, 1);
    }

    auto& current_scope = m_scope_stack.back();
    const auto& elements = list_obj->get_elements();
    for (size_t i = 0; i < node.variable_names.size() && i < elements.size(); ++i)
    {
        current_scope[node.variable_names[i]] = elements[i];
    }
}

auto interpreter_t::visit(switch_statement_t& node) -> void
{
    node.expression->accept(*this);
    auto switch_value = m_current_result;

    bool matched = false;
    bool fall_through = false;

    for (auto& case_stmt : node.cases)
    {
        if (!matched && !fall_through)
        {
            case_stmt->value->accept(*this);
            auto case_value = m_current_result;
            // Basic equality comparison
            bool is_equal = false;
            auto left_type = switch_value->get_type()->get_name();
            auto right_type = case_value->get_type()->get_name();

            if (left_type == right_type)
            {
                if (left_type == "int")
                {
                    auto left_int = std::static_pointer_cast<int_object_t>(switch_value);
                    auto right_int = std::static_pointer_cast<int_object_t>(case_value);
                    is_equal = left_int->get_value() == right_int->get_value();
                }
                else if (left_type == "string")
                {
                    auto left_str = std::static_pointer_cast<string_object_t>(switch_value);
                    auto right_str = std::static_pointer_cast<string_object_t>(case_value);
                    is_equal = left_str->get_value() == right_str->get_value();
                }
                else if (left_type == "bool")
                {
                    auto left_bool = std::static_pointer_cast<boolean_object_t>(switch_value);
                    auto right_bool = std::static_pointer_cast<boolean_object_t>(case_value);
                    is_equal = left_bool->m_value == right_bool->m_value;
                }
                else if (left_type == "none")
                {
                    is_equal = true;
                }
            }

            if (is_equal)
            {
                matched = true;
            }
        }

        if (matched || fall_through)
        {
            try
            {
                for (auto& stmt : case_stmt->statements)
                {
                    stmt->accept(*this);
                }
                fall_through = true; // Continue to next case unless break
            }
            catch (const break_exception_t&)
            {
                return;
            }
        }
    }

    if (!matched && node.has_default_case)
    {
        for (auto& stmt : node.default_statements)
        {
            stmt->accept(*this);
        }
    }
}

auto interpreter_t::visit(case_statement_t& node) -> void
{
    // This shouldn't be called directly - handled by switch_statement
    for (auto& stmt : node.statements)
    {
        stmt->accept(*this);
    }
}

auto interpreter_t::visit(program_t& node) -> void
{
    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }
}

auto interpreter_t::visit(lambda_expression_t& node) -> void
{
    // Capture current scope variables for closure with safer approach
    std::map<std::string, std::shared_ptr<object_t>> captured_vars;

    // Only capture from the immediate parent scope (most recent non-global scope)
    if (m_scope_stack.size() > 1) {
        // Find the most recent function scope (skip the current local scope if it exists)
        for (int i = static_cast<int>(m_scope_stack.size()) - 1; i >= 1; --i) {
            const auto& scope = m_scope_stack[i];
            for (const auto& pair : scope) {
                // Only capture if not already captured (first occurrence wins)
                if (captured_vars.find(pair.first) == captured_vars.end()) {
                    captured_vars[pair.first] = pair.second;
                }
            }
            // Only capture from one scope level to prevent deep capture issues
            break;
        }
    }

    if (node.is_block_body)
    {
        if (!node.body_block) {
            throw runtime_error_with_location_t("Lambda block body is null - AST reuse detected. Functions returning lambdas cannot be called multiple times.", 0, 0, 1, "InternalError");
        }
        // Clone the block body to allow function reuse
        auto cloned_body = clone_block(node.body_block.get());
        std::shared_ptr<block_t> shared_body(cloned_body.release());
        auto lambda_obj = std::make_shared<lambda_object_t>(node.parameters, shared_body, node.return_type_name, node.has_explicit_return_type, captured_vars, node.is_async);
        m_current_result = lambda_obj;
    }
    else
    {
        if (!node.body_expression) {
            throw runtime_error_with_location_t("Lambda expression body is null - AST reuse detected. Functions returning lambdas cannot be called multiple times.", 0, 0, 1, "InternalError");
        }
        // Clone the expression to allow function reuse
        auto cloned_expr = clone_expression(node.body_expression.get());
        std::shared_ptr<expression_t> shared_expr(cloned_expr.release());
        auto lambda_obj = std::make_shared<lambda_object_t>(node.parameters, shared_expr, node.return_type_name, node.has_explicit_return_type, captured_vars, node.is_async);
        m_current_result = lambda_obj;
    }
}

auto interpreter_t::clone_expression(expression_t* expr) -> std::unique_ptr<expression_t>
{
    if (!expr) {
        return nullptr;
    }

    // Handle binary operations
    if (auto binary_op = dynamic_cast<binary_op_t*>(expr)) {
        auto left_clone = clone_expression(binary_op->left.get());
        auto right_clone = clone_expression(binary_op->right.get());
        return std::make_unique<binary_op_t>(
            std::move(left_clone),
            std::move(right_clone),
            binary_op->operator_token,
            binary_op->line, binary_op->column, binary_op->end_line, binary_op->end_column
        );
    }

    // Handle variable names
    if (auto name = dynamic_cast<name_t*>(expr)) {
        return std::make_unique<name_t>(
            name->name,
            name->line, name->column, name->end_line, name->end_column
        );
    }

    // Handle numbers
    if (auto number = dynamic_cast<number_t*>(expr)) {
        return std::make_unique<number_t>(
            number->value,
            number->line, number->column, number->end_line, number->end_column
        );
    }

    // Handle floats
    if (auto float_lit = dynamic_cast<float_literal_t*>(expr)) {
        return std::make_unique<float_literal_t>(
            float_lit->value,
            float_lit->line, float_lit->column, float_lit->end_line, float_lit->end_column
        );
    }

    // Handle strings
    if (auto string_lit = dynamic_cast<string_literal_t*>(expr)) {
        return std::make_unique<string_literal_t>(
            string_lit->value,
            string_lit->line, string_lit->column, string_lit->end_line, string_lit->end_column
        );
    }

    // Handle function calls
    if (auto func_call = dynamic_cast<function_call_t*>(expr)) {
        std::vector<std::unique_ptr<expression_t>> cloned_args;
        for (const auto& arg : func_call->arguments) {
            cloned_args.push_back(clone_expression(arg.get()));
        }
        return std::make_unique<function_call_t>(
            func_call->function_name,
            std::move(cloned_args),
            func_call->line, func_call->column, func_call->end_line, func_call->end_column
        );
    }

    // Handle comparison operations
    if (auto comparison_op = dynamic_cast<comparison_op_t*>(expr)) {
        auto left_clone = clone_expression(comparison_op->left.get());
        auto right_clone = clone_expression(comparison_op->right.get());
        return std::make_unique<comparison_op_t>(
            std::move(left_clone),
            std::move(right_clone),
            comparison_op->operator_token,
            comparison_op->line, comparison_op->column, comparison_op->end_line, comparison_op->end_column
        );
    }

    // Handle ternary expressions
    if (auto ternary = dynamic_cast<ternary_expression_t*>(expr)) {
        auto condition_clone = clone_expression(ternary->condition.get());
        auto true_expr_clone = clone_expression(ternary->true_expression.get());
        auto false_expr_clone = clone_expression(ternary->false_expression.get());
        return std::make_unique<ternary_expression_t>(
            std::move(condition_clone),
            std::move(true_expr_clone),
            std::move(false_expr_clone),
            ternary->line, ternary->column, ternary->end_line, ternary->end_column
        );
    }

    // Handle logical AND operations
    if (auto logical_and = dynamic_cast<logical_and_op_t*>(expr)) {
        auto left_clone = clone_expression(logical_and->left.get());
        auto right_clone = clone_expression(logical_and->right.get());
        return std::make_unique<logical_and_op_t>(
            std::move(left_clone),
            std::move(right_clone),
            logical_and->line, logical_and->column, logical_and->end_line, logical_and->end_column
        );
    }

    // Handle logical OR operations
    if (auto logical_or = dynamic_cast<logical_or_op_t*>(expr)) {
        auto left_clone = clone_expression(logical_or->left.get());
        auto right_clone = clone_expression(logical_or->right.get());
        return std::make_unique<logical_or_op_t>(
            std::move(left_clone),
            std::move(right_clone),
            logical_or->line, logical_or->column, logical_or->end_line, logical_or->end_column
        );
    }

    // Handle logical NOT operations
    if (auto logical_not = dynamic_cast<logical_not_op_t*>(expr)) {
        auto operand_clone = clone_expression(logical_not->expression.get());
        return std::make_unique<logical_not_op_t>(
            std::move(operand_clone),
            logical_not->line, logical_not->column, logical_not->end_line, logical_not->end_column
        );
    }

    // Handle unary operations
    if (auto unary_op = dynamic_cast<unary_op_t*>(expr)) {
        auto operand_clone = clone_expression(unary_op->expression.get());
        return std::make_unique<unary_op_t>(
            unary_op->operator_token,
            std::move(operand_clone),
            unary_op->line, unary_op->column, unary_op->end_line, unary_op->end_column
        );
    }

    // Handle boolean literals
    if (auto bool_lit = dynamic_cast<boolean_literal_t*>(expr)) {
        return std::make_unique<boolean_literal_t>(
            bool_lit->value,
            bool_lit->line, bool_lit->column, bool_lit->end_line, bool_lit->end_column
        );
    }

    // Handle none literals
    if (auto none_lit = dynamic_cast<none_literal_t*>(expr)) {
        return std::make_unique<none_literal_t>(
            none_lit->line, none_lit->column, none_lit->end_line, none_lit->end_column
        );
    }

    // Handle list literals
    if (auto list_lit = dynamic_cast<list_literal_t*>(expr)) {
        auto cloned_list = std::make_unique<list_literal_t>(
            list_lit->line, list_lit->column, list_lit->end_line, list_lit->end_column
        );
        for (const auto& element : list_lit->elements) {
            cloned_list->add_element(clone_expression(element.get()));
        }
        return cloned_list;
    }

    // Handle member access
    if (auto member_access = dynamic_cast<member_access_t*>(expr)) {
        auto object_clone = clone_expression(member_access->object.get());
        return std::make_unique<member_access_t>(
            std::move(object_clone),
            member_access->member_name,
            member_access->line, member_access->column, member_access->end_line, member_access->end_column
        );
    }

    // Handle index access
    if (auto index_access = dynamic_cast<index_access_t*>(expr)) {
        auto object_clone = clone_expression(index_access->object.get());
        auto index_clone = clone_expression(index_access->index.get());
        return std::make_unique<index_access_t>(
            std::move(object_clone),
            std::move(index_clone),
            index_access->line, index_access->column, index_access->end_line, index_access->end_column
        );
    }

    // Handle this expressions
    if (auto this_expr = dynamic_cast<this_expression_t*>(expr)) {
        return std::make_unique<this_expression_t>(
            this_expr->line, this_expr->column, this_expr->end_line, this_expr->end_column
        );
    }

    // Handle method calls
    if (auto method_call = dynamic_cast<method_call_t*>(expr)) {
        auto cloned_object = clone_expression(method_call->object.get());
        std::vector<std::unique_ptr<expression_t>> cloned_args;
        for (const auto& arg : method_call->arguments) {
            cloned_args.push_back(clone_expression(arg.get()));
        }
        return std::make_unique<method_call_t>(
            std::move(cloned_object),
            method_call->method_name,
            std::move(cloned_args),
            method_call->line, method_call->column, method_call->end_line, method_call->end_column
        );
    }

    // Handle lambda expressions
    if (auto lambda_expr = dynamic_cast<lambda_expression_t*>(expr)) {
        if (lambda_expr->is_block_body) {
            // Clone block body
            auto cloned_block = clone_block(lambda_expr->body_block.get());
            return std::make_unique<lambda_expression_t>(
                lambda_expr->parameters,
                std::move(cloned_block),
                lambda_expr->return_type_name, lambda_expr->has_explicit_return_type, lambda_expr->is_async,
                lambda_expr->line, lambda_expr->column, lambda_expr->end_line, lambda_expr->end_column
            );
        } else {
            // Clone expression body
            auto cloned_expr = clone_expression(lambda_expr->body_expression.get());
            return std::make_unique<lambda_expression_t>(
                lambda_expr->parameters,
                std::move(cloned_expr),
                lambda_expr->return_type_name, lambda_expr->has_explicit_return_type, lambda_expr->is_async,
                lambda_expr->line, lambda_expr->column, lambda_expr->end_line, lambda_expr->end_column
            );
        }
    }

    // Handle increment/decrement expressions
    if (auto inc_dec_expr = dynamic_cast<increment_decrement_expression_t*>(expr)) {
        return std::make_unique<increment_decrement_expression_t>(
            inc_dec_expr->variable_name,
            inc_dec_expr->is_increment,
            inc_dec_expr->is_prefix,
            inc_dec_expr->line, inc_dec_expr->column, inc_dec_expr->end_line, inc_dec_expr->end_column
        );
    }

    // Handle in expressions
    if (auto in_expr = dynamic_cast<in_expression_t*>(expr)) {
        auto element_clone = clone_expression(in_expr->element.get());
        auto container_clone = clone_expression(in_expr->container_expression.get());
        return std::make_unique<in_expression_t>(
            std::move(element_clone),
            std::move(container_clone),
            in_expr->line, in_expr->column, in_expr->end_line, in_expr->end_column
        );
    }

    // Handle await expressions
    if (auto await_expr = dynamic_cast<await_expression_t*>(expr)) {
        auto expr_clone = clone_expression(await_expr->expression.get());
        return std::make_unique<await_expression_t>(
            std::move(expr_clone),
            await_expr->line, await_expr->column, await_expr->end_line, await_expr->end_column
        );
    }

    // Handle spawn expressions
    if (auto spawn_expr = dynamic_cast<spawn_expression_t*>(expr)) {
        auto expr_clone = clone_expression(spawn_expr->expression.get());
        return std::make_unique<spawn_expression_t>(
            std::move(expr_clone),
            spawn_expr->line, spawn_expr->column, spawn_expr->end_line, spawn_expr->end_column
        );
    }

    // Handle member increment/decrement expressions
    if (auto member_inc_dec = dynamic_cast<member_increment_decrement_t*>(expr)) {
        auto object_clone = clone_expression(member_inc_dec->object.get());
        return std::make_unique<member_increment_decrement_t>(
            std::move(object_clone),
            member_inc_dec->member_name,
            member_inc_dec->is_increment,
            member_inc_dec->is_prefix,
            member_inc_dec->line, member_inc_dec->column, member_inc_dec->end_line, member_inc_dec->end_column
        );
    }

    // Handle indexed increment/decrement expressions
    if (auto indexed_inc_dec = dynamic_cast<indexed_increment_decrement_t*>(expr)) {
        auto object_clone = clone_expression(indexed_inc_dec->object.get());
        auto index_clone = clone_expression(indexed_inc_dec->index.get());
        return std::make_unique<indexed_increment_decrement_t>(
            std::move(object_clone),
            std::move(index_clone),
            indexed_inc_dec->is_increment,
            indexed_inc_dec->is_prefix,
            indexed_inc_dec->line, indexed_inc_dec->column, indexed_inc_dec->end_line, indexed_inc_dec->end_column
        );
    }

    // Handle optional member access expressions
    if (auto optional_member = dynamic_cast<optional_member_access_t*>(expr)) {
        auto object_clone = clone_expression(optional_member->object.get());
        return std::make_unique<optional_member_access_t>(
            std::move(object_clone),
            optional_member->member_name,
            optional_member->line, optional_member->column, optional_member->end_line, optional_member->end_column
        );
    }

    // Handle optional index access expressions
    if (auto optional_index = dynamic_cast<optional_index_access_t*>(expr)) {
        auto object_clone = clone_expression(optional_index->object.get());
        auto index_clone = clone_expression(optional_index->index.get());
        return std::make_unique<optional_index_access_t>(
            std::move(object_clone),
            std::move(index_clone),
            optional_index->line, optional_index->column, optional_index->end_line, optional_index->end_column
        );
    }

    // Handle optional method call expressions
    if (auto optional_method = dynamic_cast<optional_method_call_t*>(expr)) {
        auto object_clone = clone_expression(optional_method->object.get());
        std::vector<std::unique_ptr<expression_t>> cloned_args;
        for (const auto& arg : optional_method->arguments) {
            cloned_args.push_back(clone_expression(arg.get()));
        }
        return std::make_unique<optional_method_call_t>(
            std::move(object_clone),
            optional_method->method_name,
            std::move(cloned_args),
            optional_method->line, optional_method->column, optional_method->end_line, optional_method->end_column
        );
    }

    // Handle pipe expressions
    if (auto pipe_expr = dynamic_cast<pipe_op_t*>(expr)) {
        auto left_clone = clone_expression(pipe_expr->left.get());
        auto right_clone = clone_expression(pipe_expr->right.get());
        return std::make_unique<pipe_op_t>(
            std::move(left_clone),
            std::move(right_clone),
            pipe_expr->line, pipe_expr->column, pipe_expr->end_line, pipe_expr->end_column
        );
    }

    // Handle nullish coalescing expressions
    if (auto nullish_expr = dynamic_cast<nullish_coalescing_op_t*>(expr)) {
        auto left_clone = clone_expression(nullish_expr->left.get());
        auto right_clone = clone_expression(nullish_expr->right.get());
        return std::make_unique<nullish_coalescing_op_t>(
            std::move(left_clone),
            std::move(right_clone),
            nullish_expr->line, nullish_expr->column, nullish_expr->end_line, nullish_expr->end_column
        );
    }

    // Handle dictionary literal expressions
    if (auto dict_literal = dynamic_cast<dictionary_literal_t*>(expr)) {
        auto cloned_dict = std::make_unique<dictionary_literal_t>(
            dict_literal->line, dict_literal->column, dict_literal->end_line, dict_literal->end_column
        );
        for (const auto& pair : dict_literal->key_value_pairs) {
            auto cloned_key = clone_expression(pair.first.get());
            auto cloned_value = clone_expression(pair.second.get());
            cloned_dict->add_element(std::move(cloned_key), std::move(cloned_value));
        }
        return cloned_dict;
    }

    // For unsupported expression types, throw an error
    throw runtime_error_with_location_t("Unsupported expression type in lambda cloning - please simplify lambda body", 0, 0, 1, "InternalError");
}

auto interpreter_t::clone_statement(statement_t* stmt) -> std::unique_ptr<statement_t>
{
    if (!stmt) {
        return nullptr;
    }

    // Handle expression statements
    if (auto expr_stmt = dynamic_cast<expression_statement_t*>(stmt)) {
        auto cloned_expr = clone_expression(expr_stmt->expression.get());
        return std::make_unique<expression_statement_t>(
            std::move(cloned_expr),
            expr_stmt->line, expr_stmt->column, expr_stmt->end_line, expr_stmt->end_column
        );
    }

    // Handle if statements
    if (auto if_stmt = dynamic_cast<if_statement_t*>(stmt)) {
        auto cloned_condition = clone_expression(if_stmt->condition.get());
        auto cloned_then = clone_block(if_stmt->then_block.get());
        auto cloned_else = clone_block(if_stmt->else_block.get());
        return std::make_unique<if_statement_t>(
            std::move(cloned_condition),
            std::move(cloned_then),
            std::move(cloned_else),
            if_stmt->line, if_stmt->column, if_stmt->end_line, if_stmt->end_column
        );
    }

    // Handle while statements
    if (auto while_stmt = dynamic_cast<while_statement_t*>(stmt)) {
        auto cloned_condition = clone_expression(while_stmt->condition.get());
        auto cloned_body = clone_block(while_stmt->body.get());
        return std::make_unique<while_statement_t>(
            std::move(cloned_condition),
            std::move(cloned_body),
            while_stmt->line, while_stmt->column, while_stmt->end_line, while_stmt->end_column
        );
    }

    // Handle return statements
    if (auto return_stmt = dynamic_cast<return_statement_t*>(stmt)) {
        auto cloned_value = clone_expression(return_stmt->return_value.get());
        return std::make_unique<return_statement_t>(
            std::move(cloned_value),
            return_stmt->line, return_stmt->column, return_stmt->end_line, return_stmt->end_column
        );
    }

    // Handle try-catch statements
    if (auto try_catch = dynamic_cast<try_catch_statement_t*>(stmt)) {
        auto cloned_try = clone_block(try_catch->try_block.get());
        auto cloned_catch = clone_block(try_catch->catch_block.get());
        return std::make_unique<try_catch_statement_t>(
            std::move(cloned_try),
            try_catch->exception_variable_name,
            std::move(cloned_catch),
            try_catch->line, try_catch->column, try_catch->end_line, try_catch->end_column
        );
    }

    // Handle break statements
    if (auto break_stmt = dynamic_cast<break_statement_t*>(stmt)) {
        return std::make_unique<break_statement_t>(
            break_stmt->line, break_stmt->column, break_stmt->end_line, break_stmt->end_column
        );
    }

    // Handle continue statements
    if (auto continue_stmt = dynamic_cast<continue_statement_t*>(stmt)) {
        return std::make_unique<continue_statement_t>(
            continue_stmt->line, continue_stmt->column, continue_stmt->end_line, continue_stmt->end_column
        );
    }

    // Handle assignment statements
    if (auto assignment = dynamic_cast<assignment_t*>(stmt)) {
        auto cloned_value = clone_expression(assignment->value.get());
        return std::make_unique<assignment_t>(
            assignment->variable_name,
            std::move(cloned_value),
            assignment->line, assignment->column, assignment->end_line, assignment->end_column
        );
    }

    // Handle member assignment statements
    if (auto member_assignment = dynamic_cast<member_assignment_t*>(stmt)) {
        auto cloned_object = clone_expression(member_assignment->object.get());
        auto cloned_value = clone_expression(member_assignment->value.get());
        return std::make_unique<member_assignment_t>(
            std::move(cloned_object),
            member_assignment->member_name,
            std::move(cloned_value),
            member_assignment->line, member_assignment->column, member_assignment->end_line, member_assignment->end_column
        );
    }

    // Handle indexed assignment statements
    if (auto indexed_assignment = dynamic_cast<indexed_assignment_t*>(stmt)) {
        auto cloned_object = clone_expression(indexed_assignment->object.get());
        auto cloned_index = clone_expression(indexed_assignment->index.get());
        auto cloned_value = clone_expression(indexed_assignment->value.get());
        return std::make_unique<indexed_assignment_t>(
            std::move(cloned_object),
            std::move(cloned_index),
            std::move(cloned_value),
            indexed_assignment->line, indexed_assignment->column, indexed_assignment->end_line, indexed_assignment->end_column
        );
    }

    // Handle compound assignment statements
    if (auto compound_assignment = dynamic_cast<compound_assignment_t*>(stmt)) {
        auto cloned_value = clone_expression(compound_assignment->value.get());
        return std::make_unique<compound_assignment_t>(
            compound_assignment->variable_name,
            std::move(cloned_value),
            compound_assignment->operator_token,
            compound_assignment->line, compound_assignment->column, compound_assignment->end_line, compound_assignment->end_column
        );
    }

    // Handle for statements
    if (auto for_stmt = dynamic_cast<for_statement_t*>(stmt)) {
        auto cloned_init = clone_statement(for_stmt->initialization.get());
        auto cloned_condition = clone_expression(for_stmt->condition.get());
        auto cloned_increment = clone_statement(for_stmt->increment.get());
        auto cloned_body = clone_block(for_stmt->body.get());
        return std::make_unique<for_statement_t>(
            std::move(cloned_init),
            std::move(cloned_condition),
            std::move(cloned_increment),
            std::move(cloned_body),
            for_stmt->line, for_stmt->column, for_stmt->end_line, for_stmt->end_column
        );
    }

    // Handle for each statements
    if (auto for_each_stmt = dynamic_cast<for_each_statement_t*>(stmt)) {
        auto cloned_collection = clone_expression(for_each_stmt->collection.get());
        auto cloned_body = clone_block(for_each_stmt->body.get());
        auto cloned_where = clone_expression(for_each_stmt->where_clause.get());
        return std::make_unique<for_each_statement_t>(
            for_each_stmt->variables,
            std::move(cloned_collection),
            std::move(cloned_body),
            std::move(cloned_where),
            for_each_stmt->line, for_each_stmt->column, for_each_stmt->end_line, for_each_stmt->end_column
        );
    }

    // Handle loop statements
    if (auto loop_stmt = dynamic_cast<loop_statement_t*>(stmt)) {
        auto cloned_body = clone_block(loop_stmt->body.get());
        return std::make_unique<loop_statement_t>(
            std::move(cloned_body),
            loop_stmt->line, loop_stmt->column, loop_stmt->end_line, loop_stmt->end_column
        );
    }

    // Handle do-while statements
    if (auto do_while_stmt = dynamic_cast<do_while_statement_t*>(stmt)) {
        auto cloned_body = clone_block(do_while_stmt->body.get());
        auto cloned_condition = clone_expression(do_while_stmt->condition.get());
        return std::make_unique<do_while_statement_t>(
            std::move(cloned_body),
            std::move(cloned_condition),
            do_while_stmt->line, do_while_stmt->column, do_while_stmt->end_line, do_while_stmt->end_column
        );
    }

    // Handle do-until statements
    if (auto do_until_stmt = dynamic_cast<do_until_statement_t*>(stmt)) {
        auto cloned_body = clone_block(do_until_stmt->body.get());
        auto cloned_condition = clone_expression(do_until_stmt->condition.get());
        return std::make_unique<do_until_statement_t>(
            std::move(cloned_body),
            std::move(cloned_condition),
            do_until_stmt->line, do_until_stmt->column, do_until_stmt->end_line, do_until_stmt->end_column
        );
    }

    // Handle const declarations
    if (auto const_decl = dynamic_cast<const_declaration_t*>(stmt)) {
        auto cloned_value = clone_expression(const_decl->value.get());
        return std::make_unique<const_declaration_t>(
            const_decl->variable_name,
            std::move(cloned_value),
            const_decl->line, const_decl->column, const_decl->end_line, const_decl->end_column
        );
    }

    // Handle typed declarations
    if (auto typed_decl = dynamic_cast<typed_declaration_t*>(stmt)) {
        auto cloned_value = clone_expression(typed_decl->value.get());
        return std::make_unique<typed_declaration_t>(
            typed_decl->variable_name,
            typed_decl->type_name,
            std::move(cloned_value),
            typed_decl->line, typed_decl->column, typed_decl->end_line, typed_decl->end_column
        );
    }

    // Handle typed const declarations
    if (auto typed_const_decl = dynamic_cast<typed_const_declaration_t*>(stmt)) {
        auto cloned_value = clone_expression(typed_const_decl->value.get());
        return std::make_unique<typed_const_declaration_t>(
            typed_const_decl->variable_name,
            typed_const_decl->type_name,
            std::move(cloned_value),
            typed_const_decl->line, typed_const_decl->column, typed_const_decl->end_line, typed_const_decl->end_column
        );
    }

    // Handle empty declarations
    if (auto empty_decl = dynamic_cast<empty_declaration_t*>(stmt)) {
        return std::make_unique<empty_declaration_t>(
            empty_decl->variable_name,
            empty_decl->line, empty_decl->column, empty_decl->end_line, empty_decl->end_column
        );
    }

    // Handle empty typed declarations
    if (auto empty_typed_decl = dynamic_cast<empty_typed_declaration_t*>(stmt)) {
        return std::make_unique<empty_typed_declaration_t>(
            empty_typed_decl->variable_name,
            empty_typed_decl->type_name,
            empty_typed_decl->line, empty_typed_decl->column, empty_typed_decl->end_line, empty_typed_decl->end_column
        );
    }

    // Handle increment/decrement statements
    if (auto inc_dec_stmt = dynamic_cast<increment_decrement_t*>(stmt)) {
        return std::make_unique<increment_decrement_t>(
            inc_dec_stmt->variable_name,
            inc_dec_stmt->is_increment,
            inc_dec_stmt->is_prefix,
            inc_dec_stmt->line, inc_dec_stmt->column, inc_dec_stmt->end_line, inc_dec_stmt->end_column
        );
    }

    // Handle function definitions
    if (auto func_def = dynamic_cast<function_definition_t*>(stmt)) {
        auto cloned_body = clone_block(func_def->body.get());
        return std::make_unique<function_definition_t>(
            func_def->function_name,
            func_def->parameters,
            std::move(cloned_body),
            func_def->return_type_name, func_def->has_explicit_return_type, func_def->is_async,
            func_def->line, func_def->column, func_def->end_line, func_def->end_column
        );
    }

    // Handle compound member assignment
    if (auto compound_member_assignment = dynamic_cast<compound_member_assignment_t*>(stmt)) {
        auto cloned_object = clone_expression(compound_member_assignment->object.get());
        auto cloned_value = clone_expression(compound_member_assignment->value.get());
        return std::make_unique<compound_member_assignment_t>(
            std::move(cloned_object),
            compound_member_assignment->member_name,
            std::move(cloned_value),
            compound_member_assignment->operator_token,
            compound_member_assignment->line, compound_member_assignment->column, compound_member_assignment->end_line, compound_member_assignment->end_column
        );
    }

    // Handle class definitions
    if (auto class_def = dynamic_cast<class_definition_t*>(stmt)) {
        auto cloned_class_def = std::make_unique<class_definition_t>(
            class_def->class_name,
            class_def->interfaces,
            std::vector<std::unique_ptr<member_variable_declaration_t>>(),
            std::vector<std::unique_ptr<function_definition_t>>(),
            class_def->line, class_def->column, class_def->end_line, class_def->end_column
        );
        for (const auto& member : class_def->member_variables) {
            cloned_class_def->member_variables.push_back(std::unique_ptr<member_variable_declaration_t>(static_cast<member_variable_declaration_t*>(clone_statement(member.get()).release())));
        }
        for (const auto& method : class_def->methods) {
            cloned_class_def->methods.push_back(std::unique_ptr<function_definition_t>(static_cast<function_definition_t*>(clone_statement(method.get()).release())));
        }
        return cloned_class_def;
    }

    // For unsupported statement types, throw an error
    throw runtime_error_with_location_t("Unsupported statement type in lambda cloning: " + std::string(typeid(*stmt).name()), 0, 0, 1, "InternalError");
}

auto interpreter_t::clone_block(block_t* block) -> std::unique_ptr<block_t>
{
    if (!block) {
        return nullptr;
    }

    auto cloned_block = std::make_unique<block_t>(
        block->line, block->column, block->end_line, block->end_column
    );

    for (const auto& stmt : block->statements) {
        auto cloned_stmt = clone_statement(stmt.get());
        cloned_block->add_statement(std::move(cloned_stmt));
    }

    return cloned_block;
}

auto interpreter_t::visit(class_definition_t& node) -> void
{
    // Create a new class object
    auto class_obj = std::make_shared<class_object_t>(node.class_name);

    // Check and process interfaces
    for (const auto& interface_name : node.interfaces) {
        auto interface_obj_val = resolve_variable(interface_name);
        auto interface_obj = std::dynamic_pointer_cast<interface_object_t>(interface_obj_val);
        if (!interface_obj) {
            throw type_error_t("'" + interface_name + "' is not an interface.", node.line, node.column, node.class_name.length());
        }

        class_obj->add_interface(interface_name);

        for (const auto& interface_method : interface_obj->get_methods()) {
            bool method_found = false;
            for (const auto& class_method : node.methods) {
                if (class_method->function_name == interface_method.name) {
                    if (class_method->parameters.size() != interface_method.parameters.size()) {
                        throw type_error_t("Class '" + node.class_name + "' method '" + class_method->function_name + "' has different number of parameters than in interface '" + interface_name + "'.", node.line, node.column, node.class_name.length());
                    }
                    for (size_t i = 0; i < class_method->parameters.size(); ++i) {
                        const auto& class_param = class_method->parameters[i];
                        const auto& interface_param = interface_method.parameters[i];
                        if (class_param.type_name != interface_param.type_name) {
                            throw type_error_t("Class '" + node.class_name + "' method '" + class_method->function_name + "' parameter '" + class_param.name + "' has different type than in interface '" + interface_name + "'.", node.line, node.column, node.class_name.length());
                        }
                    }
                    method_found = true;
                    break;
                }
            }
            if (!method_found) {
                throw type_error_t("Class '" + node.class_name + "' does not implement method '" + interface_method.name + "' from interface '" + interface_name + "'.", node.line, node.column, node.class_name.length());
            }
        }
    }

    // Add all methods to the class
    for (auto& method : node.methods)
    {
        if (method->function_name == "init" && contains_return_with_value(method->body.get())) {
            class_obj->m_has_invalid_init = true;
        }
        auto cloned_method = clone_statement(method.get());
        class_obj->add_method(method->function_name, std::shared_ptr<function_definition_t>(static_cast<function_definition_t*>(cloned_method.release())));
    }

    // Add all member variables to the class
    for (auto& member_var : node.member_variables)
    {
        std::shared_ptr<object_t> default_value = nullptr;

        // Evaluate default value expression if present
        if (member_var->has_default_value && member_var->value)
        {
            member_var->value->accept(*this);
            default_value = m_current_result;
        }

        // Extract member variable information
        member_variable_info_t var_info(
            member_var->variable_name,
            member_var->type_name,
            default_value,
            member_var->has_default_value,
            false // is_const - not supported in member_variable_declaration_t yet
        );
        class_obj->add_member_variable(var_info);
    }

    // Store the class in the current scope
    auto& current_scope = m_scope_stack.back();
    current_scope[node.class_name] = class_obj;
    m_current_result = class_obj;
}

auto interpreter_t::contains_return_with_value(block_t* block) -> bool
{
    if (!block) {
        return false;
    }

    for (const auto& stmt : block->statements) {
        if (auto return_stmt = dynamic_cast<return_statement_t*>(stmt.get())) {
            if (return_stmt->return_value) {
                return true;
            }
        } else if (auto if_stmt = dynamic_cast<if_statement_t*>(stmt.get())) {
            if (contains_return_with_value(if_stmt->then_block.get())) {
                return true;
            }
            if (if_stmt->else_block && contains_return_with_value(if_stmt->else_block.get())) {
                return true;
            }
        } else if (auto while_stmt = dynamic_cast<while_statement_t*>(stmt.get())) {
            if (contains_return_with_value(while_stmt->body.get())) {
                return true;
            }
        } else if (auto for_stmt = dynamic_cast<for_statement_t*>(stmt.get())) {
            if (contains_return_with_value(for_stmt->body.get())) {
                return true;
            }
        } else if (auto for_each_stmt = dynamic_cast<for_each_statement_t*>(stmt.get())) {
            if (contains_return_with_value(for_each_stmt->body.get())) {
                return true;
            }
        } else if (auto do_while_stmt = dynamic_cast<do_while_statement_t*>(stmt.get())) {
            if (contains_return_with_value(do_while_stmt->body.get())) {
                return true;
            }
        } else if (auto do_until_stmt = dynamic_cast<do_until_statement_t*>(stmt.get())) {
            if (contains_return_with_value(do_until_stmt->body.get())) {
                return true;
            }
        } else if (auto loop_stmt = dynamic_cast<loop_statement_t*>(stmt.get())) {
            if (contains_return_with_value(loop_stmt->body.get())) {
                return true;
            }
        } else if (auto switch_stmt = dynamic_cast<switch_statement_t*>(stmt.get())) {
            for (const auto& case_stmt : switch_stmt->cases) {
                for (const auto& case_block_stmt : case_stmt->statements) {
                    if (auto return_stmt = dynamic_cast<return_statement_t*>(case_block_stmt.get())) {
                        if (return_stmt->return_value) {
                            return true;
                        }
                    }
                }
            }
            for (const auto& default_stmt : switch_stmt->default_statements) {
                 if (auto return_stmt = dynamic_cast<return_statement_t*>(default_stmt.get())) {
                    if (return_stmt->return_value) {
                        return true;
                    }
                }
            }
        } else if (auto try_catch_stmt = dynamic_cast<try_catch_statement_t*>(stmt.get())) {
            if (contains_return_with_value(try_catch_stmt->try_block.get())) {
                return true;
            }
            if (contains_return_with_value(try_catch_stmt->catch_block.get())) {
                return true;
            }
        }
    }

    return false;
}

auto interpreter_t::visit(interface_definition_t& node) -> void
{
    // Create a new interface object
    auto interface_obj = std::make_shared<interface_object_t>(node.interface_name);

    // Add all methods to the interface
    for (auto& method : node.methods)
    {
        interface_obj->add_method(method);
    }

    // Store the interface in the current scope
    auto& current_scope = m_scope_stack.back();
    current_scope[node.interface_name] = interface_obj;
    m_current_result = interface_obj;
}

auto interpreter_t::visit(method_call_t& node) -> void
{
    node.object->accept(*this);
    auto obj = m_current_result;

    std::vector<std::shared_ptr<object_t>> args;
    for (auto& arg : node.arguments)
    {
        arg->accept(*this);
        args.push_back(m_current_result);
    }

    // Handle class instance method calls
    auto class_instance = std::dynamic_pointer_cast<class_instance_t>(obj);
    if (class_instance)
    {
        // Check if the method exists in the class
        if (!class_instance->has_method(node.method_name))
        {
            throw attribute_error_t("Method '" + node.method_name + "' not found in class '" +
                                   class_instance->m_class_obj->m_class_name + "'", node.line, node.column, node.method_name.length());
        }

        auto method = class_instance->get_method(node.method_name);
        if (!method)
        {
            throw attribute_error_t("Method '" + node.method_name + "' is null", node.line, node.column, node.method_name.length());
        }

        // Check parameter count
        if (args.size() != method->parameters.size())
        {
            throw type_error_t("Method '" + node.method_name + "' expects " +
                                   std::to_string(method->parameters.size()) + " arguments, got " +
                                   std::to_string(args.size()), node.line, node.column, node.method_name.length());
        }

        // Create new scope for method
        std::map<std::string, value_t> method_scope;

        // Add 'this' to the scope
        method_scope["this"] = class_instance;

        // Bind parameters to arguments
        for (size_t i = 0; i < method->parameters.size(); ++i)
        {
            const auto& param = method->parameters[i];

            // Type checking for explicitly typed parameters
            if (param.has_explicit_type && args[i]->get_type()->get_name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->get_type()->get_name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    bool is_interface = false;
                    if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(args[i])) {
                        auto class_obj = class_instance->m_class_obj;
                        for (const auto& interface_name : class_obj->get_interfaces()) {
                            if (interface_name == expected_type) {
                                is_interface = true;
                                break;
                            }
                        }
                    }
                    if (!is_interface) {
                        throw type_error_t("Type mismatch for parameter '" + param.name +
                                                   "': expected " + param.type_name +
                                                   ", got " + args[i]->get_type()->get_name(), node.line, node.column, node.method_name.length());
                    }
                }
            }

            method_scope[param.name] = args[i];

            // Track const parameters
            if (param.is_const)
            {
                m_const_variables.insert(param.name);
            }
        }

        // Push method scope
        m_scope_stack.push_back(method_scope);
        m_expected_return_types.push_back(method->return_type_name);

        try
        {
            // Execute method body
            method->body->accept(*this);

            // If no return statement, return none
            m_current_result = std::make_shared<none_object_t>();
        }
        catch (const return_value_t& return_val)
        {
            // Method returned a value
            m_current_result = return_val.value;
        }
        catch (const std::runtime_error& e)
        {
            // Pop method scope and re-throw
            m_scope_stack.pop_back();
            m_expected_return_types.pop_back();
            for (const auto& param : method->parameters)
            {
                if (param.is_const)
                {
                    m_const_variables.erase(param.name);
                }
            }
            throw;
        }

        // Pop method scope
        m_scope_stack.pop_back();
        m_expected_return_types.pop_back();

        // Remove const parameters from tracking when leaving method scope
        for (const auto& param : method->parameters)
        {
            if (param.is_const)
            {
                m_const_variables.erase(param.name);
            }
        }
        return;
    }

    // If not a class instance, fall back to the default behavior
    m_current_result = obj->call_method(node.method_name, args);
}

auto interpreter_t::visit(member_access_t& node) -> void
{
    node.object->accept(*this);
    auto obj = m_current_result;
    m_current_result = obj->get_type()->get_member(obj, node.member_name);
}

auto interpreter_t::visit(this_expression_t& node) -> void
{
    // Find 'this' in scope
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find("this") != scope.end())
        {
            m_current_result = scope["this"];
            return;
        }
    }
    throw name_error_t("'this' not available in current context", node.line, node.column, 4);
}

auto interpreter_t::visit(member_assignment_t& node) -> void
{
    node.object->accept(*this);
    auto obj = m_current_result;
    node.value->accept(*this);
    auto value = m_current_result;

    obj->get_type()->set_member(obj, node.member_name, value);
    m_current_result = value;
}

auto interpreter_t::visit(in_expression_t& node) -> void
{
    node.element->accept(*this);
    auto left = m_current_result;
    node.container_expression->accept(*this);
    auto right = m_current_result;

    bool result = false;
    auto container_type = right->get_type()->get_name();

    if (container_type == "list")
    {
        auto list_obj = std::static_pointer_cast<list_object_t>(right);
        const auto& elements = list_obj->get_elements();
        for (const auto& element : elements)
        {
            // Check if element equals left
            auto left_type = left->get_type()->get_name();
            auto elem_type = element->get_type()->get_name();

            if (left_type == elem_type)
            {
                bool is_equal = false;
                if (left_type == "int")
                {
                    auto left_int = std::static_pointer_cast<int_object_t>(left);
                    auto elem_int = std::static_pointer_cast<int_object_t>(element);
                    is_equal = left_int->get_value() == elem_int->get_value();
                }
                else if (left_type == "string")
                {
                    auto left_str = std::static_pointer_cast<string_object_t>(left);
                    auto elem_str = std::static_pointer_cast<string_object_t>(element);
                    is_equal = left_str->get_value() == elem_str->get_value();
                }

                if (is_equal)
                {
                    result = true;
                    break;
                }
            }
        }
    }
    else if (container_type == "string")
    {
        auto str_obj = std::static_pointer_cast<string_object_t>(right);
        auto search_str = std::static_pointer_cast<string_object_t>(left);
        result = str_obj->get_value().find(search_str->get_value()) != std::string::npos;
    }
    else if (container_type == "dictionary")
    {
        auto dict_obj = std::static_pointer_cast<dictionary_object_t>(right);
        if (left->get_type()->get_name() == "string")
        {
            auto key_str = std::static_pointer_cast<string_object_t>(left);
            const auto& elements = dict_obj->get_elements();
            result = elements.find(key_str->get_value()) != elements.end();
        }
    }
    m_current_result = std::make_shared<boolean_object_t>(result);
}

auto interpreter_t::visit(await_expression_t& node) -> void
{
    node.expression->accept(*this);
    auto promise_obj = std::dynamic_pointer_cast<promise_object_t>(m_current_result);

    if (promise_obj)
    {
        m_current_result = promise_obj->m_result;
    }
}

auto interpreter_t::push_scope(const std::map<std::string, value_t>& scope) -> void
{
    m_scope_stack.push_back(scope);
}

auto interpreter_t::pop_scope() -> std::map<std::string, value_t>
{
    if (m_scope_stack.empty()) {
        throw runtime_error_with_location_t("Cannot pop from empty scope stack", 0, 0, 1, "InternalError");
    }
    auto scope = m_scope_stack.back();
    m_scope_stack.pop_back();
    return scope;
}

auto interpreter_t::enter_function_scope() -> void
{
    std::map<std::string, value_t> function_scope;
    m_scope_stack.push_back(function_scope);
}

auto interpreter_t::exit_function_scope() -> void
{
    if (m_scope_stack.size() <= 1) {
        throw runtime_error_with_location_t("Cannot exit global scope", 0, 0, 1, "InternalError");
    }
    m_scope_stack.pop_back();
}

auto interpreter_t::visit(spawn_expression_t& node) -> void
{
    // Create a promise for the spawned expression
    auto promise_obj = std::make_shared<promise_object_t>();

    // For now, execute immediately (placeholder for async execution)
    node.expression->accept(*this);

    // Properly resolve the promise with the result
    promise_obj->resolve(m_current_result);

    m_current_result = promise_obj;
}

auto interpreter_t::validate_type_constraint(const std::string& variable_name, value_t value) -> void
{
    // Check if the variable has a type constraint
    auto constraint_it = m_type_constraints.find(variable_name);
    if (constraint_it != m_type_constraints.end())
    {
        const std::string& expected_type = constraint_it->second;
        const std::string& actual_type = value->get_type()->get_name();

        // Allow none assignment to any typed variable
        if (actual_type == "none")
        {
            return;
        }

        // Handle type aliases and compatibility
        std::string normalized_expected = expected_type;
        std::string normalized_actual = actual_type;

        // Dict is an alias for dictionary type
        if (normalized_expected == "dict") normalized_expected = "dictionary";
        if (normalized_actual == "dict") normalized_actual = "dictionary";


        // Function and lambda types are compatible
        if ((normalized_expected == "function" && normalized_actual == "lambda") ||
            (normalized_expected == "lambda" && normalized_actual == "function")) {
            return; // Allow assignment between function and lambda types
        }

        // Check if a class instance is assigned to an interface
        if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(value)) {
            auto class_obj = class_instance->m_class_obj;
            for (const auto& interface_name : class_obj->get_interfaces()) {
                if (interface_name == expected_type) {
                    return; // Type matches interface
                }
            }
        }

        // Check if normalized types match
        if (normalized_actual != normalized_expected)
        {
            throw type_error_t("Type error: Cannot assign " + actual_type + " to " + expected_type + " variable '" + variable_name + "'", 0, 0, 1);
        }
    }
}

auto interpreter_t::resolve_variable(const std::string& variable_name) -> value_t
{
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(variable_name) != scope.end())
        {
            return scope[variable_name];
        }
    }
    throw name_error_t("Undefined variable: " + variable_name, 0, 0, variable_name.length());
}

auto interpreter_t::set_variable(const std::string& variable_name, value_t value) -> void
{
    // Find the variable in scope stack (starting from most recent scope)
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(variable_name) != scope.end())
        {
            // Validate type constraint before assignment
            validate_type_constraint(variable_name, value);
            scope[variable_name] = value;
            return;
        }
    }

    // If variable doesn't exist, create it in current scope
    auto& current_scope = m_scope_stack.back();
    validate_type_constraint(variable_name, value);
    current_scope[variable_name] = value;
}


// Static member definitions
std::map<std::string, value_t> interpreter_t::s_builtin_functions;
bool interpreter_t::s_builtins_initialized = false;

} // namespace zephyr
