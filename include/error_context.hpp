#pragma once

#include <string>

namespace zephyr
{

struct error_location_context_t
{
    int line = 0;
    int column = 0;
    int length = 1;
};

error_location_context_t& get_current_error_location();

} // namespace zephyr
