#include "zephyr/objects/file_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/list_object.hpp"
#include "zephyr/objects/none_object.hpp"
#include "zephyr/errors.hpp"
#include <sstream>
#include <set>
#include <stdexcept>

namespace zephyr {

// Constructor
file_object_t::file_object_t(const std::string& path, const std::string& mode)
    : m_path(path), m_mode(mode), m_closed(false), m_binary_mode(false) {
    
    validate_mode(mode);
    
    // Handle exclusive creation mode
    if (mode[0] == 'x' || (mode.length() > 1 && mode[1] == 'x')) {
        handle_exclusive_creation(path);
    }
    
    std::ios::openmode open_mode = get_open_mode(mode);
    m_file.open(path, open_mode);
    
    if (!m_file.is_open()) {
        std::string error_msg = "Could not open file: " + path;
        
        // Provide more specific error messages
        if (mode[0] == 'r' && !std::filesystem::exists(path)) {
            error_msg += " (file does not exist)";
        } else if (mode[0] == 'x') {
            error_msg += " (file already exists in exclusive mode)";
        }
        
        throw io_error_t(error_msg);
    }
}

// Destructor
file_object_t::~file_object_t() {
    close();
}

// Object interface
auto file_object_t::type() const -> std::shared_ptr<type_t> {
    return file_type_t::get_instance();
}

auto file_object_t::to_string() const -> std::string {
    std::string status = m_closed ? "closed" : "open";
    return "<file '" + m_path + "', mode '" + m_mode + "', " + status + ">";
}

auto file_object_t::clone() const -> std::shared_ptr<object_t> {
    // Files cannot be cloned - return a new file object with same path/mode
    return std::make_shared<file_object_t>(m_path, m_mode);
}

// Context manager interface
auto file_object_t::__enter__() -> std::shared_ptr<object_t> {
    if (m_closed) {
        throw io_error_t("Cannot enter context of closed file");
    }
    return shared_from_this();
}

auto file_object_t::__exit__(std::shared_ptr<object_t> exc_type,
                            std::shared_ptr<object_t> exc_value,
                            std::shared_ptr<object_t> traceback) -> bool {
    close();
    return false; // Don't suppress exceptions
}

// Method call implementation for file objects
auto file_object_t::call_method(const std::string& method_name, const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
    if (method_name == "read") {
        int size = -1;
        if (!args.empty()) {
            auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            if (int_obj) {
                size = int_obj->value();
            }
        }
        std::string content = read(size);
        return std::make_shared<string_object_t>(content);
    }
    else if (method_name == "readline") {
        std::string line = readline();
        return std::make_shared<string_object_t>(line);
    }
    else if (method_name == "readlines") {
        auto lines = readlines();
        std::vector<std::shared_ptr<object_t>> line_objects;
        for (const auto& line : lines) {
            line_objects.push_back(std::make_shared<string_object_t>(line));
        }
        return std::make_shared<list_object_t>(line_objects);
    }
    else if (method_name == "write") {
        if (args.size() != 1) {
            throw type_error_t("write() takes exactly one argument");
        }
        std::string data = args[0]->to_string();
        write(data);
        return std::make_shared<none_object_t>();
    }
    else if (method_name == "write_bytes") {
        if (args.size() != 1) {
            throw type_error_t("write_bytes() takes exactly one argument");
        }
        auto list_obj = std::dynamic_pointer_cast<list_object_t>(args[0]);
        if (!list_obj) {
            throw type_error_t("write_bytes() requires a list of integers");
        }
        std::vector<uint8_t> bytes;
        for (const auto& elem : list_obj->elements()) {
            auto int_obj = std::dynamic_pointer_cast<int_object_t>(elem);
            if (int_obj) {
                bytes.push_back(static_cast<uint8_t>(int_obj->value()));
            }
        }
        write_bytes(bytes);
        return std::make_shared<none_object_t>();
    }
    else if (method_name == "read_bytes") {
        int size = -1;
        if (!args.empty()) {
            auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            if (int_obj) {
                size = int_obj->value();
            }
        }
        auto bytes = read_bytes(size);
        std::vector<std::shared_ptr<object_t>> byte_objects;
        for (uint8_t byte : bytes) {
            byte_objects.push_back(std::make_shared<int_object_t>(static_cast<int>(byte)));
        }
        return std::make_shared<list_object_t>(byte_objects);
    }
    else if (method_name == "close") {
        close();
        return std::make_shared<none_object_t>();
    }
    else if (method_name == "flush") {
        flush();
        return std::make_shared<none_object_t>();
    }
    else if (method_name == "seek") {
        if (args.size() != 1) {
            throw type_error_t("seek() takes exactly one argument");
        }
        auto int_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
        if (!int_obj) {
            throw type_error_t("seek() requires an integer argument");
        }
        seek(static_cast<std::streampos>(int_obj->value()));
        return std::make_shared<none_object_t>();
    }
    else if (method_name == "tell") {
        std::streampos pos = tell();
        return std::make_shared<int_object_t>(static_cast<int>(pos));
    }
    else {
        throw attribute_error_t("File object has no method '" + method_name + "'");
    }
}

// File reading methods
auto file_object_t::read(int size) -> std::string {
    check_open("read");
    check_readable();
    
    if (m_binary_mode) {
        throw io_error_t("Cannot read text from binary file. Use read_bytes()");
    }
    
    if (size == -1) {
        // Read entire file
        std::ostringstream buffer;
        buffer << m_file.rdbuf();
        return buffer.str();
    } else {
        // Read specific number of characters
        std::string content(size, '\0');
        m_file.read(&content[0], size);
        content.resize(static_cast<size_t>(m_file.gcount()));
        return content;
    }
}

auto file_object_t::read_bytes(int size) -> std::vector<uint8_t> {
    check_open("read_bytes");
    check_readable();
    
    if (!m_binary_mode) {
        throw io_error_t("Cannot read bytes from text file. Use read()");
    }
    
    if (size == -1) {
        // Read entire file
        m_file.seekg(0, std::ios::end);
        std::streampos length = m_file.tellg();
        m_file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> buffer(static_cast<size_t>(length));
        m_file.read(reinterpret_cast<char*>(buffer.data()), length);
        return buffer;
    } else {
        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        m_file.read(reinterpret_cast<char*>(buffer.data()), size);
        buffer.resize(static_cast<size_t>(m_file.gcount()));
        return buffer;
    }
}

auto file_object_t::readline() -> std::string {
    check_open("readline");
    check_readable();
    
    if (m_binary_mode) {
        throw io_error_t("Cannot readline from binary file");
    }
    
    std::string line;
    if (std::getline(m_file, line)) {
        return line + "\n";
    }
    return ""; // EOF
}

auto file_object_t::readlines() -> std::vector<std::string> {
    check_open("readlines");
    check_readable();
    
    if (m_binary_mode) {
        throw io_error_t("Cannot readlines from binary file");
    }
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(m_file, line)) {
        lines.push_back(line + "\n");
    }
    return lines;
}

