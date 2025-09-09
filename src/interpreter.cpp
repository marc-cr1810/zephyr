#include "interpreter.hpp"
#include "error_context.hpp"
#include "objects/function_object.hpp"
#include "objects/lambda_object.hpp"
#include "objects/class_object.hpp"
#include "objects/interface_object.hpp"
#include "objects/class_instance_object.hpp"
#include "async_scheduler.hpp"
#include "errors.hpp"
#include "types/type.hpp"
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
        throw type_error_t("len() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(str_obj->value().length()));
    } else if (type_name == "list") {
        auto list_obj = std::static_pointer_cast<list_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(list_obj->elements().size()));
    } else if (type_name == "dictionary") {
        auto dict_obj = std::static_pointer_cast<dictionary_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(dict_obj->elements().size()));
    } else {
        throw type_error_t("len() argument must be a string, list, or dictionary, not '" + type_name + "'");
    }
}

// Built-in input function
auto input_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    std::string prompt = "";
    if (args.size() == 1) {
        prompt = args[0]->to_string();
    } else if (args.size() > 1) {
                        throw type_error_t("input() takes at most 1 argument (" + std::to_string(args.size()) + " given)");
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
        throw type_error_t("str() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    return std::make_shared<string_object_t>(args[0]->to_string());
}

// Built-in int function
auto int_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("int() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "int") {
        return obj; // Already an int
    } else if (type_name == "float") {
        auto float_obj = std::static_pointer_cast<float_object_t>(obj);
        return std::make_shared<int_object_t>(static_cast<int>(float_obj->value()));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int value = std::stoi(str_obj->value());
            return std::make_shared<int_object_t>(value);
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for int(): '" + str_obj->value() + "'");
        }
    } else if (type_name == "bool") {
        auto bool_obj = std::static_pointer_cast<boolean_object_t>(obj);
        return std::make_shared<int_object_t>(bool_obj->m_value ? 1 : 0);
    } else {
        throw type_error_t("int() argument must be a string, number, or bool, not '" + type_name + "'");
    }
}

// Built-in float function
auto float_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("float() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "float") {
        return obj; // Already a float
    } else if (type_name == "int") {
        auto int_obj = std::static_pointer_cast<int_object_t>(obj);
        return std::make_shared<float_object_t>(static_cast<double>(int_obj->value()));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            double value = std::stod(str_obj->value());
            return std::make_shared<float_object_t>(value);
        } catch (const std::exception&) {
            throw value_error_t("could not convert string to float: '" + str_obj->value() + "'");
        }
    } else if (type_name == "bool") {
        auto bool_obj = std::static_pointer_cast<boolean_object_t>(obj);
        return std::make_shared<float_object_t>(bool_obj->m_value ? 1.0 : 0.0);
    } else {
        throw type_error_t("float() argument must be a string, number, or bool, not '" + type_name + "'");
    }
}

