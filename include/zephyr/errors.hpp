#pragma once

#include "zephyr/runtime_error.hpp"

namespace zephyr
{
    class syntax_error_t : public runtime_error_with_location_t
    {
    public:
        syntax_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "SyntaxError") {}
    };

    class internal_error_t : public runtime_error_with_location_t
    {
    public:
        internal_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "InternalError") {}
    };

    class name_error_t : public runtime_error_with_location_t
    {
    public:
        name_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "NameError") {}
    };

    class type_error_t : public runtime_error_with_location_t
    {
    public:
        type_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "TypeError") {}
    };

    class zero_division_error_t : public runtime_error_with_location_t
    {
    public:
        zero_division_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "ZeroDivisionError") {}
    };

    class index_error_t : public runtime_error_with_location_t
    {
    public:
        index_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "IndexError") {}
    };

    class key_error_t : public runtime_error_with_location_t
    {
    public:
        key_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "KeyError") {}
    };

    class attribute_error_t : public runtime_error_with_location_t
    {
    public:
        attribute_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "AttributeError") {}
    };

    class value_error_t : public runtime_error_with_location_t
    {
    public:
        value_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "ValueError") {}
    };

    class import_error_t : public runtime_error_with_location_t
    {
    public:
        import_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "ImportError") {}
    };

    class io_error_t : public runtime_error_with_location_t
    {
    public:
        io_error_t(const std::string& message)
            : runtime_error_with_location_t(message, "IOError") {}
    };
}
