#include "types/list_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"
#include <algorithm>

namespace zephyr
{

auto list_type_t::get_instance() -> std::shared_ptr<list_type_t>
{
    static auto instance = std::shared_ptr<list_type_t>(new list_type_t());
    return instance;
}

auto list_type_t::add(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    if (auto other_list = std::dynamic_pointer_cast<list_object_t>(other))
    {
        auto self_list = std::static_pointer_cast<list_object_t>(self);
        std::vector<std::shared_ptr<object_t>> new_elements = self_list->elements();
        const auto& other_elements = other_list->elements();
        new_elements.insert(new_elements.end(), other_elements.begin(), other_elements.end());
        return std::make_shared<list_object_t>(new_elements);
    }
    throw type_error_t("Unsupported operand types for list concatenation");
}

auto list_type_t::name() const -> std::string
{
    return "list";
}

auto list_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto list_obj = std::static_pointer_cast<list_object_t>(self);
    return !list_obj->elements().empty();
}

auto list_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->type()->name() != "list")
    {
        return false;
    }

    auto self_list = std::static_pointer_cast<list_object_t>(self);
    auto other_list = std::static_pointer_cast<list_object_t>(other);

    const auto& self_elements = self_list->elements();
    const auto& other_elements = other_list->elements();

    if (self_elements.size() != other_elements.size())
    {
        return false;
    }

    for (size_t i = 0; i < self_elements.size(); ++i)
    {
        if (self_elements[i]->type()->name() != other_elements[i]->type()->name())
        {
            return false;
        }
    }

    return true;
}

auto list_type_t::item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    if (auto index_int = std::dynamic_pointer_cast<int_object_t>(index))
    {
        int idx = index_int->value();
        const auto& elements = self_list->elements();
        if (idx < 0) {
            idx += elements.size();
        }
        if (idx < 0 || idx >= static_cast<int>(elements.size()))
        {
            throw index_error_t("List index out of bounds");
        }
        return elements[idx];
    }
    throw type_error_t("List index must be an integer");
}

auto list_type_t::item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    if (auto index_int = std::dynamic_pointer_cast<int_object_t>(index))
    {
        int idx = index_int->value();
        auto& elements = self_list->elements_mutable();
        if (idx < 0) {
            idx += elements.size();
        }
        if (idx < 0 || idx >= static_cast<int>(elements.size()))
        {
            throw index_error_t("List index out of bounds");
        }
        elements[idx] = value;
        return;
    }
    throw type_error_t("List index must be an integer");
}

auto list_type_t::contains(std::shared_ptr<object_t> self, std::shared_ptr<object_t> item) -> bool
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    const auto& elements = self_list->elements();

    for (const auto& element : elements)
    {
        if (element->type()->equals(element, item))
        {
            return true;
        }
    }
    return false;
}

auto list_type_t::length(std::shared_ptr<object_t> self) -> int
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    return static_cast<int>(self_list->elements().size());
}

auto list_type_t::slice(std::shared_ptr<object_t> self,
                        std::shared_ptr<object_t> start,
                        std::shared_ptr<object_t> end,
                        std::shared_ptr<object_t> step) -> std::shared_ptr<object_t>
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    const auto& elements = self_list->elements();
    int list_length = static_cast<int>(elements.size());
    
    // Parse start index (default to 0)
    int start_idx = 0;
    if (start && start->type()->name() != "none") {
        if (auto start_int = std::dynamic_pointer_cast<int_object_t>(start)) {
            start_idx = start_int->value();
            if (start_idx < 0) {
                start_idx += list_length;
            }
            start_idx = std::max(0, std::min(start_idx, list_length));
        } else {
            throw type_error_t("Slice indices must be integers or None");
        }
    }
    
    // Parse end index (default to length)
    int end_idx = list_length;
    if (end && end->type()->name() != "none") {
        if (auto end_int = std::dynamic_pointer_cast<int_object_t>(end)) {
            end_idx = end_int->value();
            if (end_idx < 0) {
                end_idx += list_length;
            }
            end_idx = std::max(0, std::min(end_idx, list_length));
        } else {
            throw type_error_t("Slice indices must be integers or None");
        }
    }
    
    // Parse step (default to 1)
    int step_val = 1;
    if (step && step->type()->name() != "none") {
        if (auto step_int = std::dynamic_pointer_cast<int_object_t>(step)) {
            step_val = step_int->value();
            if (step_val == 0) {
                throw value_error_t("Slice step cannot be zero");
            }
        } else {
            throw type_error_t("Slice step must be an integer");
        }
    }
    
    // Build the result list
    std::vector<std::shared_ptr<object_t>> result;
    if (step_val > 0) {
        // Forward iteration
        for (int i = start_idx; i < end_idx; i += step_val) {
            result.push_back(elements[i]);
        }
    } else {
        // Backward iteration
        // When step is negative, swap the default start and end
        if (!start && !end) {
            start_idx = list_length - 1;
            end_idx = -1;
        } else if (!start) {
            start_idx = list_length - 1;
        } else if (!end) {
            end_idx = -1;
        }
        
        // Swap if necessary for negative step
        if (start_idx < end_idx && step_val < 0) {
            std::swap(start_idx, end_idx);
            end_idx--;
        }
        
        for (int i = start_idx; i > end_idx; i += step_val) {
            if (i >= 0 && i < list_length) {
                result.push_back(elements[i]);
            }
        }
    }
    
    return std::make_shared<list_object_t>(result);
}

} // namespace zephyr
