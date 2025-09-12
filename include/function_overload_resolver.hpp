#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace zephyr
{

// Forward declarations
class object_t;
class function_object_t;
class class_instance_t;
struct parameter_t;

/**
 * Represents a function signature for overload resolution
 */
struct function_signature_overload_t
{
    std::string name;
    std::vector<std::string> parameter_types;
    std::vector<std::string> parameter_names;
    std::size_t parameter_count;
    bool has_typed_parameters;
    std::shared_ptr<function_object_t> function_obj;
    
    function_signature_overload_t(const std::string& func_name, 
                                  const std::vector<parameter_t>& params,
                                  std::shared_ptr<function_object_t> func);
    
    // Calculate specificity score for overload resolution
    auto calculate_specificity_score() const -> int;
};

/**
 * Represents a function call signature for matching
 */
struct call_signature_t
{
    std::string name;
    std::vector<std::string> argument_types;
    std::vector<std::shared_ptr<object_t>> argument_objects;
    std::size_t argument_count;
    
    call_signature_t(const std::string& func_name, 
                     const std::vector<std::shared_ptr<object_t>>& args);
};

/**
 * Result of overload resolution
 */
struct overload_resolution_result_t
{
    std::shared_ptr<function_object_t> selected_function;
    bool found_match;
    int specificity_score;
    std::string error_message;
    
    overload_resolution_result_t() : found_match(false), specificity_score(-1) {}
    overload_resolution_result_t(std::shared_ptr<function_object_t> func, int score)
        : selected_function(func), found_match(true), specificity_score(score) {}
    overload_resolution_result_t(const std::string& error)
        : found_match(false), specificity_score(-1), error_message(error) {}
};

/**
 * Manager for function overloads
 */
class function_overload_resolver_t
{
public:
    function_overload_resolver_t() = default;
    
    // Add a function overload
    auto add_overload(const std::string& name, 
                      const std::vector<parameter_t>& params,
                      std::shared_ptr<function_object_t> function_obj) -> void;
    
    // Remove all overloads for a function name
    auto remove_overloads(const std::string& name) -> void;
    
    // Resolve function call to best matching overload
    auto resolve_call(const std::string& name, 
                      const std::vector<std::shared_ptr<object_t>>& args) -> overload_resolution_result_t;
    
    // Check if function name has any overloads
    auto has_function(const std::string& name) const -> bool;
    
    // Get all overloads for a function name
    auto get_overloads(const std::string& name) const -> std::vector<function_signature_overload_t>;
    
    // Get all function names
    auto get_all_function_names() const -> std::vector<std::string>;
    
private:
    std::map<std::string, std::vector<function_signature_overload_t>> m_overloads;
    
    // Type compatibility checking
    auto is_type_compatible(const std::string& param_type, const std::string& arg_type) const -> bool;
    auto calculate_type_match_score(const std::string& param_type, const std::string& arg_type) const -> int;
    auto calculate_type_match_score_with_object(const std::string& param_type, const std::string& arg_type, const std::shared_ptr<object_t>& arg_obj) const -> int;
    
    // Overload resolution scoring
    auto calculate_overload_score(const function_signature_overload_t& signature,
                                  const call_signature_t& call) const -> int;
    
    // Find best matching overload
    auto find_best_match(const std::vector<function_signature_overload_t>& candidates,
                         const call_signature_t& call) const -> overload_resolution_result_t;
    
    // Generate error messages for failed resolution
    auto generate_no_match_error(const std::string& name,
                                 const call_signature_t& call,
                                 const std::vector<function_signature_overload_t>& candidates) const -> std::string;
};

/**
 * Utility functions for type inference and conversion
 */
namespace overload_utils
{
    // Infer type from object
    auto infer_type_from_object(const std::shared_ptr<object_t>& obj) -> std::string;
    
    // Check if object implements interface
    auto is_interface_compatible(const std::shared_ptr<object_t>& obj, const std::string& interface_name) -> bool;
    
    // Check if two types are exactly the same
    auto types_equal(const std::string& type1, const std::string& type2) -> bool;
    
    // Check if type1 can be implicitly converted to type2
    auto is_implicitly_convertible(const std::string& from_type, const std::string& to_type) -> bool;
    
    // Check if object can be implicitly converted to type (includes interface compatibility)
    auto is_implicitly_convertible_with_object(const std::shared_ptr<object_t>& from_obj, const std::string& to_type) -> bool;
    
    // Get type hierarchy distance (for inheritance-based matching)
    auto get_type_distance(const std::string& from_type, const std::string& to_type) -> int;
    
    // Format function signature for error messages
    auto format_signature(const function_signature_overload_t& signature) -> std::string;
    auto format_call(const call_signature_t& call) -> std::string;
}

} // namespace zephyr