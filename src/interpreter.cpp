#include "zephyr/interpreter.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/function_overload_resolver.hpp"
#include "zephyr/module_loader.hpp"
#include "zephyr/error_context.hpp"

#include "zephyr/errors.hpp"
#include "zephyr/objects/function_object.hpp"
#include "zephyr/objects/lambda_object.hpp"
#include "zephyr/objects/class_object.hpp"
#include "zephyr/objects/interface_object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include "zephyr/async_scheduler.hpp"
#include "zephyr/errors.hpp"
#include "zephyr/types/type.hpp"
#include <set>
#include <limits>
#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <memory>
#include <cmath>
#include "zephyr/objects/module_object.hpp"
#include "zephyr/objects/file_object.hpp"
#include "zephyr/objects/function_object.hpp"

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
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        // Convert any integer kind to regular int
        int64_t value = int_obj->value_64();
        if (value < INT32_MIN || value > INT32_MAX) {
            throw overflow_error_t("Integer value " + std::to_string(value) + " out of range for int");
        }
        return int_object_t::create_int(static_cast<int>(value));
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

    // Special handling for unified integer objects to return correct type names
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0])) {
        return std::make_shared<string_object_t>(int_obj->type_name());
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

// Sized integer builtin functions
auto i8_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("i8() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "i8") {
        return obj; // Already an i8
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < -128 || value > 127) {
            throw overflow_error_t("Value " + std::to_string(value) + " out of range for i8");
        }
        return int_object_t::create_i8(static_cast<int8_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            if (value < -128 || value > 127) {
                throw overflow_error_t("Value " + std::to_string(value) + " out of range for i8");
            }
            return int_object_t::create_i8(static_cast<int8_t>(value));
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for i8(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("i8() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto u8_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("u8() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "u8") {
        return obj; // Already a u8
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < 0 || value > 255) {
            throw overflow_error_t("Value " + std::to_string(value) + " out of range for u8");
        }
        return int_object_t::create_u8(static_cast<uint8_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            if (value < 0 || value > 255) {
                throw overflow_error_t("Value " + std::to_string(value) + " out of range for u8");
            }
            return int_object_t::create_u8(static_cast<uint8_t>(value));
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for u8(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("u8() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto i16_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("i16() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "i16") {
        return obj; // Already an i16
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < -32768 || value > 32767) {
            throw overflow_error_t("Value " + std::to_string(value) + " out of range for i16");
        }
        return int_object_t::create_i16(static_cast<int16_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            if (value < -32768 || value > 32767) {
                throw overflow_error_t("Value " + std::to_string(value) + " out of range for i16");
            }
            return int_object_t::create_i16(static_cast<int16_t>(value));
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for i16(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("i16() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto u16_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("u16() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "u16") {
        return obj; // Already a u16
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < 0 || value > 65535) {
            throw overflow_error_t("Value " + std::to_string(value) + " out of range for u16");
        }
        return int_object_t::create_u16(static_cast<uint16_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            if (value < 0 || value > 65535) {
                throw overflow_error_t("Value " + std::to_string(value) + " out of range for u16");
            }
            return int_object_t::create_u16(static_cast<uint16_t>(value));
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for u16(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("u16() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto i32_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("i32() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "i32") {
        return obj; // Already an i32
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < INT32_MIN || value > INT32_MAX) {
            throw overflow_error_t("Value " + std::to_string(value) + " out of range for i32");
        }
        return int_object_t::create_i32(static_cast<int32_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            if (value < INT32_MIN || value > INT32_MAX) {
                throw overflow_error_t("Value " + std::to_string(value) + " out of range for i32");
            }
            return int_object_t::create_i32(static_cast<int32_t>(value));
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for i32(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("i32() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto u32_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("u32() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "u32") {
        return obj; // Already a u32
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < 0) {
            throw overflow_error_t("Negative value " + std::to_string(value) + " cannot convert to u32");
        }
        if (value > UINT32_MAX) {
            throw overflow_error_t("Value " + std::to_string(value) + " out of range for u32");
        }
        return int_object_t::create_u32(static_cast<uint32_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            if (value < 0 || value > UINT32_MAX) {
                throw overflow_error_t("Value " + std::to_string(value) + " out of range for u32");
            }
            return int_object_t::create_u32(static_cast<uint32_t>(value));
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for u32(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("u32() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto i64_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("i64() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "i64") {
        return obj; // Already an i64
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        return int_object_t::create_i64(int_obj->value_64());
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            int64_t value = std::stoll(str_obj->value());
            return int_object_t::create_i64(value);
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for i64(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("i64() argument must be a string or integer, not '" + type_name + "'");
    }
}

auto u64_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>
{
    if (args.size() != 1) {
        throw type_error_t("u64() takes exactly one argument (" + std::to_string(args.size()) + " given)");
    }

    auto obj = args[0];
    auto type_name = obj->type()->name();

    if (type_name == "u64") {
        return obj; // Already a u64
    } else if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(obj)) {
        int64_t value = int_obj->value_64();
        if (value < 0) {
            throw overflow_error_t("Negative value " + std::to_string(value) + " cannot convert to u64");
        }
        return int_object_t::create_u64(static_cast<uint64_t>(value));
    } else if (type_name == "string") {
        auto str_obj = std::static_pointer_cast<string_object_t>(obj);
        try {
            uint64_t value = std::stoull(str_obj->value());
            return int_object_t::create_u64(value);
        } catch (const std::exception&) {
            throw value_error_t("invalid literal for u64(): '" + str_obj->value() + "'");
        }
    } else {
        throw type_error_t("u64() argument must be a string or integer, not '" + type_name + "'");
    }
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
        s_builtin_functions["open"] = std::make_shared<builtin_function_object_t>(open_builtin, "open");
        
        // Sized integer conversion functions
        s_builtin_functions["i8"] = std::make_shared<builtin_function_object_t>(i8_builtin, "i8");
        s_builtin_functions["u8"] = std::make_shared<builtin_function_object_t>(u8_builtin, "u8");
        s_builtin_functions["i16"] = std::make_shared<builtin_function_object_t>(i16_builtin, "i16");
        s_builtin_functions["u16"] = std::make_shared<builtin_function_object_t>(u16_builtin, "u16");
        s_builtin_functions["i32"] = std::make_shared<builtin_function_object_t>(i32_builtin, "i32");
        s_builtin_functions["u32"] = std::make_shared<builtin_function_object_t>(u32_builtin, "u32");
        s_builtin_functions["i64"] = std::make_shared<builtin_function_object_t>(i64_builtin, "i64");
        s_builtin_functions["u64"] = std::make_shared<builtin_function_object_t>(u64_builtin, "u64");
        
        s_builtins_initialized = true;
    }

    m_global_scope = s_builtin_functions;
    m_scope_stack.push_back(m_global_scope);
    
    // Initialize function resolvers with global scope
    m_function_resolvers.emplace_back();
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

auto interpreter_t::visit(sized_number_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;
    
    try {
        m_current_result = std::make_shared<int_object_t>(std::to_string(node.value), node.suffix);
    } catch (const std::exception& e) {
        // Convert any construction errors to runtime errors with proper location
        throw value_error_t(e.what());
    }
    
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

    // Delegate to the type system for proper power operation handling
    try {
        m_current_result = left->power(right);
    } catch (const type_error_t& e) {
        // If type doesn't support power operation, try the fallback mixed arithmetic
        auto left_type = left->type()->name();
        auto right_type = right->type()->name();
        
        if ((left_type == "int" || left_type == "float") && (right_type == "int" || right_type == "float"))
        {
            double left_val = (left_type == "int") ? static_cast<double>(std::static_pointer_cast<int_object_t>(left)->value()) : std::static_pointer_cast<float_object_t>(left)->value();
            double right_val = (right_type == "int") ? static_cast<double>(std::static_pointer_cast<int_object_t>(right)->value()) : std::static_pointer_cast<float_object_t>(right)->value();
            m_current_result = std::make_shared<float_object_t>(std::pow(left_val, right_val));
        }
        else
        {
            throw type_error_t("Unsupported operand types for **: " + left_type + " and " + right_type);
        }
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
        // Use type's equals method for comparison
        result = left->type()->equals(left, right);
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
        // Use type's equals method and negate
        result = !left->type()->equals(left, right);
    }
    else if (node.operator_token == "<" || node.operator_token == "<=" ||
             node.operator_token == ">" || node.operator_token == ">=")
    {
        try
        {
            // Use type's compare method for comparison
            int compare_result = left->type()->compare(left, right);
            
            if (node.operator_token == "<") result = compare_result < 0;
            else if (node.operator_token == "<=") result = compare_result <= 0;
            else if (node.operator_token == ">") result = compare_result > 0;
            else if (node.operator_token == ">=") result = compare_result >= 0;
        }
        catch (const std::exception&)
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
        // Use the object's is_truthy() method which delegates to the type
        bool is_truthy = operand->is_truthy();
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

    m_current_result = list_obj->type()->item(list_obj, index_obj);
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(slice_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    node.object->accept(*this);
    auto obj = m_current_result;
    
    // Get slice parameters
    std::shared_ptr<object_t> start_obj = nullptr;
    std::shared_ptr<object_t> end_obj = nullptr;
    std::shared_ptr<object_t> step_obj = nullptr;
    
    if (node.start) {
        node.start->accept(*this);
        start_obj = m_current_result;
    }
    
    if (node.end) {
        node.end->accept(*this);
        end_obj = m_current_result;
    }
    
    if (node.step) {
        node.step->accept(*this);
        step_obj = m_current_result;
    }
    
    // Call the slice method on the object's type
    m_current_result = obj->type()->slice(obj, start_obj, end_obj, step_obj);
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
            m_function_resolvers.emplace_back(); // Add function resolver for method scope

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
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
            }

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
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
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
    
    // Convert value to the specified type if there's a type constraint
    auto constraint_it = m_type_constraints.find(node.variable_name);
    if (constraint_it != m_type_constraints.end()) {
        value = convert_value_to_type(value, constraint_it->second);
    }

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
    
    // Export the constant if it's not internal
    if (should_export(node.is_internal))
    {
        add_to_exports(node.variable_name, value);
    }
    
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
    
    // Convert value to the specified type
    auto converted_value = convert_value_to_type(value, node.type_name);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = converted_value;
    m_current_result = converted_value;
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
    
    // Convert value to the specified type
    auto converted_value = convert_value_to_type(value, node.type_name);

    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = converted_value;
    m_current_result = converted_value;
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
            
            // Convert value to the specified type if there's a type constraint
            auto constraint_it = m_type_constraints.find(node.variable_name);
            if (constraint_it != m_type_constraints.end()) {
                result_value = convert_value_to_type(result_value, constraint_it->second);
            }

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
        // Yield point for cooperative execution
        check_and_yield();
        
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

    // Yield point for cooperative execution
    check_and_yield();

    // Create new scope for loop
    m_scope_stack.push_back(std::map<std::string, value_t>());
    m_function_resolvers.emplace_back(); // Add function resolver for loop scope

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
                // Yield point in loop body
                check_and_yield();
                node.body->accept(*this);
            } catch (const break_exception_t&)
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
        if (!m_function_resolvers.empty()) {
            m_function_resolvers.pop_back();
        }
        throw;
    }

    m_scope_stack.pop_back();
    if (!m_function_resolvers.empty()) {
        m_function_resolvers.pop_back();
    }
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(for_each_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Yield point for cooperative execution
    check_and_yield();

    node.collection->accept(*this);
    auto collection = m_current_result;

    m_scope_stack.push_back(std::map<std::string, value_t>());
    m_function_resolvers.emplace_back(); // Add function resolver for for-each scope

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
        if (!m_function_resolvers.empty()) {
            m_function_resolvers.pop_back();
        }
        throw;
    }

    m_scope_stack.pop_back();
    if (!m_function_resolvers.empty()) {
        m_function_resolvers.pop_back();
    }
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
            // Yield point in for-each body
            check_and_yield();
            node.body->accept(*this);
        } catch (const break_exception_t&)
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
    
    // Add to function overload resolver
    auto& current_resolver = m_function_resolvers.back();
    current_resolver.add_overload(node.function_name, node.parameters, func_obj);
    
    // Also store in scope for backward compatibility (will be removed later)
    auto& current_scope = m_scope_stack.back();
    current_scope[node.function_name] = func_obj;
    
    // Export the function if it's not internal
    if (should_export(node.is_internal))
    {
        add_to_exports(node.function_name, func_obj);
    }
    
    m_current_result = func_obj;
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::visit(function_call_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.function_name.length();

    // Yield point for cooperative execution
    check_and_yield();

    // Evaluate arguments first
    std::vector<std::shared_ptr<object_t>> args;
    for (auto& arg : node.arguments)
    {
        arg->accept(*this);
        args.push_back(m_current_result);
    }

    // First try to resolve using overload resolvers for user-defined functions
    std::shared_ptr<function_object_t> func_obj = nullptr;
    bool found_in_overloads = false;
    
    for (int i = m_function_resolvers.size() - 1; i >= 0; --i)
    {
        auto& resolver = m_function_resolvers[i];
        if (resolver.has_function(node.function_name))
        {
            auto resolution_result = resolver.resolve_call(node.function_name, args);
            if (resolution_result.found_match)
            {
                func_obj = resolution_result.selected_function;
                found_in_overloads = true;
                break;
            }
            else
            {
                // If we found the function name but no matching overload, throw error
                throw type_error_t(resolution_result.error_message);
            }
        }
    }

    // If found in overload resolvers, use that function
    if (found_in_overloads && func_obj)
    {
        // Handle user-defined functions (existing logic continues below)
    }
    else
    {
        // Not found in overload resolvers, try other callable objects (classes, builtins, etc.)
        std::shared_ptr<object_t> callable_obj = nullptr;
        for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
        {
            auto& scope = *it;
            if (scope.find(node.function_name) != scope.end())
            {
                callable_obj = scope[node.function_name];
                break;
            }
        }

        if (!callable_obj)
        {
            throw name_error_t("Undefined function: " + node.function_name);
        }

        // Check if it's a builtin function
        if (callable_obj->type()->name() == "builtin_function")
        {
            m_current_result = callable_obj->call(args);
            zephyr::current_error_location() = saved_location;
            return;
        }

        // Check if it's a class object (for class instantiation)
        auto class_obj = std::dynamic_pointer_cast<class_object_t>(callable_obj);
        if (class_obj)
        {
            // Handle class instantiation logic here
            if (class_obj->m_has_invalid_init) {
                throw type_error_t("init method cannot return a value.");
            }
            
            // Check if trying to instantiate an abstract class
            if (class_obj->m_is_abstract) {
                throw type_error_t("Cannot instantiate abstract class '" + class_obj->m_class_name + "'.");
            }

            // Create a new instance of this class
            auto instance = std::make_shared<class_instance_t>(class_obj);

            // Check if the class has an 'init' method
            if (class_obj->has_method("init"))
            {
                // Use overload resolver to find the best matching init method
                auto resolution_result = class_obj->resolve_method_call("init", args);
                if (!resolution_result.found_match)
                {
                    throw attribute_error_t(resolution_result.error_message);
                }

                auto init_method = resolution_result.selected_function;

                // Create new scope for init method
                std::map<std::string, value_t> init_scope;

                // Add 'this' to the scope
                init_scope["this"] = instance;

                // Bind parameters to arguments
                for (size_t i = 0; i < init_method->m_parameters.size(); ++i)
                {
                    const auto& param = init_method->m_parameters[i];

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
                m_function_resolvers.emplace_back(); // Add function resolver for init scope

                try
                {
                    // Execute init method body
                    init_method->m_body->accept(*this);
                }
                catch (const return_value_t&)
                {
                    // init methods should not return values, but we catch it anyway
                    // The error is already thrown during method creation
                }

                // Pop init scope
                m_scope_stack.pop_back();
                if (!m_function_resolvers.empty()) {
                    m_function_resolvers.pop_back();
                }

                // Remove const parameters from tracking when leaving init scope
                for (const auto& param : init_method->m_parameters)
                {
                    if (param.is_const)
                    {
                        m_const_variables.erase(param.name);
                    }
                }
            }
            else
            {
                // No init method, check if args were provided
                if (!args.empty())
                {
                    throw type_error_t("Class '" + class_obj->m_class_name + "' constructor expects 0 arguments, got " +
                                       std::to_string(args.size()));
                }
            }

            m_current_result = instance;
            zephyr::current_error_location() = saved_location;
            return;
        }

        // Check if it's a lambda object
        auto lambda_obj = std::dynamic_pointer_cast<lambda_object_t>(callable_obj);
        if (lambda_obj)
        {
            // Handle lambda execution in the interpreter
            // Check parameter count
            if (args.size() != lambda_obj->m_parameters.size())
            {
                throw type_error_t("Lambda expects " +
                                   std::to_string(lambda_obj->m_parameters.size()) + " arguments, got " +
                                   std::to_string(args.size()));
            }

            // Create new scope for lambda
            std::map<std::string, value_t> lambda_scope;

            // Add captured variables
            for (const auto& captured : lambda_obj->m_captured_variables)
            {
                lambda_scope[captured.first] = captured.second;
            }

            // Bind parameters to arguments
            for (size_t i = 0; i < lambda_obj->m_parameters.size(); ++i)
            {
                const auto& param = lambda_obj->m_parameters[i];

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
            m_function_resolvers.emplace_back(); // Add function resolver for lambda scope

            try
            {
                if (lambda_obj->m_is_block_body)
                {
                    // Execute lambda block body
                    lambda_obj->m_body_block->accept(*this);
                    // If no return statement, return none
                    m_current_result = std::make_shared<none_object_t>();
                }
                else
                {
                    // Execute lambda expression body
                    lambda_obj->m_body_expression->accept(*this);
                    // Result is already in m_current_result
                }
            }
            catch (const return_value_t& return_val)
            {
                // Lambda returned a value
                m_current_result = return_val.value;
            }

            // Pop lambda scope
            m_scope_stack.pop_back();
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
            }

            // Remove const parameters from tracking when leaving lambda scope
            for (const auto& param : lambda_obj->m_parameters)
            {
                if (param.is_const)
                {
                    m_const_variables.erase(param.name);
                }
            }

            zephyr::current_error_location() = saved_location;
            return;
        }

        // If it's a function object, cast it and continue with user-defined function logic
        func_obj = std::dynamic_pointer_cast<function_object_t>(callable_obj);
        if (!func_obj)
        {
            throw type_error_t("Object '" + node.function_name + "' is not callable");
        }
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

            /**
             * Type checking and implicit conversion for explicitly typed parameters.
             * 
             * This section handles the conversion of function arguments to match
             * explicitly typed parameters. It supports:
             * 1. Exact type matches (no conversion needed)
             * 2. Interface compatibility for class instances
             * 3. Implicit conversions between compatible types (especially integers)
             * 
             * For integer types, the unified integer system allows seamless conversion
             * between all integer types (int, i8, u8, i16, u16, i32, u32, i64, u64).
             * This enables natural function calls like:
             *   func process(val: u8) { ... }
             *   process(42)  // int literal automatically converted to u8
             */
            if (param.has_explicit_type && args[i]->type()->name() != "none")
            {
                std::string expected_type = param.type_name;
                std::string actual_type = args[i]->type()->name();

                // Normalize type names for common aliases
                if (expected_type == "dict") expected_type = "dictionary";
                if (actual_type == "dict") actual_type = "dictionary";

                if (actual_type != expected_type)
                {
                    bool is_compatible = false;
                    
                    // Check for interface compatibility
                    if (auto class_instance = std::dynamic_pointer_cast<class_instance_t>(args[i])) {
                        auto class_obj = class_instance->m_class_obj;
                        for (const auto& interface_name : class_obj->interfaces()) {
                            if (interface_name == expected_type) {
                                is_compatible = true;
                                break;
                            }
                        }
                    }
                    
                    // Check for implicit conversion compatibility (especially for integer types)
                    // This uses the same conversion logic as the overload resolver to maintain consistency
                    if (!is_compatible && overload_utils::is_implicitly_convertible(actual_type, expected_type))
                    {
                        // Perform the conversion for integer types using the unified integer system
                        // All integer types can be converted through int_object_t with appropriate range handling
                        std::vector<std::string> integer_types = {"int", "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"};
                        auto from_is_int = std::find(integer_types.begin(), integer_types.end(), actual_type) != integer_types.end();
                        auto to_is_int = std::find(integer_types.begin(), integer_types.end(), expected_type) != integer_types.end();
                        
                        if (from_is_int && to_is_int) {
                            // Convert integer types using the unified integer system
                            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[i])) {
                                try {
                                    // Create new integer object with the expected type
                                    int64_t value = int_obj->value_64();
                                    
                                    // Create the appropriate integer type based on expected_type
                                    if (expected_type == "i8") {
                                        args[i] = int_object_t::create_i8(static_cast<int8_t>(value));
                                    } else if (expected_type == "u8") {
                                        args[i] = int_object_t::create_u8(static_cast<uint8_t>(value));
                                    } else if (expected_type == "i16") {
                                        args[i] = int_object_t::create_i16(static_cast<int16_t>(value));
                                    } else if (expected_type == "u16") {
                                        args[i] = int_object_t::create_u16(static_cast<uint16_t>(value));
                                    } else if (expected_type == "i32") {
                                        args[i] = int_object_t::create_i32(static_cast<int32_t>(value));
                                    } else if (expected_type == "u32") {
                                        args[i] = int_object_t::create_u32(static_cast<uint32_t>(value));
                                    } else if (expected_type == "i64") {
                                        args[i] = int_object_t::create_i64(value);
                                    } else if (expected_type == "u64") {
                                        args[i] = int_object_t::create_u64(static_cast<uint64_t>(value));
                                    } else if (expected_type == "int") {
                                        args[i] = std::make_shared<int_object_t>(static_cast<int>(value));
                                    }
                                    
                                    is_compatible = true;
                                } catch (const std::exception& e) {
                                    // Conversion failed (e.g., value out of range)
                                    throw type_error_t("Cannot convert " + actual_type + " value " + 
                                                     std::to_string(int_obj->value_64()) + " to " + expected_type + 
                                                     ": " + std::string(e.what()));
                                }
                            }
                        } else {
                            // For non-integer implicit conversions, mark as compatible
                            // The actual conversion logic would be handled elsewhere if needed
                            is_compatible = true;
                        }
                    }
                    
                    if (!is_compatible) {
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
        m_function_resolvers.emplace_back(); // Add function resolver for function scope
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
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
            }
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
        if (!m_function_resolvers.empty()) {
            m_function_resolvers.pop_back();
        }
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
        m_function_resolvers.emplace_back(); // Add function resolver for lambda scope

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
        if (!m_function_resolvers.empty()) {
            m_function_resolvers.pop_back();
        }

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
        
        // Check if trying to instantiate an abstract class
        if (class_obj->m_is_abstract) {
            throw type_error_t("Cannot instantiate abstract class '" + class_obj->m_class_name + "'.");
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
                m_function_resolvers.emplace_back(); // Add function resolver for init scope

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
                if (!m_function_resolvers.empty()) {
                    m_function_resolvers.pop_back();
                }

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

    /**
     * Function return type checking with implicit conversion support.
     * 
     * This section handles the validation and conversion of return values to match
     * explicitly declared return types. It supports:
     * 1. Exact type matches (no conversion needed)
     * 2. Special handling for 'none' returns
     * 3. Implicit conversions between compatible types (especially integers)
     * 
     * For integer types, expressions that return 'int' can be automatically
     * converted to any sized integer return type. This enables natural syntax like:
     *   func compute(a: i32, b: i32) : i32 {
     *       return a + b  // int expression automatically converted to i32
     *   }
     */
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
                    
                    // Check for implicit conversion compatibility (especially for integer types)
                    // This ensures consistent behavior with function parameter conversion
                    if (overload_utils::is_implicitly_convertible(actual_type, expected_type)) {
                        // Perform the conversion for integer types using the unified integer system
                        // Range validation and truncation happen here for out-of-bounds values
                        std::vector<std::string> integer_types = {"int", "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"};
                        auto from_is_int = std::find(integer_types.begin(), integer_types.end(), actual_type) != integer_types.end();
                        auto to_is_int = std::find(integer_types.begin(), integer_types.end(), expected_type) != integer_types.end();
                        
                        if (from_is_int && to_is_int) {
                            // Convert integer types using the unified integer system
                            if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(m_current_result)) {
                                try {
                                    // Create new integer object with the expected type
                                    int64_t value = int_obj->value_64();
                                    
                                    // Create the appropriate integer type based on expected_type
                                    if (expected_type == "i8") {
                                        m_current_result = int_object_t::create_i8(static_cast<int8_t>(value));
                                    } else if (expected_type == "u8") {
                                        m_current_result = int_object_t::create_u8(static_cast<uint8_t>(value));
                                    } else if (expected_type == "i16") {
                                        m_current_result = int_object_t::create_i16(static_cast<int16_t>(value));
                                    } else if (expected_type == "u16") {
                                        m_current_result = int_object_t::create_u16(static_cast<uint16_t>(value));
                                    } else if (expected_type == "i32") {
                                        m_current_result = int_object_t::create_i32(static_cast<int32_t>(value));
                                    } else if (expected_type == "u32") {
                                        m_current_result = int_object_t::create_u32(static_cast<uint32_t>(value));
                                    } else if (expected_type == "i64") {
                                        m_current_result = int_object_t::create_i64(value);
                                    } else if (expected_type == "u64") {
                                        m_current_result = int_object_t::create_u64(static_cast<uint64_t>(value));
                                    } else if (expected_type == "int") {
                                        m_current_result = std::make_shared<int_object_t>(static_cast<int>(value));
                                    }
                                } catch (const std::exception& e) {
                                    // Conversion failed (e.g., value out of range)
                                    throw type_error_t("Cannot convert return value " + actual_type + " (" + 
                                                     std::to_string(int_obj->value_64()) + ") to " + expected_type + 
                                                     ": " + std::string(e.what()));
                                }
                            }
                        }
                        // For non-integer implicit conversions, the conversion would be handled elsewhere if needed
                    } else {
                        throw type_error_t("Type mismatch in return statement: expected " + expected_type + ", got " + actual_type);
                    }
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

auto interpreter_t::visit(with_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Evaluate the context expression
    node.context_expression->accept(*this);
    auto context_obj = m_current_result;
    
    if (!context_obj) {
        throw value_error_t("with statement context expression evaluated to null");
    }
    
    // Call __enter__ method on the context object
    std::shared_ptr<object_t> entered_value = nullptr;
    try {
        entered_value = context_obj->__enter__();
    } catch (const std::exception& e) {
        throw value_error_t("Error in context manager __enter__: " + std::string(e.what()));
    }
    
    if (!entered_value) {
        // If __enter__ returns null, use the original object
        entered_value = context_obj;
    }
    
    // Create new scope for the with block
    m_scope_stack.emplace_back();
    
    // Bind the variable to the entered value
    auto& current_scope = m_scope_stack.back();
    current_scope[node.variable_name] = entered_value;
    
    // Exception handling variables
    std::shared_ptr<object_t> exception_type = nullptr;
    std::shared_ptr<object_t> exception_value = nullptr;
    std::shared_ptr<object_t> traceback = nullptr;
    bool exception_occurred = false;
    
    try {
        // Execute the block
        node.body->accept(*this);
    } catch (const value_error_t& e) {
        exception_occurred = true;
        exception_type = std::make_shared<string_object_t>("runtime_error");
        exception_value = std::make_shared<string_object_t>(e.what());
        
        // Pop scope before calling __exit__
        m_scope_stack.pop_back();
        
        // Call __exit__ with exception info
        bool suppress_exception = false;
        try {
            suppress_exception = context_obj->__exit__(exception_type, exception_value, traceback);
        } catch (const std::exception& exit_error) {
            // If __exit__ throws, that becomes the new exception
            throw value_error_t("Error in context manager __exit__: " + std::string(exit_error.what()));
        }
        
        zephyr::current_error_location() = saved_location;
        
        // Re-throw the original exception unless __exit__ suppressed it
        if (!suppress_exception) {
            throw;
        }
        
        return;  // Exception was suppressed
        
    } catch (const std::exception& e) {
        exception_occurred = true;
        exception_type = std::make_shared<string_object_t>("exception");
        exception_value = std::make_shared<string_object_t>(e.what());
        
        m_scope_stack.pop_back();
        
        bool suppress_exception = false;
        try {
            suppress_exception = context_obj->__exit__(exception_type, exception_value, traceback);
        } catch (const std::exception& exit_error) {
            throw value_error_t("Error in context manager __exit__: " + std::string(exit_error.what()));
        }
        
        zephyr::current_error_location() = saved_location;
        
        if (!suppress_exception) {
            throw;
        }
        
        return;
    }
    
    // Normal exit path - no exception occurred
    m_scope_stack.pop_back();
    
    try {
        context_obj->__exit__(exception_type, exception_value, traceback);
    } catch (const std::exception& e) {
        throw value_error_t("Error in context manager __exit__: " + std::string(e.what()));
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

    // Capture variables from all non-global scopes for proper closure support
    if (m_scope_stack.size() > 1) {
        // Iterate through all scopes from innermost to outermost (excluding global)
        for (int i = static_cast<int>(m_scope_stack.size()) - 1; i >= 1; --i) {
            const auto& scope = m_scope_stack[i];
            for (const auto& pair : scope) {
                // Only capture if not already captured (inner scope wins)
                if (captured_vars.find(pair.first) == captured_vars.end()) {
                    captured_vars[pair.first] = pair.second;
                }
            }
            // Continue to capture from all non-global scopes
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
    
    // Set class modifiers
    class_obj->m_is_final = node.is_final;
    class_obj->m_is_abstract = node.is_abstract;
    
    // Determine what is a class and what is an interface
    std::shared_ptr<class_object_t> parent_class_obj = nullptr;
    std::vector<std::string> actual_interfaces;
    
    // Handle parent class inheritance
    if (!node.parent_class.empty()) {
        auto parent_obj_val = resolve_variable(node.parent_class);
        auto parent_as_class = std::dynamic_pointer_cast<class_object_t>(parent_obj_val);
        auto parent_as_interface = std::dynamic_pointer_cast<interface_object_t>(parent_obj_val);
        
        if (parent_as_class) {
            // It's a class - use as parent
            if (parent_as_class->m_is_final) {
                throw type_error_t("Cannot inherit from final class '" + node.parent_class + "'.");
            }
            parent_class_obj = parent_as_class;
            class_obj->set_parent_class(parent_class_obj);
        } else if (parent_as_interface) {
            // It's an interface - add to interfaces list
            actual_interfaces.push_back(node.parent_class);
        } else {
            throw type_error_t("'" + node.parent_class + "' is neither a class nor an interface.");
        }
    }
    
    // Process additional inherited items (from comma-separated list)
    for (const auto& item_name : node.interfaces) {
        auto item_obj_val = resolve_variable(item_name);
        auto item_as_class = std::dynamic_pointer_cast<class_object_t>(item_obj_val);
        auto item_as_interface = std::dynamic_pointer_cast<interface_object_t>(item_obj_val);
        
        if (item_as_class) {
            // It's a class - check if we already have a parent class
            if (parent_class_obj) {
                throw type_error_t("Cannot inherit from multiple classes. Class '" + node.class_name + 
                                 "' already inherits from '" + parent_class_obj->m_class_name + 
                                 "' and cannot also inherit from '" + item_name + "'.");
            }
            if (item_as_class->m_is_final) {
                throw type_error_t("Cannot inherit from final class '" + item_name + "'.");
            }
            parent_class_obj = item_as_class;
            class_obj->set_parent_class(parent_class_obj);
        } else if (item_as_interface) {
            // It's an interface - add to interfaces list
            actual_interfaces.push_back(item_name);
        } else {
            throw type_error_t("'" + item_name + "' is neither a class nor an interface.");
        }
    }
    
    // If we have a parent class, inherit from it
    if (parent_class_obj) {
        
        // Inherit member variables from parent
        for (const auto& parent_var : parent_class_obj->m_member_variables) {
            // Check if child class redefines this member variable
            bool redefined = false;
            for (const auto& child_var : node.member_variables) {
                if (child_var->variable_name == parent_var.name) {
                    redefined = true;
                    break;
                }
            }
            if (!redefined) {
                class_obj->add_member_variable(parent_var);
            }
        }
        
        // Check abstract methods from parent are implemented
        if (parent_class_obj->m_is_abstract && !node.is_abstract) {
            for (const auto& [method_name, parent_method] : parent_class_obj->m_methods) {
                if (parent_method && parent_method->is_abstract) {
                    bool implemented = false;
                    for (const auto& child_method : node.methods) {
                        if (child_method->function_name == method_name && !child_method->is_abstract) {
                            implemented = true;
                            break;
                        }
                    }
                    if (!implemented) {
                        throw type_error_t("Class '" + node.class_name + "' must implement abstract method '" + 
                                         method_name + "' from parent class '" + parent_class_obj->m_class_name + "'.");
                    }
                }
            }
        }
    }

    // Check and process interfaces (now using the validated list)
    for (const auto& interface_name : actual_interfaces) {
        auto interface_obj_val = resolve_variable(interface_name);
        auto interface_obj = std::dynamic_pointer_cast<interface_object_t>(interface_obj_val);
        if (!interface_obj) {
            throw type_error_t("'" + interface_name + "' is not an interface.");
        }

        class_obj->add_interface(interface_name);

        for (const auto& interface_method : interface_obj->methods()) {
            bool method_found = false;
            
            // Look for an exact signature match in the class methods
            for (const auto& class_method : node.methods) {
                if (class_method->function_name == interface_method.name) {
                    // Check if this class method matches the interface method signature exactly
                    if (class_method->parameters.size() == interface_method.parameters.size()) {
                        bool signature_matches = true;
                        
                        for (size_t i = 0; i < class_method->parameters.size(); ++i) {
                            const auto& class_param = class_method->parameters[i];
                            const auto& interface_param = interface_method.parameters[i];
                            
                            // Compare parameter types (empty type means untyped parameter)
                            if (class_param.type_name != interface_param.type_name) {
                                signature_matches = false;
                                break;
                            }
                        }
                        
                        if (signature_matches) {
                            method_found = true;
                            break;
                        }
                    }
                }
            }
            
            if (!method_found) {
                // Generate a detailed error message with the expected signature
                std::string expected_signature = interface_method.name + "(";
                for (size_t i = 0; i < interface_method.parameters.size(); ++i) {
                    if (i > 0) expected_signature += ", ";
                    expected_signature += interface_method.parameters[i].name;
                    if (!interface_method.parameters[i].type_name.empty()) {
                        expected_signature += " : " + interface_method.parameters[i].type_name;
                    }
                }
                expected_signature += ")";
                
                throw type_error_t("Class '" + node.class_name + "' does not implement interface method '" + 
                                 expected_signature + "' from interface '" + interface_name + "'.");
            }
        }
    }

    // Add all methods to the class
    for (auto& method : node.methods)
    {
        if (method->function_name == "init" && method->body && contains_return_with_value(method->body.get())) {
            class_obj->m_has_invalid_init = true;
        }
        
        // Check that abstract methods don't have a body
        if (method->is_abstract && method->body) {
            throw syntax_error_t("Abstract method '" + method->function_name + "' cannot have a body.");
        }
        
        // Check that non-abstract methods have a body
        if (!method->is_abstract && !method->body) {
            throw syntax_error_t("Non-abstract method '" + method->function_name + "' must have a body.");
        }
        
        auto cloned_method = method->clone();
        class_obj->add_method(method->function_name, std::shared_ptr<function_definition_t>(static_cast<function_definition_t*>(cloned_method.release())));
    }
    
    // Check that non-abstract classes don't have abstract methods
    if (!node.is_abstract) {
        for (const auto& method : node.methods) {
            if (method->is_abstract) {
                throw type_error_t("Non-abstract class '" + node.class_name + 
                                 "' cannot have abstract method '" + method->function_name + "'.");
            }
        }
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
    
    // Export the class if it's not internal
    if (should_export(node.is_internal))
    {
        add_to_exports(node.class_name, class_obj);
    }
    
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
    
    // Check if this is a super call
    bool is_super = m_is_super_call;
    m_is_super_call = false; // Reset the flag

    std::vector<std::shared_ptr<object_t>> args;
    for (auto& arg : node.arguments)
    {
        arg->accept(*this);
        args.push_back(m_current_result);
    }

    // Handle module method calls as function calls
    auto module_obj = std::dynamic_pointer_cast<module_object_t>(obj);
    if (module_obj)
    {
        // Get the exported function from the module
        auto export_value = module_obj->get_module()->get_export(node.method_name);
        if (!export_value)
        {
            throw name_error_t("Module '" + module_obj->get_module_name() + "' has no export '" + node.method_name + "'");
        }
        
        // Check if it's a function object
        auto function_obj = std::dynamic_pointer_cast<function_object_t>(export_value);
        auto builtin_function_obj = std::dynamic_pointer_cast<builtin_function_object_t>(export_value);
        auto class_obj = std::dynamic_pointer_cast<class_object_t>(export_value);
        
        if (!function_obj && !builtin_function_obj && !class_obj)
        {
            throw type_error_t("'" + node.method_name + "' is not a function");
        }
        
        // Handle builtin functions from plugins
        if (builtin_function_obj)
        {
            m_current_result = builtin_function_obj->call(args);
            zephyr::current_error_location() = saved_location;
            return;
        }
        
        // Handle class instantiation from modules
        if (class_obj)
        {
            // Use the existing class instantiation logic from function_call_t
            if (class_obj->m_has_invalid_init) {
                throw type_error_t("init method cannot return a value.");
            }
            
            // Check if trying to instantiate an abstract class
            if (class_obj->m_is_abstract) {
                throw type_error_t("Cannot instantiate abstract class '" + class_obj->m_class_name + "'.");
            }

            // Create a new instance of this class
            auto instance = std::make_shared<class_instance_t>(class_obj);

            // Check if the class has an 'init' method
            if (class_obj->has_method("init"))
            {
                // Use overload resolver to find the best matching init method
                auto resolution_result = class_obj->resolve_method_call("init", args);
                if (!resolution_result.found_match)
                {
                    throw type_error_t("No matching 'init' method found for given arguments in class '" + class_obj->m_class_name + "'.\n" + resolution_result.error_message);
                }

                auto init_method = resolution_result.selected_function;

                // Store the current 'this' value (if any) to restore later
                value_t saved_this = nullptr;
                try {
                    saved_this = resolve_variable("this");
                } catch (...) {
                    // 'this' doesn't exist yet, that's fine
                }

                // Set up the method call environment
                enter_function_scope();
                variable("this", instance);

                // Set up parameters
                for (size_t i = 0; i < args.size(); ++i)
                {
                    variable(init_method->m_parameters[i].name, args[i]);
                }

                try
                {
                    // Execute the init method
                    init_method->m_body->accept(*this);
                    exit_function_scope();
                }
                catch (const std::exception& e)
                {
                    exit_function_scope();
                    throw;
                }

                // Restore the previous 'this' value
                if (saved_this)
                {
                    variable("this", saved_this);
                }
            }
            else if (!args.empty())
            {
                throw type_error_t("Class '" + class_obj->m_class_name + "' has no 'init' method, but arguments were provided.");
            }

            m_current_result = instance;
            zephyr::current_error_location() = saved_location;
            return;
        }
        
        // Execute the function directly using interpreter logic (similar to function_call_t)
        
        // Check parameter count
        if (args.size() != function_obj->m_parameters.size())
        {
            throw type_error_t("Function '" + node.method_name + "' expects " +
                               std::to_string(function_obj->m_parameters.size()) + " arguments, got " +
                               std::to_string(args.size()));
        }

        // Create new scope for function
        std::map<std::string, value_t> function_scope;

        // Bind parameters to arguments
        for (size_t i = 0; i < function_obj->m_parameters.size(); ++i)
        {
            const auto& param = function_obj->m_parameters[i];

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

            function_scope[param.name] = args[i];

            // Track const parameters
            if (param.is_const)
            {
                m_const_variables.insert(param.name);
            }
        }

        // Push module's global scope first (for accessing module-level variables)
        auto module_global_scope = module_obj->get_module()->get_global_scope();
        if (!module_global_scope.empty())
        {
            m_scope_stack.push_back(module_global_scope);
            m_function_resolvers.emplace_back(); // Add function resolver for module scope
        }
        
        // Push function scope
        m_scope_stack.push_back(function_scope);
        m_function_resolvers.emplace_back(); // Add function resolver for function scope
        m_expected_return_types.push_back(function_obj->m_return_type_name);

        try
        {
            // Execute function body
            function_obj->m_body->accept(*this);

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
            // Pop function scope and module global scope, then re-throw
            m_scope_stack.pop_back(); // function scope
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
            }
            if (!module_global_scope.empty())
            {
                m_scope_stack.pop_back(); // module global scope
                if (!m_function_resolvers.empty()) {
                    m_function_resolvers.pop_back();
                }
            }
            m_expected_return_types.pop_back();
            for (const auto& param : function_obj->m_parameters)
            {
                if (param.is_const)
                {
                    m_const_variables.erase(param.name);
                }
            }
            throw;
        }

        // Pop function scope and module global scope
        m_scope_stack.pop_back(); // function scope
        if (!m_function_resolvers.empty()) {
            m_function_resolvers.pop_back();
        }
        if (!module_global_scope.empty())
        {
            m_scope_stack.pop_back(); // module global scope
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
            }
        }
        m_expected_return_types.pop_back();

        // Remove const parameters from tracking when leaving function scope
        for (const auto& param : function_obj->m_parameters)
        {
            if (param.is_const)
            {
                m_const_variables.erase(param.name);
            }
        }
        
        zephyr::current_error_location() = saved_location;
        return;
    }

    // Handle class instance method calls
    auto class_instance = std::dynamic_pointer_cast<class_instance_t>(obj);
    if (class_instance)
    {
        // For super calls, use the parent class's methods
        std::shared_ptr<class_object_t> class_to_use = class_instance->m_class_obj;
        if (is_super)
        {
            class_to_use = class_instance->m_class_obj->parent_class();
            if (!class_to_use)
            {
                throw type_error_t("Class has no parent class for super call");
            }
        }
        
        // Use overload resolver to find the best matching method
        auto resolution_result = class_to_use->resolve_method_call(node.method_name, args);
        if (!resolution_result.found_match)
        {
            throw attribute_error_t(resolution_result.error_message);
        }

        auto method_func = resolution_result.selected_function;
        if (!method_func)
        {
            throw attribute_error_t("Method '" + node.method_name + "' resolution returned null function");
        }

        // Create new scope for method
        std::map<std::string, value_t> method_scope;

        // Add 'this' to the scope
        method_scope["this"] = class_instance;

        // Bind parameters to arguments using the selected function's parameters
        for (size_t i = 0; i < method_func->m_parameters.size(); ++i)
        {
            const auto& param = method_func->m_parameters[i];

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
        m_function_resolvers.emplace_back(); // Add function resolver for method scope
        m_expected_return_types.push_back(method_func->m_return_type_name);

        try
        {
            // Execute method body
            method_func->m_body->accept(*this);

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
            if (!m_function_resolvers.empty()) {
                m_function_resolvers.pop_back();
            }
            m_expected_return_types.pop_back();
            for (const auto& param : method_func->m_parameters)
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
        if (!m_function_resolvers.empty()) {
            m_function_resolvers.pop_back();
        }
        m_expected_return_types.pop_back();

        // Remove const parameters from tracking when leaving method scope
        for (const auto& param : method_func->m_parameters)
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

auto interpreter_t::visit(super_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Find 'this' in scope to get the current instance
    std::shared_ptr<class_instance_t> current_instance;
    for (auto it = m_scope_stack.rbegin(); it != m_scope_stack.rend(); ++it)
    {
        auto& scope = *it;
        if (scope.find("this") != scope.end())
        {
            current_instance = std::dynamic_pointer_cast<class_instance_t>(scope["this"]);
            if (current_instance)
            {
                break;
            }
        }
    }
    
    if (!current_instance)
    {
        throw name_error_t("'super' not available in current context - must be inside a class method");
    }
    
    // Get the parent class
    auto parent_class = current_instance->m_class_obj->parent_class();
    if (!parent_class)
    {
        throw type_error_t("Class '" + current_instance->m_class_obj->m_class_name + "' has no parent class");
    }
    
    // Create a special "super" proxy that references the parent class methods but uses the current instance's data
    // For now, we'll return the current instance but mark it for special handling in method calls
    // The actual super method resolution will happen in method_call visitor
    m_current_result = current_instance;
    m_is_super_call = true;  // We'll need to add this flag to the interpreter
    
    zephyr::current_error_location() = saved_location;
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
    m_function_resolvers.emplace_back(); // Add new function resolver for this scope
}

auto interpreter_t::pop_scope() -> std::map<std::string, value_t>
{
    if (m_scope_stack.empty()) {
        throw internal_error_t("Cannot pop from empty scope stack");
    }
    auto scope = m_scope_stack.back();
    m_scope_stack.pop_back();
    
    // Also remove the corresponding function resolver
    if (!m_function_resolvers.empty()) {
        m_function_resolvers.pop_back();
    }
    
    return scope;
}

auto interpreter_t::enter_function_scope() -> void
{
    std::map<std::string, value_t> function_scope;
    m_scope_stack.push_back(function_scope);
    m_function_resolvers.emplace_back();
}

auto interpreter_t::exit_function_scope() -> void
{
    if (m_scope_stack.size() <= 1) {
        throw internal_error_t("Cannot exit global scope");
    }
    m_scope_stack.pop_back();
    if (!m_function_resolvers.empty()) {
        m_function_resolvers.pop_back();
    }
}

auto interpreter_t::visit(spawn_expression_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    // Execute the expression immediately and resolve the promise
    // This fixes the segfault while maintaining the cooperative scheduler architecture
    node.expression->accept(*this);
    auto result = m_current_result;

    // Create a resolved promise instead of using lambda capture
    auto& scheduler = async_scheduler_t::instance();
    auto promise_obj = scheduler.create_resolved_promise(result);
    
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

        // Handle int/sized_int compatibility for unified integer system
        if (is_integer_type_compatible(normalized_actual, normalized_expected, value)) {
            return; // Integer types are compatible
        }
        
        // Provide specific error messages for integer overflow cases
        if (is_integer_type_name(normalized_actual) && is_integer_type_name(normalized_expected)) {
            throw_integer_overflow_error(value, normalized_actual, normalized_expected, variable_name);
            return; // This line won't be reached, but added for clarity
        }

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

auto interpreter_t::convert_value_to_type(value_t value, const std::string& target_type) -> value_t
{
    const std::string& current_type = value->type()->name();
    
    // If types are already the same, no conversion needed
    if (current_type == target_type) {
        return value;
    }
    
    // Handle integer type conversions
    if (is_integer_type_name(current_type) && is_integer_type_name(target_type)) {
        return convert_integer_value(value, target_type);
    }
    
    // For other types, return as-is (compatibility was already checked)
    return value;
}

auto interpreter_t::is_integer_type_name(const std::string& type_name) -> bool
{
    static const std::set<std::string> integer_types = {
        "int", "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64"
    };
    return integer_types.find(type_name) != integer_types.end();
}

auto interpreter_t::convert_integer_value(value_t value, const std::string& target_type) -> value_t
{
    int64_t int_value = 0;
    
    // Extract the integer value
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        int_value = int_obj->value_64();
    } else {
        return value; // Not an integer, return as-is
    }
    
    // Convert to target type
    if (target_type == "int") {
        // Convert to regular int (with range checking already done in validate_type_constraint)
        return int_object_t::create_int(static_cast<int>(int_value));
    } else if (target_type == "i8") {
        return int_object_t::create_i8(static_cast<int8_t>(int_value));
    } else if (target_type == "u8") {
        return int_object_t::create_u8(static_cast<uint8_t>(int_value));
    } else if (target_type == "i16") {
        return int_object_t::create_i16(static_cast<int16_t>(int_value));
    } else if (target_type == "u16") {
        return int_object_t::create_u16(static_cast<uint16_t>(int_value));
    } else if (target_type == "i32") {
        return int_object_t::create_i32(static_cast<int32_t>(int_value));
    } else if (target_type == "u32") {
        return int_object_t::create_u32(static_cast<uint32_t>(int_value));
    } else if (target_type == "i64") {
        return int_object_t::create_i64(int_value);
    } else if (target_type == "u64") {
        return int_object_t::create_u64(static_cast<uint64_t>(int_value));
    }
    
    // Unknown type, return as-is
    return value;
}

auto interpreter_t::is_integer_type_compatible(const std::string& actual_type, const std::string& expected_type, value_t value) -> bool
{
    // Define integer types (regular int and all sized integer types)
    static const std::set<std::string> integer_types = {
        "int", "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64"
    };

    // Both must be integer types for compatibility checking
    if (integer_types.find(actual_type) == integer_types.end() || 
        integer_types.find(expected_type) == integer_types.end()) {
        return false;
    }

    // Special case: int and i32 are always compatible (unified system)
    if ((actual_type == "int" && expected_type == "i32") ||
        (actual_type == "i32" && expected_type == "int")) {
        return true;
    }

    // If types are exactly the same, they're compatible
    if (actual_type == expected_type) {
        return true;
    }

    // For different sized integer types, check if the value fits in the target type
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        // Extract the actual value from the unified integer object
        int64_t int_value = int_obj->value_64();
        return value_fits_in_integer_type(int_value, expected_type);
    }

    // If we can't determine compatibility, be safe and reject
    return false;
}

auto interpreter_t::throw_integer_overflow_error(value_t value, const std::string& actual_type, const std::string& expected_type, const std::string& variable_name) -> void
{
    int64_t int_value = 0;
    
    // Extract the integer value for the error message
    if (auto int_obj = std::dynamic_pointer_cast<int_object_t>(value)) {
        int_value = int_obj->value_64();
    }
    
    // Get the valid range for the target type
    std::string range_info = get_type_range_string(expected_type);
    
    // Create a helpful error message
    std::string error_msg = "Integer overflow: Cannot assign value " + std::to_string(int_value) + 
                           " to variable '" + variable_name + "' of type " + expected_type + ". " +
                           "Valid range for " + expected_type + " is " + range_info + ".";
    
    // Add suggestion for what type could hold this value
    if (expected_type != "i64" && expected_type != "u64") {
        std::string suggested_type = suggest_integer_type_for_value(int_value);
        if (!suggested_type.empty() && suggested_type != expected_type) {
            error_msg += " Consider using type " + suggested_type + " instead.";
        }
    }
    
    throw type_error_t(error_msg);
}

auto interpreter_t::get_type_range_string(const std::string& type_name) -> std::string
{
    if (type_name == "int" || type_name == "i32") {
        return "-2,147,483,648 to 2,147,483,647";
    }
    else if (type_name == "i8") {
        return "-128 to 127";
    }
    else if (type_name == "u8") {
        return "0 to 255";
    }
    else if (type_name == "i16") {
        return "-32,768 to 32,767";
    }
    else if (type_name == "u16") {
        return "0 to 65,535";
    }
    else if (type_name == "u32") {
        return "0 to 4,294,967,295";
    }
    else if (type_name == "i64") {
        return "-9,223,372,036,854,775,808 to 9,223,372,036,854,775,807";
    }
    else if (type_name == "u64") {
        return "0 to 9,223,372,036,854,775,807"; // Limited by our int64_t representation
    }
    
    return "unknown range";
}

auto interpreter_t::suggest_integer_type_for_value(int64_t value) -> std::string
{
    if (value < 0) {
        // Negative values need signed types
        if (value >= -128) return "i8";
        if (value >= -32768) return "i16";
        if (value >= -2147483648LL) return "i32";
        return "i64";
    } else {
        // Positive values - prefer smaller types first
        if (value <= 127) return "i8";
        if (value <= 255) return "u8";
        if (value <= 32767) return "i16";
        if (value <= 65535) return "u16";
        if (value <= 2147483647LL) return "i32";
        if (value <= 4294967295LL) return "u32";
        return "i64";
    }
}

auto interpreter_t::value_fits_in_integer_type(int64_t value, const std::string& type_name) -> bool
{
    if (type_name == "int" || type_name == "i32") {
        return value >= std::numeric_limits<int32_t>::min() && 
               value <= std::numeric_limits<int32_t>::max();
    }
    else if (type_name == "i8") {
        return value >= -128 && value <= 127;
    }
    else if (type_name == "u8") {
        return value >= 0 && value <= 255;
    }
    else if (type_name == "i16") {
        return value >= -32768 && value <= 32767;
    }
    else if (type_name == "u16") {
        return value >= 0 && value <= 65535;
    }
    else if (type_name == "u32") {
        return value >= 0 && value <= std::numeric_limits<uint32_t>::max();
    }
    else if (type_name == "i64") {
        return true; // int64_t can always fit in i64
    }
    else if (type_name == "u64") {
        return value >= 0; // Only non-negative values fit in u64
    }
    
    return false;
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
            
            // Convert value to the specified type if there's a type constraint
            auto constraint_it = m_type_constraints.find(variable_name);
            if (constraint_it != m_type_constraints.end()) {
                value = convert_value_to_type(value, constraint_it->second);
            }
            
            scope[variable_name] = value;
            return;
        }
    }

    // If variable doesn't exist, create it in current scope
    auto& current_scope = m_scope_stack.back();
    validate_type_constraint(variable_name, value);
    
    // Convert value to the specified type if there's a type constraint
    auto constraint_it = m_type_constraints.find(variable_name);
    if (constraint_it != m_type_constraints.end()) {
        value = convert_value_to_type(value, constraint_it->second);
    }
    
    current_scope[variable_name] = value;
}


// Static member definitions
std::map<std::string, value_t> interpreter_t::s_builtin_functions;
bool interpreter_t::s_builtins_initialized = false;

auto interpreter_t::set_module_loader(std::shared_ptr<module_loader_t> loader) -> void
{
    m_module_loader = loader;
}

auto interpreter_t::set_current_module(std::shared_ptr<module_t> module) -> void
{
    m_current_module = module;
    inject_module_name_variable();
}

auto interpreter_t::get_current_module() const -> std::shared_ptr<module_t>
{
    return m_current_module;
}

auto interpreter_t::visit(import_statement_t& node) -> void
{
    zephyr::error_location_context_t saved_location = zephyr::current_error_location();
    zephyr::current_error_location().line = node.line;
    zephyr::current_error_location().column = node.column;
    zephyr::current_error_location().length = node.end_column - node.column + 1;

    if (!m_module_loader)
    {
        throw import_error_t("Module loader not set");
    }
    
    // Load the module first to get the canonical file path
    auto module = m_module_loader->load_module(
        node.get_module_specifier(), 
        node.is_path_based(), 
        m_current_module ? m_current_module->get_file_path() : ""
    );
    
    // Use the loaded module's file path as the import key for consistent tracking
    // Canonicalize the path to resolve differences like "./file.ext" vs "file.ext"
    std::string module_file_path;
    try {
        module_file_path = std::filesystem::canonical(module->get_file_path()).string();
    } catch (const std::filesystem::filesystem_error&) {
        // Fallback to absolute path if canonical fails
        module_file_path = std::filesystem::absolute(module->get_file_path()).string();
    }
    
    // Check for double import using the module's canonical file path
    if (m_imported_modules.find(module_file_path) != m_imported_modules.end())
    {
        throw import_error_t("Module '" + node.get_module_specifier() + "' has already been imported in this module");
    }
    
    // Track the import using the module's canonical file path
    m_imported_modules.insert(module_file_path);
    
    switch (node.get_import_type())
    {
        case import_statement_t::import_type_e::lazy_import:
        {
            // Lazy import: import math [as alias] - creates module object
            std::string var_name;
            if (node.has_alias())
            {
                var_name = node.get_alias_name();
            }
            else
            {
                // Use the last part of a dotted module name
                std::string spec = node.get_module_specifier();
                size_t last_dot = spec.find_last_of('.');
                var_name = (last_dot != std::string::npos) ? spec.substr(last_dot + 1) : spec;
            }
            
            // Create a module object that supports direct member access
            auto module_obj = std::make_shared<module_object_t>(module, var_name);
            
            // Store the module object in the current scope
            variable(var_name, module_obj);
            break;
        }
        
        case import_statement_t::import_type_e::named_import:
        {
            // Named import: import symbol1, symbol2 from module [as alias]
            for (const auto& symbol_name : node.get_imported_symbols())
            {
                auto export_value = module->get_export(symbol_name);
                if (!export_value)
                {
                    throw name_error_t("Module '" + node.get_module_specifier() + "' has no export '" + symbol_name + "'");
                }
                
                // Import the symbol into current scope
                variable(symbol_name, export_value);
            }
            
            // If there's an alias for the module itself, create a filtered module object
            if (node.has_alias())
            {
                auto module_obj = std::make_shared<module_object_t>(module, node.get_alias_name(), node.get_imported_symbols());
                variable(node.get_alias_name(), module_obj);
            }
            break;
        }
        
        case import_statement_t::import_type_e::string_import:
        {
            // String import: import "./lib/math.zephyr" [as alias] - creates module object
            std::string var_name;
            if (node.has_alias())
            {
                var_name = node.get_alias_name();
            }
            else
            {
                // Use the filename without extension
                std::filesystem::path path(node.get_module_specifier());
                var_name = path.stem().string();
            }
            
            // Create a module object that supports direct member access
            auto module_obj = std::make_shared<module_object_t>(module, var_name);
            
            // Store the module object in the current scope
            variable(var_name, module_obj);
            break;
        }
    }
    
    zephyr::current_error_location() = saved_location;
}

auto interpreter_t::inject_module_name_variable() -> void
{
    if (m_current_module)
    {
        std::string module_name = m_current_module->get_name();
        auto module_name_obj = std::make_shared<string_object_t>(module_name);
        variable("__module_name__", module_name_obj);
    }
}

auto interpreter_t::add_to_exports(const std::string& symbol_name, value_t value) -> void
{
    if (m_current_module)
    {
        m_current_module->add_export(symbol_name, value);
    }
}

auto interpreter_t::should_export(bool is_internal) const -> bool
{
    return !is_internal && m_current_module;
}

auto interpreter_t::update_module_name_variable() -> void
{
    inject_module_name_variable();
}

auto interpreter_t::get_global_scope() const -> const std::map<std::string, value_t>&
{
    if (!m_scope_stack.empty())
    {
        return m_scope_stack[0]; // Return the first (global) scope
    }
    static const std::map<std::string, value_t> empty_scope;
    return empty_scope;
}

auto interpreter_t::set_global_variable(const std::string& name, value_t value) -> void
{
    if (!m_scope_stack.empty())
    {
        auto& global_scope = m_scope_stack[0]; // Access the global scope
        global_scope[name] = value;
    }
}

auto interpreter_t::has_global_variable(const std::string& name) const -> bool
{
    if (!m_scope_stack.empty())
    {
        const auto& global_scope = m_scope_stack[0];
        return global_scope.find(name) != global_scope.end();
    }
    return false;
}

auto interpreter_t::remove_global_variable(const std::string& name) -> bool
{
    if (!m_scope_stack.empty())
    {
        auto& global_scope = m_scope_stack[0];
        auto it = global_scope.find(name);
        if (it != global_scope.end())
        {
            global_scope.erase(it);
            return true;
        }
    }
    return false;
}

auto interpreter_t::check_and_yield() -> void
{
    m_operation_count++;
    
    // Yield every 50 operations to allow cooperative multitasking
    if (m_operation_count >= 50) {
        m_operation_count = 0;
        
        // Check if we're in an async task and should yield
        auto& scheduler = async_scheduler_t::instance();
        auto current_task = scheduler.get_current_task();
        if (current_task) {
            // Throw yield exception to suspend task execution
            throw zephyr::yield_exception_t();
        }
    }
}

} // namespace zephyr