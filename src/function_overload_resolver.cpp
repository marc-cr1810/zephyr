#include "zephyr/function_overload_resolver.hpp"
#include "zephyr/objects/function_object.hpp"
#include "zephyr/objects/object.hpp"
#include "zephyr/objects/class_instance_object.hpp"
#include "zephyr/objects/class_object.hpp"
#include "zephyr/types/type.hpp"
#include "zephyr/ast.hpp"
#include "zephyr/errors.hpp"
#include <algorithm>
#include <sstream>

namespace zephyr
{

// Function signature overload implementation
function_signature_overload_t::function_signature_overload_t(const std::string& func_name, 
                                                            const std::vector<parameter_t>& params,
                                                            std::shared_ptr<function_object_t> func)
    : name(func_name), function_obj(func), parameter_count(params.size()), has_typed_parameters(false)
{
    parameter_types.reserve(params.size());
    parameter_names.reserve(params.size());
    
    for (const auto& param : params)
    {
        parameter_names.push_back(param.name);
        parameter_types.push_back(param.type_name);
        
        if (param.has_explicit_type && !param.type_name.empty())
        {
            has_typed_parameters = true;
        }
    }
}

auto function_signature_overload_t::calculate_specificity_score() const -> int
{
    int score = 0;
    
    // Base score for having parameters
    score += parameter_count * 10;
    
    // Bonus for typed parameters
    for (const auto& type : parameter_types)
    {
        if (!type.empty())
        {
            score += 100; // Typed parameters are much more specific
        }
    }
    
    return score;
}

// Call signature implementation
call_signature_t::call_signature_t(const std::string& func_name, 
                                   const std::vector<std::shared_ptr<object_t>>& args)
    : name(func_name), argument_count(args.size())
{
    argument_types.reserve(args.size());
    argument_objects.reserve(args.size());
    
    for (const auto& arg : args)
    {
        argument_types.push_back(overload_utils::infer_type_from_object(arg));
        argument_objects.push_back(arg);
    }
}

// Function overload resolver implementation
auto function_overload_resolver_t::add_overload(const std::string& name, 
                                                const std::vector<parameter_t>& params,
                                                std::shared_ptr<function_object_t> function_obj) -> void
{
    function_signature_overload_t signature(name, params, function_obj);
    
    // Check for exact duplicate signatures
    auto& overloads = m_overloads[name];
    for (const auto& existing : overloads)
    {
        if (existing.parameter_count == signature.parameter_count)
        {
            bool same_signature = true;
            for (size_t i = 0; i < signature.parameter_count; ++i)
            {
                if (!overload_utils::types_equal(existing.parameter_types[i], signature.parameter_types[i]))
                {
                    same_signature = false;
                    break;
                }
            }
            
            if (same_signature)
            {
                throw type_error_t("Function '" + name + "' with signature (" + 
                                 overload_utils::format_signature(signature) + ") is already defined");
            }
        }
    }
    
    overloads.push_back(std::move(signature));
    
    // Sort by specificity (most specific first)
    std::sort(overloads.begin(), overloads.end(), 
              [](const function_signature_overload_t& a, const function_signature_overload_t& b) {
                  return a.calculate_specificity_score() > b.calculate_specificity_score();
              });
}

auto function_overload_resolver_t::remove_overloads(const std::string& name) -> void
{
    m_overloads.erase(name);
}

auto function_overload_resolver_t::resolve_call(const std::string& name, 
                                               const std::vector<std::shared_ptr<object_t>>& args) -> overload_resolution_result_t
{
    auto it = m_overloads.find(name);
    if (it == m_overloads.end())
    {
        return overload_resolution_result_t("Function '" + name + "' is not defined");
    }
    
    call_signature_t call_sig(name, args);
    return find_best_match(it->second, call_sig);
}

auto function_overload_resolver_t::has_function(const std::string& name) const -> bool
{
    return m_overloads.find(name) != m_overloads.end();
}

auto function_overload_resolver_t::get_overloads(const std::string& name) const -> std::vector<function_signature_overload_t>
{
    auto it = m_overloads.find(name);
    if (it == m_overloads.end())
    {
        return {};
    }
    return it->second;
}

auto function_overload_resolver_t::get_all_function_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(m_overloads.size());
    
