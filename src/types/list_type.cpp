#include "types/list_type.hpp"
#include "objects/objects.hpp"
#include "errors.hpp"

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
        std::vector<std::shared_ptr<object_t>> new_elements = self_list->get_elements();
        const auto& other_elements = other_list->get_elements();
        new_elements.insert(new_elements.end(), other_elements.begin(), other_elements.end());
        return std::make_shared<list_object_t>(new_elements);
    }
    throw type_error_t("Unsupported operand types for list concatenation");
}

auto list_type_t::subtract(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for lists");
}

auto list_type_t::multiply(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for lists");
}

auto list_type_t::divide(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for lists");
}

auto list_type_t::modulo(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> std::shared_ptr<object_t>
{
    throw type_error_t("Unsupported operation for lists");
}

auto list_type_t::get_name() const -> std::string
{
    return "list";
}

auto list_type_t::is_truthy(std::shared_ptr<object_t> self) -> bool
{
    auto list_obj = std::static_pointer_cast<list_object_t>(self);
    return !list_obj->get_elements().empty();
}

auto list_type_t::equals(std::shared_ptr<object_t> self, std::shared_ptr<object_t> other) -> bool
{
    if (other->get_type()->get_name() != "list")
    {
        return false;
    }

    auto self_list = std::static_pointer_cast<list_object_t>(self);
    auto other_list = std::static_pointer_cast<list_object_t>(other);

    const auto& self_elements = self_list->get_elements();
    const auto& other_elements = other_list->get_elements();

    if (self_elements.size() != other_elements.size())
    {
        return false;
    }

    for (size_t i = 0; i < self_elements.size(); ++i)
    {
        if (self_elements[i]->get_type()->get_name() != other_elements[i]->get_type()->get_name())
        {
            return false;
        }
    }

    return true;
}


auto list_type_t::get_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index) -> std::shared_ptr<object_t>
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    if (auto index_int = std::dynamic_pointer_cast<int_object_t>(index))
    {
        int idx = index_int->get_value();
        const auto& elements = self_list->get_elements();
        if (idx < 0 || idx >= static_cast<int>(elements.size()))
        {
            throw index_error_t("List index out of bounds");
        }
        return elements[idx];
    }
    throw type_error_t("List index must be an integer");
}

auto list_type_t::set_item(std::shared_ptr<object_t> self, std::shared_ptr<object_t> index, std::shared_ptr<object_t> value) -> void
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    if (auto index_int = std::dynamic_pointer_cast<int_object_t>(index))
    {
        int idx = index_int->get_value();
        auto& elements = self_list->get_elements_mutable();
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
    const auto& elements = self_list->get_elements();

    for (const auto& element : elements)
    {
        if (element->get_type()->equals(element, item))
        {
            return true;
        }
    }
    return false;
}

auto list_type_t::get_length(std::shared_ptr<object_t> self) -> int
{
    auto self_list = std::static_pointer_cast<list_object_t>(self);
    return static_cast<int>(self_list->get_elements().size());
}

} // namespace zephyr
