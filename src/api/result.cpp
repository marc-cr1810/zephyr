#include "zephyr/api/result.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>

namespace zephyr::api {

// =============================================================================
// Utility Functions Implementation
// =============================================================================

auto create_error_info(const std::string& message, 
                      const std::string& type,
                      const std::string& plugin_name,
                      int line, int column,
                      const std::string& filename,
                      error_severity_t severity) -> error_info_t {
    error_info_t info;
    info.message = message;
    info.type = type;
    info.plugin_name = plugin_name;
    info.line = line;
    info.column = column;
    info.filename = filename;
    info.severity = severity;
    
    // Add timestamp to source location
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    info.source_location = ss.str();
    
    return info;
}

auto create_plugin_error(const std::string& message,
                        const std::string& plugin_name,
                        const std::string& type) -> error_info_t {
    return create_error_info(message, type, plugin_name, -1, -1, "", error_severity_t::error);
}

// Note: error_info_t methods and plugin_exception_t are already defined inline in the header

} // namespace zephyr::api