    for (const auto& pair : m_overloads)
    {
        names.push_back(pair.first);
    }
    
    return names;
}

// Private methods
auto function_overload_resolver_t::is_type_compatible(const std::string& param_type, const std::string& arg_type) const -> bool
{
    // Empty param_type means untyped parameter - accepts anything
    if (param_type.empty())
    {
        return true;
    }
    
    // Exact match
    if (overload_utils::types_equal(param_type, arg_type))
    {
        return true;
    }
    
    // Implicit conversion
    if (overload_utils::is_implicitly_convertible(arg_type, param_type))
    {
        return true;
    }
    
    return false;
}

auto function_overload_resolver_t::calculate_type_match_score(const std::string& param_type, const std::string& arg_type) const -> int
{
    // Exact match gets highest priority
    if (!param_type.empty() && overload_utils::types_equal(param_type, arg_type))
    {
        return 1000; // Highest priority
    }
    
    // Empty param_type means untyped parameter - should be preferred over implicit conversions
    if (param_type.empty())
    {
        return 800; // High priority for untyped parameters when no exact match
    }
    
    // Implicit conversion gets lower priority
    if (overload_utils::is_implicitly_convertible(arg_type, param_type))
    {
        int distance = overload_utils::get_type_distance(arg_type, param_type);
        return 500 - distance; // Medium priority, closer types score higher
    }
    
    return -1; // No match
}

auto function_overload_resolver_t::calculate_type_match_score_with_object(const std::string& param_type, const std::string& arg_type, const std::shared_ptr<object_t>& arg_obj) const -> int
{
    // Exact match gets highest priority
    if (!param_type.empty() && overload_utils::types_equal(param_type, arg_type))
    {
        return 1000; // Highest priority
    }
    
    // Interface compatibility gets high priority (slightly lower than exact match)
    if (!param_type.empty() && overload_utils::is_interface_compatible(arg_obj, param_type))
    {
        return 950; // High priority for interface implementations
    }
    
    // Empty param_type means untyped parameter - should be preferred over implicit conversions
    if (param_type.empty())
    {
        return 800; // High priority for untyped parameters when no exact match
    }
    
    // Implicit conversion gets lower priority
    if (overload_utils::is_implicitly_convertible(arg_type, param_type))
    {
        int distance = overload_utils::get_type_distance(arg_type, param_type);
        return 500 - distance; // Medium priority, closer types score higher
    }
    
    return -1; // No match
}

auto function_overload_resolver_t::calculate_overload_score(const function_signature_overload_t& signature,
                                                           const call_signature_t& call) const -> int
{
    // Parameter count must match exactly
    if (signature.parameter_count != call.argument_count)
    {
        return -1; // No match
    }
    
    int total_score = 0;
    
    for (size_t i = 0; i < signature.parameter_count; ++i)
    {
        int param_score = calculate_type_match_score_with_object(signature.parameter_types[i], call.argument_types[i], call.argument_objects[i]);
        if (param_score < 0)
        {
            return -1; // No match if any parameter doesn't match
        }
        total_score += param_score;
    }
    
    return total_score;
}

