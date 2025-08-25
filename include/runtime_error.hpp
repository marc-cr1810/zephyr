#pragma once

#include <stdexcept>
#include <string>
#include "error_context.hpp"

namespace zephyr
{

class runtime_error_with_location_t : public std::runtime_error
{
public:
    // Existing constructor (can keep for explicit passing if needed)
    runtime_error_with_location_t(const std::string& message, int line, int column, int length, const std::string& error_name = "Error")
        : std::runtime_error(message), m_line(line), m_column(column), m_length(length), m_error_name(error_name)
    {}

    // New constructor that uses thread-local context
    runtime_error_with_location_t(const std::string& message, const std::string& error_name = "Error")
        : std::runtime_error(message),
              m_line(zephyr::get_current_error_location().line),
              m_column(zephyr::get_current_error_location().column),
              m_length(zephyr::get_current_error_location().length),
              m_error_name(error_name)
    {}

    // Accessors
    int line() const { return m_line; }
    int column() const { return m_column; }
    int length() const { return m_length; }
    const std::string& error_name() const { return m_error_name; }

private:
    int m_line;
    int m_column;
    int m_length;
    std::string m_error_name;
};

}
