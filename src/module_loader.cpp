#include "module_loader.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include "errors.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace zephyr
{

module_t::module_t(const std::string& name, const std::string& file_path)
    : m_name(name), m_file_path(file_path), m_is_executed(false)
{
}

auto module_t::get_name() const -> const std::string&
{
    return m_name;
}

auto module_t::get_file_path() const -> const std::string&
{
    return m_file_path;
}

auto module_t::get_source_code() const -> const std::string&
{
    return m_source_code;
}

auto module_t::set_source_code(const std::string& source) -> void
{
    m_source_code = source;
}

auto module_t::get_ast() const -> const program_t*
{
    return m_ast.get();
}

auto module_t::set_ast(std::unique_ptr<program_t> ast) -> void
{
    m_ast = std::move(ast);
}

auto module_t::is_executed() const -> bool
{
    return m_is_executed;
}

auto module_t::set_executed(bool executed) -> void
{
    m_is_executed = executed;
}

auto module_t::execute(module_loader_t& loader) -> void
{
    if (m_is_executed)
    {
        return;
    }
    
    if (!m_ast)
    {
        throw value_error_t("Module AST not set");
    }
    
    // Create interpreter for this module
    m_module_interpreter = std::make_shared<interpreter_t>(m_file_path, m_source_code);
    m_module_interpreter->set_module_loader(std::shared_ptr<module_loader_t>(&loader, [](module_loader_t*){}));
    m_module_interpreter->set_current_module(std::shared_ptr<module_t>(this, [](module_t*){}));
    
    // Execute the module
    m_module_interpreter->interpret(*m_ast);
    
    m_is_executed = true;
}

auto module_t::get_export(const std::string& symbol_name) const -> value_t
{
    auto it = m_exports.find(symbol_name);
    if (it != m_exports.end())
    {
        return it->second;
    }
    return nullptr;
}

auto module_t::add_export(const std::string& symbol_name, value_t value) -> void
{
    m_exports[symbol_name] = value;
}

auto module_t::get_all_exports() const -> const std::map<std::string, value_t>&
{
    return m_exports;
}

module_loader_t::module_loader_t()
{
    initialize_search_paths();
}

auto module_loader_t::initialize_search_paths() -> void
{
    // Add current working directory
    m_search_paths.push_back(".");
    
    // Parse ZEPHYRPATH environment variable
    const char* zephyr_path = std::getenv("ZEPHYRPATH");
    if (zephyr_path)
    {
        std::string path_string(zephyr_path);
        // Split by platform-specific delimiter
#ifdef _WIN32
        const char delimiter = ';';
#else
        const char delimiter = ':';
#endif
        
        size_t start = 0;
        size_t end = path_string.find(delimiter);
        
        while (end != std::string::npos)
        {
            std::string path = path_string.substr(start, end - start);
            if (!path.empty())
            {
                m_search_paths.push_back(path);
            }
            start = end + 1;
            end = path_string.find(delimiter, start);
        }
        
        // Add the last path
        if (start < path_string.length())
        {
            std::string path = path_string.substr(start);
            if (!path.empty())
            {
                m_search_paths.push_back(path);
            }
        }
    }
}

auto module_loader_t::load_module(const std::string& specifier, 
                                 bool is_path_based, 
                                 const std::string& requesting_file_path) -> std::shared_ptr<module_t>
{
    // Resolve the module path
    std::string resolved_path = resolve_module_path(specifier, is_path_based, requesting_file_path);
    
    // Check if module is already cached
    auto it = m_module_cache.find(resolved_path);
    if (it != m_module_cache.end())
    {
        return it->second;
    }
    
    // Check for circular dependencies
    detect_circular_dependency(resolved_path);
    
    // Add to loading stack
    m_loading_stack.push_back(resolved_path);
    
    try
    {
        // Read the module file
        std::string source_code = read_file_content(resolved_path);
        
        // Determine module name
        std::string module_name;
        if (is_path_based)
        {
            // For path-based imports, use the filename without extension
            std::filesystem::path path(resolved_path);
            module_name = path.stem().string();
        }
        else
        {
            // For name-based imports, use the specifier as the module name
            module_name = specifier;
        }
        
        // Create module
        auto module = std::make_shared<module_t>(module_name, resolved_path);
        module->set_source_code(source_code);
        
        // Parse the module
        lexer_t lexer(source_code);
        parser_t parser(lexer);
        auto ast = parser.parse();
        module->set_ast(std::move(ast));
        
        // Cache the module before execution to handle circular dependencies
        m_module_cache[resolved_path] = module;
        
        // Execute the module
        module->execute(*this);
        
        // Remove from loading stack
        m_loading_stack.pop_back();
        
        return module;
    }
    catch (const std::exception& e)
    {
        // Remove from loading stack on error
        m_loading_stack.pop_back();
        throw value_error_t("Failed to load module '" + specifier + "': " + e.what());
    }
}

auto module_loader_t::resolve_module_path(const std::string& specifier, 
                                         bool is_path_based,
                                         const std::string& requesting_file_path) const -> std::string
{
    if (is_path_based)
    {
        // Path-based resolution
        std::filesystem::path requesting_dir = std::filesystem::path(requesting_file_path).parent_path();
        std::filesystem::path resolved_path = requesting_dir / specifier;
        
        // Convert to absolute path
        resolved_path = std::filesystem::absolute(resolved_path);
        
        if (!file_exists(resolved_path.string()))
        {
            throw value_error_t("Module file not found: " + resolved_path.string());
        }
        
        return resolved_path.string();
    }
    else
    {
        // Name-based resolution
        std::string filename = specifier + ".zephyr";
        
        // Search in all search paths
        for (const auto& search_path : m_search_paths)
        {
            std::filesystem::path candidate_path = std::filesystem::path(search_path) / filename;
            candidate_path = std::filesystem::absolute(candidate_path);
            
            if (file_exists(candidate_path.string()))
            {
                return candidate_path.string();
            }
        }
        
        // Module not found
        throw value_error_t("Module '" + specifier + "' not found in search paths");
    }
}

auto module_loader_t::detect_circular_dependency(const std::string& module_path) -> void
{
    auto it = std::find(m_loading_stack.begin(), m_loading_stack.end(), module_path);
    if (it != m_loading_stack.end())
    {
        // Build circular dependency chain
        std::string chain = module_path;
        for (auto stack_it = it; stack_it != m_loading_stack.end(); ++stack_it)
        {
            chain += " -> " + *stack_it;
        }
        chain += " -> " + module_path;
        
        throw value_error_t("Circular dependency detected: " + chain);
    }
}

auto module_loader_t::file_exists(const std::string& path) const -> bool
{
    std::filesystem::path file_path(path);
    return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
}

auto module_loader_t::make_absolute_path(const std::string& path) const -> std::string
{
    return std::filesystem::absolute(std::filesystem::path(path)).string();
}

auto module_loader_t::read_file_content(const std::string& path) const -> std::string
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw value_error_t("Cannot open file: " + path);
    }
    
    // Read entire file content
    std::string content;
    std::string line;
    while (std::getline(file, line))
    {
        content += line + '\n';
    }
    
    return content;
}

} // namespace zephyr