auto function_overload_resolver_t::find_best_match(const std::vector<function_signature_overload_t>& candidates,
                                                   const call_signature_t& call) const -> overload_resolution_result_t
{
    std::vector<std::pair<const function_signature_overload_t*, int>> valid_matches;
    
    // Find all valid matches
    for (const auto& candidate : candidates)
    {
        int score = calculate_overload_score(candidate, call);
        if (score >= 0)
        {
            valid_matches.emplace_back(&candidate, score);
        }
    }
    
    if (valid_matches.empty())
    {
        return overload_resolution_result_t(generate_no_match_error(call.name, call, candidates));
    }
    
    // Sort by score (highest first)
    std::sort(valid_matches.begin(), valid_matches.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // Check for ambiguous calls (multiple matches with same highest score)
    if (valid_matches.size() > 1 && valid_matches[0].second == valid_matches[1].second)
    {
        std::stringstream ss;
        ss << "Ambiguous function call to '" << call.name << "' with arguments (" 
           << overload_utils::format_call(call) << "). Candidates:\n";
        
        for (const auto& match : valid_matches)
        {
            if (match.second == valid_matches[0].second)
            {
                ss << "  " << overload_utils::format_signature(*match.first) << "\n";
            }
        }
        
        return overload_resolution_result_t(ss.str());
    }
    
    // Return best match
    return overload_resolution_result_t(valid_matches[0].first->function_obj, valid_matches[0].second);
}

auto function_overload_resolver_t::generate_no_match_error(const std::string& name,
                                                          const call_signature_t& call,
                                                          const std::vector<function_signature_overload_t>& candidates) const -> std::string
{
    std::stringstream ss;
    ss << "No matching overload for function '" << name << "' with arguments (" 
       << overload_utils::format_call(call) << ").\n";
    
    if (!candidates.empty())
    {
        ss << "Available overloads:\n";
        for (const auto& candidate : candidates)
        {
            ss << "  " << overload_utils::format_signature(candidate) << "\n";
        }
    }
    
    return ss.str();
}

// Utility functions implementation
namespace overload_utils
{

auto infer_type_from_object(const std::shared_ptr<object_t>& obj) -> std::string
{
    if (!obj)
    {
        return "none";
    }
    
    return obj->type()->name();
}

auto is_interface_compatible(const std::shared_ptr<object_t>& obj, const std::string& interface_name) -> bool
{
    // Check if the object is a class instance that implements the interface
    auto class_instance = std::dynamic_pointer_cast<class_instance_t>(obj);
    if (class_instance)
    {
        auto class_obj = class_instance->m_class_obj;
        for (const auto& implemented_interface : class_obj->interfaces()) {
            if (implemented_interface == interface_name) {
                return true;
            }
        }
    }
    return false;
}

auto types_equal(const std::string& type1, const std::string& type2) -> bool
{
    // Handle empty types (untyped)
    if (type1.empty() && type2.empty())
    {
        return true;
    }
    
    return type1 == type2;
}

auto is_implicitly_convertible(const std::string& from_type, const std::string& to_type) -> bool
{
    // Basic implicit conversions
    if (from_type == "int" && to_type == "float")
    {
        return true;
    }
    
    if (from_type == "int" && to_type == "string")
    {
        return true;
    }
    
    if (from_type == "float" && to_type == "string")
    {
        return true;
    }
    
    if (from_type == "bool" && to_type == "string")
    {
        return true;
    }
    
    // Add more conversion rules as needed
    return false;
}

auto is_implicitly_convertible_with_object(const std::shared_ptr<object_t>& from_obj, const std::string& to_type) -> bool
{
    if (!from_obj) return false;
    
    std::string from_type = infer_type_from_object(from_obj);
    
    // Check basic implicit conversions first
    if (is_implicitly_convertible(from_type, to_type))
    {
        return true;
    }
    
    // Check interface compatibility for class instances
    if (is_interface_compatible(from_obj, to_type))
    {
        return true;
    }
    
    return false;
}

auto get_type_distance(const std::string& from_type, const std::string& to_type) -> int
{
    if (types_equal(from_type, to_type))
    {
        return 0;
    }
    
    // Define type distances for common conversions
    if (from_type == "int" && to_type == "float")
    {
        return 1;
    }
    
    if ((from_type == "int" || from_type == "float") && to_type == "string")
    {
        return 2;
    }
    
    if (from_type == "bool" && to_type == "string")
    {
        return 2;
    }
    
    // Unknown conversion - high distance
    return 100;
}

auto format_signature(const function_signature_overload_t& signature) -> std::string
{
    std::stringstream ss;
    ss << signature.name << "(";
    
    for (size_t i = 0; i < signature.parameter_count; ++i)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        
        ss << signature.parameter_names[i];
        
        if (!signature.parameter_types[i].empty())
        {
            ss << " : " << signature.parameter_types[i];
        }
    }
    
    ss << ")";
    return ss.str();
}

auto format_call(const call_signature_t& call) -> std::string
{
    std::stringstream ss;
    
    for (size_t i = 0; i < call.argument_count; ++i)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        ss << call.argument_types[i];
    }
    
    return ss.str();
}

} // namespace overload_utils

} // namespace zephyr