#pragma once

#include "zephyr/objects/object.hpp"
#include <string>
#include <memory>
#include <stdexcept>
#include <optional>
#include <functional>
#include <type_traits>

namespace zephyr::api {

// Forward declarations
using value_t = std::shared_ptr<object_t>;

// Error severity levels
enum class error_severity_t {
    info,
    warning,
    error,
    fatal
};

// Error information structure
struct error_info_t {
    std::string message;
    std::string type = "RuntimeError";
    std::string source_location;
    int line = -1;
    int column = -1;
    std::string filename;
    std::string stack_trace;
    error_severity_t severity = error_severity_t::error;
    std::string plugin_name;
    
    auto to_string() const -> std::string {
        std::string result = type + ": " + message;
        if (!plugin_name.empty()) {
            result = "[" + plugin_name + "] " + result;
        }
        if (line >= 0 && column >= 0) {
            result += " at line " + std::to_string(line) + ", column " + std::to_string(column);
        }
        if (!filename.empty()) {
            result += " in " + filename;
        }
        return result;
    }
    
    auto severity_string() const -> std::string {
        switch (severity) {
            case error_severity_t::info: return "INFO";
            case error_severity_t::warning: return "WARNING";
            case error_severity_t::error: return "ERROR";
            case error_severity_t::fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};

// Exception class for plugin errors
class plugin_exception_t : public std::runtime_error {
public:
    plugin_exception_t(const error_info_t& info) 
        : std::runtime_error(info.message), m_info(info) {}
    
    plugin_exception_t(const std::string& message, const std::string& plugin_name = "")
        : std::runtime_error(message) {
        m_info.message = message;
        m_info.plugin_name = plugin_name;
    }
    
    auto info() const -> const error_info_t& { return m_info; }

private:
    error_info_t m_info;
};

// Result type for operations that can fail
template<typename T>
class result_t {
public:
    // Default constructor (for initialization purposes)
    result_t() : m_has_value(false) {}
    
    // Success constructor - made explicit to avoid ambiguity
    explicit result_t(T value) : m_value(std::move(value)), m_has_value(true) {}
    
    // Error constructors - use tag to disambiguate
    struct error_tag {};
    result_t(error_tag, const std::string& error) : m_error(error), m_has_value(false) {}
    result_t(error_tag, const error_info_t& error) : m_error_info(error), m_has_value(false) {}
    
    // Static factory methods
    static auto success(T value) -> result_t<T> {
        return result_t<T>(std::move(value));
    }
    
    static auto error(const std::string& message) -> result_t<T> {
        return result_t<T>(error_tag{}, message);
    }
    
    static auto error(const error_info_t& info) -> result_t<T> {
        return result_t<T>(error_tag{}, info);
    }
    
    // State checking
    auto is_success() const -> bool { return m_has_value; }
    auto is_error() const -> bool { return !m_has_value; }
    operator bool() const { return m_has_value; }
    
    // Value access
    auto value() const -> const T& {
        if (!m_has_value) {
            throw std::runtime_error("Attempted to access value of failed result");
        }
        return m_value;
    }
    
    auto value() -> T& {
        if (!m_has_value) {
            throw std::runtime_error("Attempted to access value of failed result");
        }
        return m_value;
    }
    
    auto value_or(const T& default_value) const -> T {
        return m_has_value ? m_value : default_value;
    }
    
    // Error access
    auto error() const -> std::string {
        if (m_has_value) {
            return "";
        }
        return m_error_info.has_value() ? m_error_info->message : m_error;
    }
    
    auto error_info() const -> std::optional<error_info_t> {
        return m_error_info;
    }
    
    // Monadic operations
    template<typename F>
    auto map(F&& func) const -> result_t<decltype(func(std::declval<T>()))> {
        if (!m_has_value) {
            if (m_error_info.has_value()) {
                return result_t<decltype(func(std::declval<T>()))>::error(m_error_info.value());
            }
            return result_t<decltype(func(std::declval<T>()))>::error(m_error);
        }
        return result_t<decltype(func(std::declval<T>()))>::success(func(m_value));
    }
    
    template<typename F>
    auto and_then(F&& func) const -> decltype(func(std::declval<T>())) {
        if (!m_has_value) {
            if (m_error_info.has_value()) {
                return decltype(func(std::declval<T>()))::error(m_error_info.value());
            }
            return decltype(func(std::declval<T>()))::error(m_error);
        }
        return func(m_value);
    }

private:
    T m_value{};
    std::string m_error;
    std::optional<error_info_t> m_error_info;
    bool m_has_value;
};

// Specialization for void results
template<>
class result_t<void> {
public:
    // Success constructor
    // Default constructor (success for void)
    result_t() : m_has_value(true) {}
    
    // Error constructors - use tag to maintain consistency
    struct error_tag {};
    result_t(error_tag, const std::string& error) : m_error(error), m_has_value(false) {}
    result_t(error_tag, const error_info_t& error) : m_error_info(error), m_has_value(false) {}
    
    // Static factory methods
    static auto success() -> result_t<void> {
        return result_t<void>();
    }
    
    static auto error(const std::string& message) -> result_t<void> {
        return result_t<void>(error_tag{}, message);
    }
    
    static auto error(const error_info_t& info) -> result_t<void> {
        return result_t<void>(error_tag{}, info);
    }
    
    // State checking
    auto is_success() const -> bool { return m_has_value; }
    auto is_error() const -> bool { return !m_has_value; }
    operator bool() const { return m_has_value; }
    
    // Error access
    auto error() const -> std::string {
        if (m_has_value) {
            return "";
        }
        return m_error_info.has_value() ? m_error_info->message : m_error;
    }
    
    auto error_info() const -> std::optional<error_info_t> {
        return m_error_info;
    }

private:
    std::string m_error;
    std::optional<error_info_t> m_error_info;
    bool m_has_value;
};

// Type aliases for common result types
using void_result_t = result_t<void>;
using value_result_t = result_t<value_t>;

// Error handler function type
using error_handler_t = std::function<void(const error_info_t&)>;

// Utility functions for creating error info
auto create_error_info(const std::string& message, 
                      const std::string& type = "RuntimeError",
                      const std::string& plugin_name = "",
                      int line = -1, int column = -1,
                      const std::string& filename = "",
                      error_severity_t severity = error_severity_t::error) -> error_info_t;

auto create_plugin_error(const std::string& message,
                        const std::string& plugin_name,
                        const std::string& type = "PluginError") -> error_info_t;

} // namespace zephyr::api