// File writing methods
auto file_object_t::write(const std::string& data) -> void {
    check_open("write");
    check_writable();
    
    if (m_binary_mode) {
        throw io_error_t("Cannot write text to binary file. Use write_bytes()");
    }
    
    m_file << data;
    m_file.flush();
}

auto file_object_t::write_bytes(const std::vector<uint8_t>& data) -> void {
    check_open("write_bytes");
    check_writable();
    
    if (!m_binary_mode) {
        throw io_error_t("Cannot write bytes to text file. Use write()");
    }
    
    m_file.write(reinterpret_cast<const char*>(data.data()), 
                 static_cast<std::streamsize>(data.size()));
    m_file.flush();
}

// File positioning
auto file_object_t::tell() -> std::streampos {
    check_open("tell");
    return m_file.tellg();
}

auto file_object_t::seek(std::streampos pos) -> void {
    check_open("seek");
    m_file.seekg(pos);
    m_file.seekp(pos);
}

auto file_object_t::seek_to_beginning() -> void {
    check_open("seek_to_beginning");
    m_file.seekg(0, std::ios::beg);
    m_file.seekp(0, std::ios::beg);
}

auto file_object_t::seek_to_end() -> void {
    check_open("seek_to_end");
    m_file.seekg(0, std::ios::end);
    m_file.seekp(0, std::ios::end);
}

// File operations
auto file_object_t::flush() -> void {
    check_open("flush");
    m_file.flush();
}

auto file_object_t::close() -> void {
    if (!m_closed && m_file.is_open()) {
        m_file.close();
        m_closed = true;
    }
}

// File properties
auto file_object_t::is_closed() const -> bool {
    return m_closed;
}

auto file_object_t::name() const -> std::string {
    return m_path;
}

auto file_object_t::mode() const -> std::string {
    return m_mode;
}

