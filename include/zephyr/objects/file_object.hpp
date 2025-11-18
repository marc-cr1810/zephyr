#pragma once

#include "zephyr/objects/object.hpp"
#include "zephyr/types/file_type.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace zephyr {

class file_object_t : public object_t {
private:
    std::fstream m_file;
    std::string m_path;
    std::string m_mode;
    bool m_closed;
    bool m_binary_mode;
    
public:
    // Constructor with default mode "r"
    file_object_t(const std::string& path, const std::string& mode = "r");
    
    // Destructor - ensures file is closed
    ~file_object_t() override;
    
    // Object interface
    auto type() const -> std::shared_ptr<type_t> override;
    auto to_string() const -> std::string override;
    auto clone() const -> std::shared_ptr<object_t> override;
    
    // Context manager interface
    auto __enter__() -> std::shared_ptr<object_t> override;
    auto __exit__(std::shared_ptr<object_t> exc_type = nullptr,
                  std::shared_ptr<object_t> exc_value = nullptr,
                  std::shared_ptr<object_t> traceback = nullptr) -> bool override;
    
    // Method call interface
    auto call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> override;
    
    // File reading methods
    auto read(int size = -1) -> std::string;
    auto read_bytes(int size = -1) -> std::vector<uint8_t>;
    auto readline() -> std::string;
    auto readlines() -> std::vector<std::string>;
    
    // File writing methods
    auto write(const std::string& data) -> void;
    auto write_bytes(const std::vector<uint8_t>& data) -> void;
    
    // File positioning
    auto tell() -> std::streampos;
    auto seek(std::streampos pos) -> void;
    auto seek_to_beginning() -> void;
    auto seek_to_end() -> void;
    
    // File operations
    auto flush() -> void;
    auto close() -> void;
    
    // File properties
    auto is_closed() const -> bool;
    auto name() const -> std::string;
    auto mode() const -> std::string;
    auto size() const -> std::streamsize;
    
    // File iteration support (for 'for line in file')
    class iterator {
    private:
        file_object_t* m_file;
        std::string m_current_line;
        bool m_at_end;
        
    public:
        iterator(file_object_t* file, bool at_end = false);
        
        auto operator*() const -> const std::string&;
        auto operator++() -> iterator&;
        auto operator!=(const iterator& other) const -> bool;
        
    private:
        auto advance() -> void;
    };
    
    auto begin() -> iterator;
    auto end() -> iterator;
    
private:
    // Helper methods
    auto validate_mode(const std::string& mode) -> void;
    auto get_open_mode(const std::string& mode) -> std::ios::openmode;
    auto check_open(const std::string& operation) -> void;
    auto check_readable() -> void;
    auto check_writable() -> void;
    auto handle_exclusive_creation(const std::string& path) -> void;
};

// Built-in open function declaration
auto open_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t>;

} // namespace zephyr