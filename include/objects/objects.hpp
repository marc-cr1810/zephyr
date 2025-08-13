#pragma once

// Include base object and type
#include "object.hpp"

// Include all concrete objects
#include "none_object.hpp"
#include "int_object.hpp"
#include "float_object.hpp"
#include "string_object.hpp"
#include "boolean_object.hpp"
#include "list_object.hpp"
#include "dictionary_object.hpp"
#include "function_object.hpp"
#include "lambda_object.hpp"
#include "class_object.hpp"
#include "class_instance_object.hpp"
#include "builtin_function_object.hpp"
#include "promise_object.hpp"

namespace zephyr
{

// Forward declarations for all object types
class none_object_t;
class int_object_t;
class float_object_t;
class string_object_t;
class boolean_object_t;
class list_object_t;
class dictionary_object_t;
class function_object_t;
class lambda_object_t;
class class_object_t;
class class_instance_t;
class builtin_function_object_t;
class promise_object_t;

}