auto file_object_t::size() const -> std::streamsize {
    if (m_closed || !std::filesystem::exists(m_path)) {
        return 0;
    }
    
    try {
        return static_cast<std::streamsize>(std::filesystem::file_size(m_path));
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

// File iterator implementation
file_object_t::iterator::iterator(file_object_t* file, bool at_end) 
    : m_file(file), m_at_end(at_end) {
    if (!at_end && m_file && !m_file->is_closed()) {
        advance();
    }
}

auto file_object_t::iterator::operator*() const -> const std::string& {
    return m_current_line;
}

auto file_object_t::iterator::operator++() -> iterator& {
    advance();
    return *this;
}

auto file_object_t::iterator::operator!=(const iterator& other) const -> bool {
    return m_at_end != other.m_at_end;
}

auto file_object_t::iterator::advance() -> void {
    if (!m_file || m_file->is_closed() || m_file->m_binary_mode) {
        m_at_end = true;
        return;
    }
    
    if (m_file->m_file.eof() || !std::getline(m_file->m_file, m_current_line)) {
        m_at_end = true;
    } else {
        m_current_line += "\n";
    }
}

auto file_object_t::begin() -> iterator {
    return iterator(this);
}

auto file_object_t::end() -> iterator {
    return iterator(this, true);
}

// Helper methods
auto file_object_t::validate_mode(const std::string& mode) -> void {
    if (mode.empty()) {
        throw std::invalid_argument("File mode cannot be empty");
    }
    
    // Valid mode patterns
    std::set<std::string> valid_modes = {
        "r", "w", "a", "x",
        "rb", "wb", "ab", "xb",
        "r+", "w+", "a+",
        "r+b", "w+b", "a+b",
        "rb+", "wb+", "ab+"
    };
    
    if (valid_modes.find(mode) == valid_modes.end()) {
        throw std::invalid_argument("Invalid file mode: " + mode);
    }
    
    // Check for binary mode
    m_binary_mode = (mode.find('b') != std::string::npos);
}

auto file_object_t::get_open_mode(const std::string& mode) -> std::ios::openmode {
    std::ios::openmode open_mode = std::ios::in; // Default
    
    char primary_mode = mode[0];
    bool has_plus = mode.find('+') != std::string::npos;
    bool is_binary = mode.find('b') != std::string::npos;
    
    switch (primary_mode) {
        case 'r':
            open_mode = std::ios::in;
            if (has_plus) open_mode |= std::ios::out;
            break;
            
        case 'w':
            open_mode = std::ios::out | std::ios::trunc;
            if (has_plus) open_mode |= std::ios::in;
            break;
            
        case 'a':
            open_mode = std::ios::out | std::ios::app;
            if (has_plus) open_mode |= std::ios::in;
            break;
            
        case 'x':
            open_mode = std::ios::out | std::ios::trunc;
            if (has_plus) open_mode |= std::ios::in;
            break;
            
        default:
            throw std::invalid_argument("Unknown file mode: " + mode);
    }
    
    if (is_binary) {
        open_mode |= std::ios::binary;
    }
    
    return open_mode;
}

auto file_object_t::check_open(const std::string& operation) -> void {
    if (m_closed || !m_file.is_open()) {
        throw io_error_t("I/O operation " + operation + " on closed file");
    }
}

auto file_object_t::check_readable() -> void {
    // Check if mode supports reading
    char primary = m_mode[0];
    bool has_plus = m_mode.find('+') != std::string::npos;
    
    if (primary != 'r' && !has_plus) {
        throw io_error_t("File not open for reading (mode: " + m_mode + ")");
    }
}

auto file_object_t::check_writable() -> void {
    // Check if mode supports writing
    char primary = m_mode[0];
    bool has_plus = m_mode.find('+') != std::string::npos;
    
    if (primary == 'r' && !has_plus) {
        throw io_error_t("File not open for writing (mode: " + m_mode + ")");
    }
}

auto file_object_t::handle_exclusive_creation(const std::string& path) -> void {
    if (std::filesystem::exists(path)) {
        throw io_error_t("File exists: " + path + " (exclusive creation mode)");
    }
}

// Built-in open function
auto open_builtin(const std::vector<std::shared_ptr<object_t>>& args) -> std::shared_ptr<object_t> {
    if (args.empty() || args.size() > 2) {
        throw std::runtime_error("open() takes 1 or 2 arguments (path, mode='r')");
    }
    
    std::string path = args[0]->to_string();
    std::string mode = args.size() > 1 ? args[1]->to_string() : "r";
    
    try {
        return std::make_shared<file_object_t>(path, mode);
    } catch (const std::exception& e) {
        throw io_error_t("Failed to open file '" + path + "': " + e.what());
    }
}

} // namespace zephyr