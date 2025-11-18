#include "zephyr/types/file_type.hpp"

namespace zephyr {

// Static instance
std::shared_ptr<file_type_t> file_type_t::s_instance = nullptr;

auto file_type_t::get_instance() -> std::shared_ptr<file_type_t> {
    if (!s_instance) {
        s_instance = std::shared_ptr<file_type_t>(new file_type_t());
    }
    return s_instance;
}

auto file_type_t::name() const -> std::string {
    return "file";
}

auto file_type_t::to_string() const -> std::string {
    return "<type 'file'>";
}

} // namespace zephyr