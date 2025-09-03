#include "error_context.hpp"

namespace zephyr
{
thread_local error_location_context_t current_error_location_impl;

error_location_context_t& current_error_location()
{
    return current_error_location_impl;
}
} // namespace zephyr
