#pragma once

#include "interpreter.hpp"

namespace zephyr
{
    class name_error_t : public runtime_error_with_location_t
    {
    public:
        name_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "NameError") {}
    };

    class type_error_t : public runtime_error_with_location_t
    {
    public:
        type_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "TypeError") {}
    };

    class zero_division_error_t : public runtime_error_with_location_t
    {
    public:
        zero_division_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "ZeroDivisionError") {}
    };

    class index_error_t : public runtime_error_with_location_t
    {
    public:
        index_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "IndexError") {}
    };

    class key_error_t : public runtime_error_with_location_t
    {
    public:
        key_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "KeyError") {}
    };

    class attribute_error_t : public runtime_error_with_location_t
    {
    public:
        attribute_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "AttributeError") {}
    };

    class value_error_t : public runtime_error_with_location_t
    {
    public:
        value_error_t(const std::string& message, int line, int column, int length = 1)
            : runtime_error_with_location_t(message, line, column, length, "ValueError") {}
    };
}