// Built-in append function
auto append_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 2) {
        throw type_error_t("append() takes exactly 2 arguments (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    auto item = args[1];

    if (list_obj->type()->name() != "list") {
        throw type_error_t("append() first argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    list->elements_mutable().push_back(item);

    return std::make_shared<none_object_t>();
}

// Built-in pop function
auto pop_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() < 1 || args.size() > 2) {
        throw type_error_t("pop() takes 1 or 2 arguments (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    if (list_obj->type()->name() != "list") {
        throw type_error_t("pop() first argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    if (list->elements().empty()) {
        throw index_error_t("pop from empty list");
    }

    int index = static_cast<int>(list->elements().size()) - 1; // Default to last element

    if (args.size() == 2) {
        auto index_obj = args[1];
        if (index_obj->type()->name() != "int") {
            throw type_error_t("pop() index must be an integer, not '" + index_obj->type()->name() + "'");
        }
        auto int_obj = std::static_pointer_cast<int_object_t>(index_obj);
        index = int_obj->value();

        // Handle negative indices
        if (index < 0) {
            index += static_cast<int>(list->elements().size());
        }

        if (index < 0 || index >= static_cast<int>(list->elements().size())) {
            throw index_error_t("pop index out of range");
        }
    }

    auto result = list->elements()[index];
    list->elements_mutable().erase(list->elements_mutable().begin() + index);
    return result;
}

// Built-in keys function
auto keys_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("keys() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto dict_obj = args[0];
    if (dict_obj->type()->name() != "dictionary") {
        throw type_error_t("keys() argument must be a dictionary, not '" + dict_obj->type()->name() + "'");
    }

    auto dict = std::static_pointer_cast<dictionary_object_t>(dict_obj);
    std::vector<std::shared_ptr<object_t>> keys;

    for (const auto& pair : dict->elements()) {
        keys.push_back(std::make_shared<string_object_t>(pair.first));
    }

    return std::make_shared<list_object_t>(keys);
}

// Built-in values function
auto values_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("values() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto dict_obj = args[0];
    if (dict_obj->type()->name() != "dictionary") {
        throw type_error_t("values() argument must be a dictionary, not '" + dict_obj->type()->name() + "'");
    }

    auto dict = std::static_pointer_cast<dictionary_object_t>(dict_obj);
    std::vector<std::shared_ptr<object_t>> values;

    for (const auto& pair : dict->elements()) {
        values.push_back(pair.second);
    }

    return std::make_shared<list_object_t>(values);
}

// Built-in type function
auto type_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("type() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    return std::make_shared<string_object_t>(args[0]->type()->name());
}

// Built-in map function
auto map_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 2) {
        throw type_error_t("map() takes exactly 2 arguments (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    auto func_obj = args[1];

    if (list_obj->type()->name() != "list") {
        throw type_error_t("map() first argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto func_type = func_obj->type()->name();
    if (func_type != "function" && func_type != "lambda" && func_type != "builtin_function") {
        throw type_error_t("map() second argument must be a function, not '" + func_type + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<object_t>> result_elements;

    // Note: Actual function calling would require interpreter context
    // This is a placeholder implementation
    for (const auto& element : list->elements()) {
        // In a real implementation, we would call the function with the element
        result_elements.push_back(element);
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in filter function
auto filter_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 2) {
        throw type_error_t("filter() takes exactly 2 arguments (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    auto func_obj = args[1];

    if (list_obj->type()->name() != "list") {
        throw type_error_t("filter() first argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto func_type = func_obj->type()->name();
    if (func_type != "function" && func_type != "lambda" && func_type != "builtin_function") {
        throw type_error_t("filter() second argument must be a function, not '" + func_type + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<object_t>> result_elements;

    // Note: Actual function calling would require interpreter context
    // This is a placeholder implementation
    for (const auto& element : list->elements()) {
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
        throw type_error_t("reduce() takes 2 or 3 arguments (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    auto func_obj = args[1];

    if (list_obj->type()->name() != "list") {
        throw type_error_t("reduce() first argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto func_type = func_obj->type()->name();
    if (func_type != "function" && func_type != "lambda" && func_type != "builtin_function") {
        throw type_error_t("reduce() second argument must be a function, not '" + func_type + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    if (list->elements().empty() && args.size() < 3) {
        throw value_error_t("reduce() of empty sequence with no initial value");
    }

    std::shared_ptr<object_t> accumulator;
    size_t start_index = 0;

    if (args.size() == 3) {
        accumulator = args[2];
    } else {
        accumulator = list->elements()[0];
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
                throw type_error_t("enumerate() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    if (list_obj->type()->name() != "list") {
        throw type_error_t("enumerate() argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<object_t>> result_elements;

    for (size_t i = 0; i < list->elements().size(); ++i) {
        std::vector<std::shared_ptr<object_t>> pair = {
            std::make_shared<int_object_t>(static_cast<int>(i)),
            list->elements()[i]
        };
        result_elements.push_back(std::make_shared<list_object_t>(pair));
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in zip function
auto zip_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() < 2) {
        throw type_error_t("zip() requires at least 2 arguments (" + std::to_string(args.size()) + " given)");
    }

    std::vector<std::shared_ptr<list_object_t>> lists;
    size_t min_length = SIZE_MAX;

    // Validate all arguments are lists and find minimum length
    for (const auto& arg : args) {
        if (arg->type()->name() != "list") {
            throw type_error_t("zip() arguments must be lists, not '" + arg->type()->name() + "'");
        }

        auto list = std::static_pointer_cast<list_object_t>(arg);
        lists.push_back(list);
        min_length = std::min(min_length, list->elements().size());
    }

    std::vector<std::shared_ptr<object_t>> result_elements;

    for (size_t i = 0; i < min_length; ++i) {
        std::vector<std::shared_ptr<object_t>> tuple;
        for (const auto& list : lists) {
            tuple.push_back(list->elements()[i]);
        }
        result_elements.push_back(std::make_shared<list_object_t>(tuple));
    }

    return std::make_shared<list_object_t>(result_elements);
}

// Built-in all function for promises
auto all_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("all() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto list_obj = args[0];
    if (list_obj->type()->name() != "list") {
        throw type_error_t("all() argument must be a list, not '" + list_obj->type()->name() + "'");
    }

    auto list = std::static_pointer_cast<list_object_t>(list_obj);
    std::vector<std::shared_ptr<promise_object_t>> promises;

    // Extract promises from the list
    for (const auto& element : list->elements()) {
        if (element->type()->name() == "promise") {
            auto promise = std::static_pointer_cast<promise_object_t>(element);
            promises.push_back(promise);
        } else {
            throw type_error_t("all() list must contain only Promise objects");
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
        if (code_obj->type()->name() == "int") {
            auto int_obj = std::static_pointer_cast<int_object_t>(code_obj);
            exit_code = int_obj->value();
        } else {
            throw type_error_t("exit() argument must be an integer, not '" + code_obj->type()->name() + "'");
        }
    } else if (args.size() > 1) {
        throw type_error_t("exit() takes at most 1 argument (" + std::to_string(args.size()) + " given)");
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

auto interpreter_t::current_result() const -> value_t
{
    return m_current_result;
}

auto interpreter_t::builtins() -> const std::map<std::string, value_t>&
{
    return s_builtin_functions;
}

// AST Visitor Methods Implementation
auto interpreter_t::visit(number_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;
    m_current_result = std::make_shared<int_object_t>(node.value);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(float_literal_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;
    m_current_result = std::make_shared<float_object_t>(node.value);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(string_literal_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;
    m_current_result = std::make_shared<string_object_t>(node.value);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(fstring_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(boolean_literal_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;
    m_current_result = std::make_shared<boolean_object_t>(node.value);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(none_literal_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;
    m_current_result = std::make_shared<none_object_t>();
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(list_literal_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    auto list_obj = std::make_shared<list_object_t>();
    for (auto& element : node.elements)
    {
        element->accept(*this);
        list_obj->elements_mutable().push_back(m_current_result);
    }
    m_current_result = list_obj;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(dictionary_literal_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    auto dict_obj = std::make_shared<dictionary_object_t>();
    for (auto& pair : node.key_value_pairs)
    {
        pair.first->accept(*this);
        auto key = m_current_result;
        pair.second->accept(*this);
        auto value = m_current_result;
        dict_obj->item(key, value);
    }
    m_current_result = dict_obj;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(name_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.name.length(); // Length is name length

    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find(node.name) != scope.end())
        {
            m_current_result = scope[node.name];
            zephyr::current_error_location() = saved_location; // Restore before returning
            return;
        }
    }
    throw name_error_t("Undefined variable: " + node.name); // No 0,0,1 needed
    zephyr::current_error_location() = saved_location; // Restore after throwing (unreachable, but good practice)
}

auto interpreter_t::visit(binary_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = 1; // Operator token is usually 1 char

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(power_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = 2;

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    auto left_type = left->type()->name();
    auto right_type = right->type()->name();

    if (left_type == "int" && right_type == "int")
    {
        auto left_int = std::static_pointer_cast<int_object_t>(left);
        auto right_int = std::static_pointer_cast<int_object_t>(right);
        m_current_result = std::make_shared<int_object_t>(static_cast<int>(std::pow(left_int->value(), right_int->value())));
    }
    else if ((left_type == "int" || left_type == "float") && (right_type == "int" || right_type == "float"))
    {
        double left_val = (left_type == "int") ? static_cast<double>(std::static_pointer_cast<int_object_t>(left)->value()) : std::static_pointer_cast<float_object_t>(left)->value();
        double right_val = (right_type == "int") ? static_cast<double>(std::static_pointer_cast<int_object_t>(right)->value()) : std::static_pointer_cast<float_object_t>(right)->value();
        m_current_result = std::make_shared<float_object_t>(std::pow(left_val, right_val));
    }
    else
    {
        throw type_error_t("Unsupported operand types for **: " + left_type + " and " + right_type);
    }

    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(comparison_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.operator_token.length(); // Length is operator token length

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    bool result = false;

    // Basic type-based comparison implementation
    auto left_type = left->type()->name();
    auto right_type = right->type()->name();

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
            result = left_int->value() == right_int->value();
        }
        else if (left_type == "float")
        {
            auto left_float = std::static_pointer_cast<float_object_t>(left);
            auto right_float = std::static_pointer_cast<float_object_t>(right);
            result = left_float->value() == right_float->value();
        }
        else if (left_type == "string")
        {
            auto left_str = std::static_pointer_cast<string_object_t>(left);
            auto right_str = std::static_pointer_cast<string_object_t>(right);
            result = left_str->value() == right_str->value();
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
    else if (node.operator_token == "is")
    {
        if (left_type == "none" && right_type == "none")
        {
            result = true;
        }
        else
        {
            result = (left.get() == right.get());
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
            result = left_int->value() != right_int->value();
        }
        else if (left_type == "float")
        {
            auto left_float = std::static_pointer_cast<float_object_t>(left);
            auto right_float = std::static_pointer_cast<float_object_t>(right);
            result = left_float->value() != right_float->value();
        }
        else if (left_type == "string")
        {
            auto left_str = std::static_pointer_cast<string_object_t>(left);
            auto right_str = std::static_pointer_cast<string_object_t>(right);
            result = left_str->value() != right_str->value();
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
            int left_val = left_int->value();
            int right_val = right_int->value();

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else if (left_type == "float" && right_type == "float")
        {
            auto left_float = std::static_pointer_cast<float_object_t>(left);
            auto right_float = std::static_pointer_cast<float_object_t>(right);
            double left_val = left_float->value();
            double right_val = right_float->value();

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else if (left_type == "string" && right_type == "string")
        {
            auto left_str = std::static_pointer_cast<string_object_t>(left);
            auto right_str = std::static_pointer_cast<string_object_t>(right);
            std::string left_val = left_str->value();
            std::string right_val = right_str->value();

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
                left_val = static_cast<double>(left_int->value());
            }
            else
            {
                auto left_float = std::static_pointer_cast<float_object_t>(left);
                left_val = left_float->value();
            }

            if (right_type == "int")
            {
                auto right_int = std::static_pointer_cast<int_object_t>(right);
                right_val = static_cast<double>(right_int->value());
            }
            else
            {
                auto right_float = std::static_pointer_cast<float_object_t>(right);
                right_val = right_float->value();
            }

            if (node.operator_token == "<") result = left_val < right_val;
            else if (node.operator_token == "<=") result = left_val <= right_val;
            else if (node.operator_token == ">") result = left_val > right_val;
            else if (node.operator_token == ">=") result = left_val >= right_val;
        }
        else
        {
            throw type_error_t("Cannot compare objects of different types or unsupported types");
        }
    }

    m_current_result = std::make_shared<boolean_object_t>(result);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(logical_and_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;

    if (!left->is_truthy())
    {
        m_current_result = left;
        zephyr::current_error_location() = saved_location; // Restore before returning
        return;
    }

    node.right->accept(*this);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(logical_or_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;

    if (left->is_truthy())
    {
        m_current_result = left;
        zephyr::current_error_location() = saved_location; // Restore before returning
        return;
    }

    node.right->accept(*this);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(nullish_coalescing_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;

    if (left->type()->name() != "none")
    {
        m_current_result = left;
        zephyr::current_error_location() = saved_location; // Restore before returning
        return;
    }

    node.right->accept(*this);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(pipe_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Placeholder implementation
    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;
    m_current_result = right;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(logical_not_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.expression->accept(*this);
    auto operand = m_current_result;
    m_current_result = std::make_shared<boolean_object_t>(!operand->is_truthy());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(unary_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = 1; // Operator token is usually 1 char

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
        auto type_name = operand->type()->name();
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
            is_truthy = int_obj->value() != 0;
        }
        else if (type_name == "string")
        {
            auto str_obj = std::static_pointer_cast<string_object_t>(operand);
            is_truthy = !str_obj->value().empty();
        }

        m_current_result = std::make_shared<boolean_object_t>(!is_truthy);
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(index_access_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.object->accept(*this);
    auto list_obj = m_current_result;
    node.index->accept(*this);
    auto index_obj = m_current_result;

    m_current_result = list_obj->item(index_obj);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(optional_index_access_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.object->accept(*this);
    auto list_obj = m_current_result;

    if (list_obj->type()->name() == "none")
    {
        m_current_result = std::make_shared<none_object_t>();
        zephyr::current_error_location() = saved_location; // Restore before returning
        return;
    }

    node.index->accept(*this);
    auto index_obj = m_current_result;

    try {
        m_current_result = list_obj->item(index_obj);
    }
    catch (const std::runtime_error& e)
    {
        // For optional access, return none instead of error
        m_current_result = std::make_shared<none_object_t>();
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(optional_member_access_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.object->accept(*this);
    auto obj = m_current_result;

    if (obj->type()->name() == "none")
    {
        m_current_result = std::make_shared<none_object_t>();
        zephyr::current_error_location() = saved_location; // Restore before returning
        return;
    }

    try {
        m_current_result = obj->type()->member(obj, node.member_name);
    } catch (const std::runtime_error& e) {
        // For optional access, try object-based member access, but return none if it fails
        try {
            m_current_result = obj->member(node.member_name);
        } catch (const std::runtime_error& member_error) {
            // For optional chaining, member not found should return none
            m_current_result = std::make_shared<none_object_t>();
        }
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(optional_method_call_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.object->accept(*this);
    auto obj = m_current_result;

    if (obj->type()->name() == "none")
    {
        m_current_result = std::make_shared<none_object_t>();
        zephyr::current_error_location() = saved_location; // Restore before returning
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
            zephyr::current_error_location() = saved_location; // Restore before returning
            return;
        }

        auto method = class_instance->method(node.method_name);
        if (!method)
        {
            m_current_result = std::make_shared<none_object_t>();
            zephyr::current_error_location() = saved_location; // Restore before returning
            return;
        }

        // Check parameter count
        if (args.size() != method->parameters.size())
        {
            // For optional calls, parameter mismatch returns none instead of error
            m_current_result = std::make_shared<none_object_t>();
            zephyr::current_error_location() = saved_location; // Restore before returning
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
                if (param.has_explicit_type && args[i]->type()->name() != "none")
                {
                    std::string expected_type = param.type_name;
                    std::string actual_type = args[i]->type()->name();

                    // Normalize type names for common aliases
                    if (expected_type == "int" && actual_type == "number") {
                        actual_type = "int";
                    } else if (expected_type == "float" && actual_type == "number") {
                        // Allow int to float conversion
                        args[i] = std::make_shared<float_object_t>(
                            static_cast<double>(std::static_pointer_cast<int_object_t>(args[i])->value())
                        );
                        actual_type = "float";
                    } else if (expected_type == "string" && actual_type == "string_literal") {
                        actual_type = "string";
                    }

                    if (actual_type != expected_type)
                    {
                        // For optional calls, type mismatch returns none instead of error
                        m_current_result = std::make_shared<none_object_t>();
                        zephyr::current_error_location() = saved_location; // Restore before returning
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

        zephyr::current_error_location() = saved_location; // Restore before returning
        return;  // Important: return here for class instances, don't fall through
    }

    // Fallback to regular method call for other object types with error protection
    try {
        m_current_result = obj->call_method(node.method_name, args);
    } catch (const std::exception& e) {
        // For optional calls, any error returns none
        m_current_result = std::make_shared<none_object_t>();
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(ternary_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    // Check if variable is const before assignment
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name);
    }

    node.value->accept(*this);
    auto value = m_current_result;

    // Validate type constraint before assignment
    validate_type_constraint(node.variable_name, value);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(const_declaration_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    node.value->accept(*this);
    auto value = m_current_result;

    // Mark as const for protection
    m_const_variables.insert(node.variable_name);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(typed_declaration_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    node.value->accept(*this);
    auto value = m_current_result;

    // Store the type constraint
    m_type_constraints[node.variable_name] = node.type_name;

    // Validate the initial assignment against the type constraint
    validate_type_constraint(node.variable_name, value);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = value;
    m_current_result = value;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(typed_const_declaration_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(empty_declaration_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = std::make_shared<none_object_t>();
    m_current_result = std::make_shared<none_object_t>();
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(empty_typed_declaration_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    // Store the type constraint
    m_type_constraints[node.variable_name] = node.type_name;

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = std::make_shared<none_object_t>();
    m_current_result = std::make_shared<none_object_t>();
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(member_variable_declaration_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    if (node.value)
    {
        node.value->accept(*this);
    }
    else
    {
        m_current_result = std::make_shared<none_object_t>();
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(compound_assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    // Check if variable is const before assignment
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name);
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
            else if (node.operator_token == "**=") // Added for **= operator
            {
                result_value = current_value->power(new_value);
            }

            // Validate type constraint before assignment
            validate_type_constraint(node.variable_name, result_value);

            scope[node.variable_name] = result_value;
            m_current_result = result_value;
            zephyr::current_error_location() = saved_location; // Restore before returning
            return;
        }
    }
    throw name_error_t("Undefined variable: " + node.variable_name);
    zephyr::current_error_location() = saved_location; // Restore after throwing (unreachable, but good practice)
}

auto interpreter_t::visit(compound_member_assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    if (auto name_node = dynamic_cast<name_t*>(node.object.get())) {
        if (m_const_variables.count(name_node->name)) {
            throw type_error_t("Cannot modify member of a const variable: " + name_node->name);
        }
    }

    node.object->accept(*this);
    auto object = m_current_result;
    node.value->accept(*this);
    auto new_value = m_current_result;

    // Get the current value of the member element
    auto current_value = object->member(node.member_name);

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
    else if (node.operator_token == "**=")
    {
        result_value = current_value->power(new_value);
    }
    else
    {
        throw internal_error_t("Unknown compound member assignment operator: " + node.operator_token);
    }

    // Set the updated value back to the member element
    object->member(node.member_name, result_value);
    m_current_result = result_value;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(compound_indexed_assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    if (auto name_node = dynamic_cast<name_t*>(node.object.get())) {
        if (m_const_variables.count(name_node->name)) {
            throw type_error_t("Cannot modify element of a const variable: " + name_node->name);
        }
    }

    node.object->accept(*this);
    auto object = m_current_result;
    node.index->accept(*this);
    auto index = m_current_result;
    node.value->accept(*this);
    auto new_value = m_current_result;

    // Get the current value of the indexed element
    auto current_value = object->item(index);

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
    else if (node.operator_token == "**=")
    {
        result_value = current_value->power(new_value);
    }
    else
    {
        throw internal_error_t("Unknown compound indexed assignment operator: " + node.operator_token);
    }

    // Set the updated value back to the indexed element
    object->item(index, result_value);
    m_current_result = result_value;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(increment_decrement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    // Check if variable is const before increment/decrement
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name);
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
        throw name_error_t("Undefined variable: " + node.variable_name);
    }

    auto current_value = *var_ptr;

    // Check if it's a numeric type
    auto int_obj = std::dynamic_pointer_cast<int_object_t>(current_value);
    auto float_obj = std::dynamic_pointer_cast<float_object_t>(current_value);

    if (int_obj)
    {
        int new_value = int_obj->value();
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
        double new_value = float_obj->value();
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
        throw type_error_t("Cannot apply increment/decrement to non-numeric type");
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(increment_decrement_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.variable_name.length();

    // Check if variable is const before increment/decrement
    if (m_const_variables.find(node.variable_name) != m_const_variables.end())
    {
        throw type_error_t("Cannot assign to const variable: " + node.variable_name);
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
        throw name_error_t("Undefined variable: " + node.variable_name);
    }

    auto current_value = *var_ptr;

    // Check if it's a numeric type
    auto int_obj = std::dynamic_pointer_cast<int_object_t>(current_value);
    auto float_obj = std::dynamic_pointer_cast<float_object_t>(current_value);

    if (int_obj)
    {
        int old_value = int_obj->value();
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
        double old_value = float_obj->value();
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
        throw type_error_t("Cannot apply increment/decrement to non-numeric type");
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(member_increment_decrement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Placeholder implementation
    m_current_result = std::make_shared<none_object_t>();
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(indexed_increment_decrement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Placeholder implementation
    m_current_result = std::make_shared<none_object_t>();
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(indexed_assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    if (auto name_node = dynamic_cast<name_t*>(node.object.get())) {
        if (m_const_variables.count(name_node->name)) {
            throw type_error_t("Cannot modify element of a const variable: " + name_node->name);
        }
    }

    node.object->accept(*this);
    auto list_obj = m_current_result;
    node.index->accept(*this);
    auto index_obj = m_current_result;
    node.value->accept(*this);
    auto value_obj = m_current_result;

    list_obj->item(index_obj, value_obj);
    m_current_result = value_obj;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(expression_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.expression->accept(*this);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(block_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(if_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.condition->accept(*this);
    auto condition = m_current_result;

    if (condition->type()->is_truthy(condition))
    {
        node.then_block->accept(*this);
    }
    else if (node.else_block)
    {
        node.else_block->accept(*this);
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(while_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(do_while_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(do_until_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(for_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(for_each_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.collection->accept(*this);
    auto collection = m_current_result;

    m_scope_stack.push_back(std::map<std::string, value_t>());

    try
    {
        auto list_obj = std::dynamic_pointer_cast<list_object_t>(collection);
        if (list_obj)
        {
            for (auto& element : list_obj->elements())
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
                    if (element_list && element_list->elements().size() >= node.variables.size())
                    {
                        for (size_t i = 0; i < node.variables.size(); ++i)
                        {
                            current_scope[node.variables[i].name] = element_list->elements()[i];
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
            for (const auto& pair : dict_obj->elements())
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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(loop_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(function_definition_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.function_name.length();

    // Note: function_object_t constructor expects a unique_ptr to body
    auto func_obj = std::make_shared<function_object_t>(node.parameters, std::unique_ptr<block_t>(static_cast<block_t*>(node.body->clone().release())), node.return_type_name, node.explicit_return_type, node.async);
    auto& current_scope = m_scope_stack.back();
    current_scope[node.function_name] = func_obj;
    m_current_result = func_obj;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(function_call_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.function_name.length();

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
        throw name_error_t("Undefined function: " + node.function_name);
    }

    std::vector<std::shared_ptr<object_t>> args;
    for (auto& arg : node.arguments)
    {
        arg->accept(*this);
        args.push_back(m_current_result);
    }

    // Check if it's a builtin function
    if (func_obj->type()->name() == "builtin_function")
    {
        m_current_result = func_obj->call(args);
        zephyr::current_error_location() = saved_location;
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
                                   std::to_string(args.size()));
        }

        // Create new scope for function
        std::map<std::string, value_t> function_scope;

        // Bind parameters to arguments
        for (size_t i = 0; i < func->m_parameters.size(); ++i)
        {
            const auto& param = func->m_parameters[i];

            // Type checking for explicitly typed parameters
            if (param.has_explicit_type && args[i]->type()->name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->type()->name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    bool is_interface = false;
                    if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(args[i])) {
                        auto class_obj = class_instance->m_class_obj;
                        for (const auto& interface_name : class_obj->interfaces()) {
                            if (interface_name == expected_type) {
                                is_interface = true;
                                break;
                            }
                        }
                    }
                    if (!is_interface) {
                        throw type_error_t("Type mismatch for parameter '" + param.name +
                                               "': expected " + param.type_name +
                                               ", got " + args[i]->type()->name());
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
        zephyr::current_error_location() = saved_location;
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
                                   std::to_string(args.size()));
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
            if (param.has_explicit_type && args[i]->type()->name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->type()->name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    throw type_error_t("Type mismatch for parameter '" + param.name +
                                           "': expected " + param.type_name +
                                           ", got " + args[i]->type()->name());
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
                    throw internal_error_t("Lambda body expression is null - possible AST corruption");
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
        zephyr::current_error_location() = saved_location;
        return;
    }

    // Handle class objects (class instantiation)
    auto class_obj = std::dynamic_pointer_cast<class_object_t>(func_obj);
    if (class_obj)
    {
        if (class_obj->m_has_invalid_init) {
            throw type_error_t("init method cannot return a value.");
        }

        // Create a new instance of this class
        auto instance = std::make_shared<class_instance_t>(class_obj);

        // Check if the class has an 'init' method
        if (class_obj->has_method("init"))
        {
            auto init_method = class_obj->method("init");
            if (init_method && contains_return_with_value(init_method->body.get())) {
                throw type_error_t("init method cannot return a value.");
            }

            if (init_method)
            {
                // Check parameter count
                if (args.size() != init_method->parameters.size())
                {
                    throw type_error_t("Constructor for class '" + class_obj->m_class_name + "' expects " +
                                           std::to_string(init_method->parameters.size()) + " arguments, got " +
                                           std::to_string(args.size()));
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
                    if (param.has_explicit_type && args[i]->type()->name() != "none")
                    {
                        std::string expected_type = param.type_name;
                        std::string actual_type = args[i]->type()->name();

                        // Normalize type names for common aliases
                        if (expected_type == "dict") expected_type = "dictionary";
                        if (actual_type == "dict") actual_type = "dictionary";

                        if (actual_type != expected_type)
                        {
                            throw type_error_t("Type mismatch for parameter '" + param.name +
                                                   "': expected " + param.type_name +
                                                   ", got " + args[i]->type()->name());
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
                                    std::to_string(args.size()) + " arguments were provided");
        }

        // Return the created instance
        m_current_result = instance;
        zephyr::current_error_location() = saved_location;
        return;
    }

    // If we get here, it's not a callable object
    throw type_error_t("Object is not callable");
}

auto interpreter_t::visit(return_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1; // Assuming return_statement_t has end_column

    if (node.return_value)
    {
        node.return_value->accept(*this);
        if (!m_expected_return_types.empty()) {
            std::string expected_type = m_expected_return_types.back();
            if (!expected_type.empty()) {
                std::string actual_type = m_current_result->type()->name();
                if (actual_type != expected_type) {
                    // Allow returning none from a function with a return type
                    if (actual_type == "none") {
                        throw return_value_t{m_current_result};
                    }
                    throw type_error_t("Type mismatch in return statement: expected " + expected_type + ", got " + actual_type);
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
                throw type_error_t("Type mismatch in return statement: expected " + expected_type + ", got none");
            }
        }
        throw return_value_t{std::make_shared<none_object_t>()};
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(break_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    throw break_exception_t{};
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(continue_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    throw continue_exception_t{};
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(try_catch_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(list_destructuring_assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.assigned_expression->accept(*this);
    auto list_obj = std::dynamic_pointer_cast<list_object_t>(m_current_result);

    if (!list_obj)
    {
        throw value_error_t("Cannot destructure non-list value");
    }

    auto& current_scope = m_scope_stack.back();
    const auto& elements = list_obj->elements();
    for (size_t i = 0; i < node.variable_names.size() && i < elements.size(); ++i)
    {
        current_scope[node.variable_names[i]] = elements[i];
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(switch_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
            auto left_type = switch_value->type()->name();
            auto right_type = case_value->type()->name();

            if (left_type == right_type)
            {
                if (left_type == "int")
                {
                    auto left_int = std::static_pointer_cast<int_object_t>(switch_value);
                    auto right_int = std::static_pointer_cast<int_object_t>(case_value);
                    is_equal = left_int->value() == right_int->value();
                }
                else if (left_type == "string")
                {
                    auto left_str = std::static_pointer_cast<string_object_t>(switch_value);
                    auto right_str = std::static_pointer_cast<string_object_t>(case_value);
                    is_equal = left_str->value() == right_str->value();
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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(case_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // This shouldn't be called directly - handled by switch_statement
    for (auto& stmt : node.statements)
    {
        stmt->accept(*this);
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(program_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(lambda_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

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
            throw internal_error_t("Lambda block body is null - AST reuse detected. Functions returning lambdas cannot be called multiple times.");
        }
        // Clone the block body to allow function reuse
        auto cloned_body = std::unique_ptr<block_t>(static_cast<block_t*>(node.body_block->clone().release()));
        std::shared_ptr<block_t> shared_body(cloned_body.release());
        auto lambda_obj = std::make_shared<lambda_object_t>(node.parameters, shared_body, node.return_type_name, node.explicit_return_type, captured_vars, node.async);
        m_current_result = lambda_obj;
    }
    else
    {
        if (!node.body_expression) {
            throw internal_error_t("Lambda expression body is null - AST reuse detected. Functions returning lambdas cannot be called multiple times.");
        }
        // Clone the expression to allow function reuse
        auto cloned_expr = std::unique_ptr<expression_t>(static_cast<expression_t*>(node.body_expression->clone().release()));
        std::shared_ptr<expression_t> shared_expr(cloned_expr.release());
        auto lambda_obj = std::make_shared<lambda_object_t>(node.parameters, shared_expr, node.return_type_name, node.explicit_return_type, captured_vars, node.async);
        m_current_result = lambda_obj;
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(class_definition_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.class_name.length();

    // Create a new class object
    auto class_obj = std::make_shared<class_object_t>(node.class_name);

    // Check and process interfaces
    for (const auto& interface_name : node.interfaces) {
        auto interface_obj_val = resolve_variable(interface_name);
        auto interface_obj = std::dynamic_pointer_cast<interface_object_t>(interface_obj_val);
        if (!interface_obj) {
            throw type_error_t("'" + interface_name + "' is not an interface.");
        }

        class_obj->add_interface(interface_name);

        for (const auto& interface_method : interface_obj->methods()) {
            bool method_found = false;
            for (const auto& class_method : node.methods) {
                if (class_method->function_name == interface_method.name) {
                    if (class_method->parameters.size() != interface_method.parameters.size()) {
                        throw type_error_t("Class '" + node.class_name + "' method '" + class_method->function_name + "' has different number of parameters than in interface '" + interface_name + "'.");
                    }
                    for (size_t i = 0; i < class_method->parameters.size(); ++i) {
                        const auto& class_param = class_method->parameters[i];
                        const auto& interface_param = interface_method.parameters[i];
                        if (class_param.type_name != interface_param.type_name) {
                            throw type_error_t("Class '" + node.class_name + "' method '" + class_method->function_name + "' parameter '" + class_param.name + "' has different type than in interface '" + interface_name + "'.");
                        }
                    }
                    method_found = true;
                    break;
                }
            }
            if (!method_found) {
                throw type_error_t("Class '" + node.class_name + "' does not implement method '" + interface_method.name + "' from interface '" + interface_name + "'.");
            }
        }
    }

    // Add all methods to the class
    for (auto& method : node.methods)
    {
        if (method->function_name == "init" && contains_return_with_value(method->body.get())) {
            class_obj->m_has_invalid_init = true;
        }
        auto cloned_method = method->clone();
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
            member_var->is_const
        );
        class_obj->add_member_variable(var_info);
    }

    // Store the class in the current scope
    auto& current_scope = m_scope_stack.back();
    current_scope[node.class_name] = class_obj;
    m_current_result = class_obj;
    zephyr::current_error_location() = saved_location;
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
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.interface_name.length();

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
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(method_call_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.method_name.length();

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
                                   class_instance->m_class_obj->m_class_name + "'");
        }

        auto method = class_instance->method(node.method_name);
        if (!method)
        {
            throw attribute_error_t("Method '" + node.method_name + "' is null");
        }

        // Check parameter count
        if (args.size() != method->parameters.size())
        {
            throw type_error_t("Method '" + node.method_name + "' expects " +
                                   std::to_string(method->parameters.size()) + " arguments, got " +
                                   std::to_string(args.size()));
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
            if (param.has_explicit_type && args[i]->type()->name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->type()->name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    bool is_interface = false;
                    if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(args[i])) {
                        auto class_obj = class_instance->m_class_obj;
                        for (const auto& interface_name : class_obj->interfaces()) {
                            if (interface_name == expected_type) {
                                is_interface = true;
                                break;
                            }
                        }
                    }
                    if (!is_interface) {
                        throw type_error_t("Type mismatch for parameter '" + param.name +
                                                   "': expected " + param.type_name +
                                                   ", got " + args[i]->type()->name());
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
        zephyr::current_error_location() = saved_location;
        return;
    }

    // If not a class instance, fall back to the default behavior
    m_current_result = obj->call_method(node.method_name, args);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(member_access_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.member_name.length();

    node.object->accept(*this);
    auto obj = m_current_result;
    m_current_result = obj->type()->member(obj, node.member_name);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(this_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Find 'this' in scope
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find("this") != scope.end())
        {
            m_current_result = scope["this"];
            zephyr::current_error_location() = saved_location; // Restore before returning
            return;
        }
    }
    throw name_error_t("'this' not available in current context");
    zephyr::current_error_location() = saved_location; // Restore after throwing (unreachable, but good practice)
}

auto interpreter_t::visit(member_assignment_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.member_name.length();

    if (auto name_node = dynamic_cast<name_t*>(node.object.get())) {
        if (m_const_variables.count(name_node->name)) {
            throw type_error_t("Cannot modify member of a const variable: " + name_node->name);
        }
    }

    node.object->accept(*this);
    auto obj = m_current_result;

    if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(obj)) {
        if (class_instance->is_member_const(node.member_name)) {
            throw type_error_t("Cannot assign to const member: '" + node.member_name + "'");
        }
    }

    node.value->accept(*this);
    auto value = m_current_result;

    obj->type()->member(obj, node.member_name, value);
    m_current_result = value;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(in_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.element->accept(*this);
    auto left = m_current_result;
    node.container_expression->accept(*this);
    auto right = m_current_result;

    bool result = false;
    auto container_type = right->type()->name();

    if (container_type == "list")
    {
        auto list_obj = std::static_pointer_cast<list_object_t>(right);
        const auto& elements = list_obj->elements();
        for (const auto& element : elements)
        {
            // Check if element equals left
            auto left_type = left->type()->name();
            auto elem_type = element->type()->name();

            if (left_type == elem_type)
            {
                bool is_equal = false;
                if (left_type == "int")
                {
                    auto left_int = std::static_pointer_cast<int_object_t>(left);
                    auto elem_int = std::static_pointer_cast<int_object_t>(element);
                    is_equal = left_int->value() == elem_int->value();
                }
                else if (left_type == "string")
                {
                    auto left_str = std::static_pointer_cast<string_object_t>(left);
                    auto elem_str = std::static_pointer_cast<string_object_t>(element);
                    is_equal = left_str->value() == elem_str->value();
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
        result = str_obj->value().find(search_str->value()) != std::string::npos;
    }
    else if (container_type == "dictionary")
    {
        auto dict_obj = std::static_pointer_cast<dictionary_object_t>(right);
        if (left->type()->name() == "string")
        {
            auto key_str = std::static_pointer_cast<string_object_t>(left);
            const auto& elements = dict_obj->elements();
            result = elements.find(key_str->value()) != elements.end();
        }
    }
    m_current_result = std::make_shared<boolean_object_t>(result);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(await_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.expression->accept(*this);
    auto promise_obj = std::dynamic_pointer_cast<promise_object_t>(m_current_result);

    if (promise_obj)
    {
        m_current_result = promise_obj->m_result;
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::push_scope(const std::map<std::string, value_t>& scope) -> void
{
    m_scope_stack.push_back(scope);
}

auto interpreter_t::pop_scope() -> std::map<std::string, value_t>
{
    if (m_scope_stack.empty()) {
        throw internal_error_t("Cannot pop from empty scope stack");
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
        throw internal_error_t("Cannot exit global scope");
    }
    m_scope_stack.pop_back();
}

auto interpreter_t::visit(spawn_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Create a promise for the spawned expression
    auto promise_obj = std::make_shared<promise_object_t>();

    // For now, execute immediately (placeholder for async execution)
    node.expression->accept(*this);

    // Properly resolve the promise with the result
    promise_obj->resolve(m_current_result);

    m_current_result = promise_obj;
    zephyr::current_error_location() = saved_location;
}

// New bitwise visit methods start here
auto interpreter_t::visit(bitwise_and_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    if (left->type()->name() != "int" || right->type()->name() != "int") {
        throw type_error_t("Bitwise AND operator can only be applied to integers.");
    }

    auto left_int = std::static_pointer_cast<int_object_t>(left);
    auto right_int = std::static_pointer_cast<int_object_t>(right);

    m_current_result = std::make_shared<int_object_t>(left_int->value() & right_int->value());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(bitwise_or_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    if (left->type()->name() != "int" || right->type()->name() != "int") {
        throw type_error_t("Bitwise OR operator can only be applied to integers.");
    }

    auto left_int = std::static_pointer_cast<int_object_t>(left);
    auto right_int = std::static_pointer_cast<int_object_t>(right);

    m_current_result = std::make_shared<int_object_t>(left_int->value() | right_int->value());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(bitwise_xor_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    if (left->type()->name() != "int" || right->type()->name() != "int") {
        throw type_error_t("Bitwise XOR operator can only be applied to integers.");
    }

    auto left_int = std::static_pointer_cast<int_object_t>(left);
    auto right_int = std::static_pointer_cast<int_object_t>(right);

    m_current_result = std::make_shared<int_object_t>(left_int->value() ^ right_int->value());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(bitwise_not_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.expression->accept(*this);
    auto operand = m_current_result;

    if (operand->type()->name() != "int") {
        throw type_error_t("Bitwise NOT operator can only be applied to integers.");
    }

    auto int_obj = std::static_pointer_cast<int_object_t>(operand);

    m_current_result = std::make_shared<int_object_t>(~int_obj->value());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(left_shift_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    if (left->type()->name() != "int" || right->type()->name() != "int") {
        throw type_error_t("Bitwise left shift operator can only be applied to integers.");
    }

    auto left_int = std::static_pointer_cast<int_object_t>(left);
    auto right_int = std::static_pointer_cast<int_object_t>(right);

    if (right_int->value() < 0) {
        throw value_error_t("Shift amount cannot be negative.");
    }

    m_current_result = std::make_shared<int_object_t>(left_int->value() << right_int->value());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(right_shift_op_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.left->accept(*this);
    auto left = m_current_result;
    node.right->accept(*this);
    auto right = m_current_result;

    if (left->type()->name() != "int" || right->type()->name() != "int") {
        throw type_error_t("Bitwise right shift operator can only be applied to integers.");
    }

    auto left_int = std::static_pointer_cast<int_object_t>(left);
    auto right_int = std::static_pointer_cast<int_object_t>(right);

    if (right_int->value() < 0) {
        throw value_error_t("Shift amount cannot be negative.");
    }

    m_current_result = std::make_shared<int_object_t>(left_int->value() >> right_int->value());
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::validate_type_constraint(const std::string& variable_name, value_t value) -> void
{
    // Check if the variable has a type constraint
    auto constraint_it = m_type_constraints.find(variable_name);
    if (constraint_it != m_type_constraints.end())
    {
        const std::string& expected_type = constraint_it->second;
        const std::string& actual_type = value->type()->name();

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
            for (const auto& interface_name : class_obj->interfaces()) {
                if (interface_name == expected_type) {
                    return; // Type matches interface
                }
            }
        }

        // Check if normalized types match
        if (normalized_actual != normalized_expected)
        {
            throw type_error_t("Type error: Cannot assign " + actual_type + " to " + expected_type + " variable '" + variable_name + "'");
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
    throw name_error_t("Undefined variable: " + variable_name);
}

auto interpreter_t::variable(const std::string& variable_name, value_t value) -> void
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