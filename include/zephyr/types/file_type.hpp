#pragma once

#include "zephyr/types/type.hpp"
#include <memory>

namespace zephyr {

class file_type_t : public type_t {
private:
    static std::shared_ptr<file_type_t> s_instance;
    
    file_type_t() = default;
    
public:
    static auto get_instance() -> std::shared_ptr<file_type_t>;
    
    auto name() const -> std::string override;
    auto to_string() const -> std::string;
};

} // namespace zephyr