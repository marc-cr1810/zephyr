# Exception Handling in Zephyr

This guide covers Zephyr's comprehensive exception handling system, which provides robust error management capabilities similar to modern programming languages like Python and Java.

## Table of Contents

1. [Basic Concepts](#basic-concepts)
2. [Exception Objects](#exception-objects)
3. [Try-Catch Syntax](#try-catch-syntax)
4. [Finally Blocks](#finally-blocks)
5. [Typed Exception Catching](#typed-exception-catching)
6. [Multiple Catch Blocks](#multiple-catch-blocks)
7. [Throwing Exceptions](#throwing-exceptions)
8. [Control Flow Integration](#control-flow-integration)
9. [Best Practices](#best-practices)
10. [Common Patterns](#common-patterns)

## Basic Concepts

Exception handling in Zephyr allows you to gracefully manage errors and exceptional conditions in your programs. Instead of letting your program crash, you can catch exceptions and handle them appropriately.

### Simple Try-Catch

```zephyr
try {
    # Code that might throw an exception
    risky_operation()
} catch (e) {
    # Handle the exception
    print("An error occurred:", e.message)
}
```

## Exception Objects

Exceptions in Zephyr are first-class objects with properties that provide information about the error.

### Exception Properties

```zephyr
try {
    throw RuntimeError("Something went wrong")
} catch (e) {
    print("Exception type:", e.type)      # "RuntimeError"
    print("Exception message:", e.message) # "Something went wrong"
    print("Exception cause:", e.cause)     # none (or another exception)
    print("Full exception:", e)            # "RuntimeError: Something went wrong"
}
```

### Creating Exception Objects

You can create exception objects in several ways:

#### Convenience Constructors (Recommended)
```zephyr
# Built-in exception types
index_err = IndexError("Array index out of bounds")
type_err = TypeError("Cannot convert string to number")
value_err = ValueError("Invalid input value")
key_err = KeyError("Dictionary key not found")
attr_err = AttributeError("Object has no such attribute")
zero_div_err = ZeroDivisionError("Division by zero")
overflow_err = OverflowError("Number too large")
runtime_err = RuntimeError("General runtime error")
```

#### Generic Constructor
```zephyr
# Using the generic Exception constructor
ex1 = Exception("RuntimeError", "Custom error message")
ex2 = Exception("TypeError", "Type mismatch")
```

## Try-Catch Syntax

### Basic Try-Catch

```zephyr
try {
    # Code that might fail
    result = risky_calculation()
    print("Success:", result)
} catch (e) {
    # Handle any exception
    print("Error:", e.message)
}
```

### Blank Catch (No Exception Variable)

Sometimes you want to catch exceptions without accessing the exception object:

```zephyr
try {
    attempt_operation()
} catch {
    # Handle exception without accessing it
    print("Operation failed, but continuing...")
}
```

## Finally Blocks

Finally blocks execute regardless of whether an exception occurs, making them perfect for cleanup code.

### Try-Catch-Finally

```zephyr
try {
    file = open("data.txt")
    process_file(file)
} catch (e) {
    print("File processing failed:", e.message)
} finally {
    # This always executes
    if file != none {
        file.close()
    }
    print("Cleanup completed")
}
```

### Try-Finally (Without Catch)

```zephyr
try {
    acquire_resource()
    do_work()
} finally {
    # Always cleanup, even if exception propagates
    release_resource()
}
```

## Typed Exception Catching

Use the `as` keyword to catch specific exception types:

### Single Typed Catch

```zephyr
try {
    items = [1, 2, 3]
    value = items[10]  # IndexError
} catch (e as IndexError) {
    print("Index out of bounds:", e.message)
}
```

### Type Validation

```zephyr
try {
    user_input = get_user_input()
    number = int(user_input)
} catch (e as ValueError) {
    print("Invalid number format:", e.message)
} catch (e as TypeError) {
    print("Type conversion failed:", e.message)
}
```

## Multiple Catch Blocks

Chain multiple catch blocks to handle different exception types:

```zephyr
func safe_divide(a, b) {
    try {
        if b == 0 {
            throw ZeroDivisionError("Cannot divide by zero")
        }
        return a / b
    } catch (e as ZeroDivisionError) {
        print("Division by zero detected")
        return 0
    } catch (e as TypeError) {
        print("Invalid types for division")
        return none
    } catch (e) {
        print("Unexpected error:", e.message)
        return none
    }
}

# Usage
print(safe_divide(10, 2))    # 5
print(safe_divide(10, 0))    # Division by zero detected, returns 0
print(safe_divide("a", 2))   # Invalid types, returns none
```

## Throwing Exceptions

### Throwing Built-in Exceptions

```zephyr
func validate_age(age) {
    if age < 0 {
        throw ValueError("Age cannot be negative")
    }
    if age > 150 {
        throw ValueError("Age seems unrealistic")
    }
    return age
}
```

### Throwing Custom Exceptions

```zephyr
func divide_numbers(a, b) {
    try {
        if b == 0 {
            throw ZeroDivisionError("Division by zero is not allowed")
        }
        return a / b
    } catch (e) {
        # Re-throw with additional context
        throw RuntimeError("Math operation failed: " + e.message)
    }
}
```

## Control Flow Integration

Exception handling properly integrates with control flow statements.

### Return in Try-Finally

```zephyr
func safe_operation() {
    try {
        perform_operation()
        return "success"
    } finally {
        # This executes before the return
        cleanup()
    }
}
```

### Break and Continue in Loops

```zephyr
for item in items {
    try {
        if should_skip(item) {
            continue  # Finally blocks still execute
        }
        if should_stop(item) {
            break     # Finally blocks still execute
        }
        process_item(item)
    } catch (e) {
        print("Failed to process", item, ":", e.message)
        continue
    } finally {
        cleanup_item(item)  # Always executes
    }
}
```

## Best Practices

### 1. Use Specific Exception Types

```zephyr
# Good: Specific exception types
func validate_email(email) {
    if email == none {
        throw ValueError("Email cannot be none")
    }
    if "@" not in email {
        throw ValueError("Invalid email format")
    }
    return email
}
```

### 2. Provide Meaningful Error Messages

```zephyr
# Good: Descriptive error messages
func get_user_by_id(user_id) {
    if user_id < 1 {
        throw ValueError("User ID must be a positive integer, got: " + str(user_id))
    }
    # ... rest of function
}
```

### 3. Use Finally for Cleanup

```zephyr
# Good: Resource management with finally
func process_file(filename) {
    file = none
    try {
        file = open(filename)
        return process_data(file.read())
    } catch (e as IOError) {
        print("File error:", e.message)
        return none
    } finally {
        if file != none {
            file.close()
        }
    }
}
```

### 4. Don't Catch and Ignore

```zephyr
# Bad: Silently ignoring exceptions
try {
    risky_operation()
} catch {
    # Don't do this - at least log the error
}

# Good: Handle appropriately
try {
    risky_operation()
} catch (e) {
    log_error("Operation failed", e)
    # Take appropriate action
}
```

## Common Patterns

### 1. Input Validation

```zephyr
func validate_user_input(data) {
    try {
        if data == none {
            throw ValueError("Input data cannot be none")
        }
        
        if len(str(data)) == 0 {
            throw ValueError("Input data cannot be empty")
        }
        
        if len(str(data)) < 3 {
            throw ValueError("Input data too short (minimum 3 characters)")
        }
        
        return str(data)
    } catch (e as ValueError) {
        print("Validation failed:", e.message)
        throw RuntimeError("User input validation failed: " + e.message)
    }
}
```

### 2. Safe Dictionary Access

```zephyr
func safe_dict_get(dict, key, default_value) {
    try {
        if key in dict {
            return dict[key]
        } else {
            throw KeyError("Key '" + str(key) + "' not found")
        }
    } catch (e as KeyError) {
        print("Using default value for missing key:", key)
        return default_value
    } finally {
        print("Dictionary access completed")
    }
}

# Usage
user_data = {"name": "Alice", "age": 30}
name = safe_dict_get(user_data, "name", "Unknown")      # "Alice"
city = safe_dict_get(user_data, "city", "Not specified") # "Not specified"
```

### 3. Retry Pattern

```zephyr
func retry_operation(max_attempts) {
    attempts = 0
    while attempts < max_attempts {
        attempts = attempts + 1
        try {
            return perform_network_operation()
        } catch (e as NetworkError) {
            if attempts >= max_attempts {
                throw RuntimeError("Operation failed after " + str(max_attempts) + " attempts: " + e.message)
            }
            print("Attempt", attempts, "failed, retrying...")
            sleep(1)  # Wait before retry
        }
    }
}
```

### 4. Exception Chaining

```zephyr
func high_level_operation(data) {
    try {
        return low_level_operation(data)
    } catch (e) {
        # Add context to the original exception
        context_msg = "High-level operation failed with input: " + str(data)
        throw RuntimeError(context_msg + " | Original error: " + e.message)
    }
}

func low_level_operation(data) {
    if data == none {
        throw ValueError("Low-level operation requires non-none data")
    }
    # ... processing
}
```

### 5. Batch Processing with Error Handling

```zephyr
func process_batch(items) {
    results = []
    errors = []
    
    for i, item in enumerate(items) {
        try {
            result = process_single_item(item)
            append(results, result)
        } catch (e) {
            error_info = {
                "index": i,
                "item": item,
                "error": e.type,
                "message": e.message
            }
            append(errors, error_info)
            print("Failed to process item", i, ":", e.message)
        }
    }
    
    return {
        "results": results,
        "errors": errors,
        "success_count": len(results),
        "error_count": len(errors)
    }
}
```

## Integration with Existing Features

### With Async Functions

```zephyr
async func async_operation_with_exceptions() {
    try {
        result = await risky_async_operation()
        return result
    } catch (e as NetworkError) {
        print("Network error in async operation:", e.message)
        return await fallback_operation()
    } finally {
        print("Async operation cleanup")
    }
}
```

### With Classes

```zephyr
class SafeCalculator {
    func divide(a, b) {
        try {
            if b == 0 {
                throw ZeroDivisionError("SafeCalculator: Cannot divide by zero")
            }
            return a / b
        } catch (e) {
            print("Calculator error:", e.message)
            return 0
        }
    }
    
    func validate_input(value) {
        if type(value) != "int" and type(value) != "float" {
            throw TypeError("SafeCalculator only accepts numbers")
        }
        return value
    }
}
```

## Summary

Zephyr's exception handling system provides:

- **Robust Error Management**: Catch and handle exceptions gracefully
- **Type Safety**: Use typed catches to handle specific exception types
- **Resource Management**: Finally blocks ensure cleanup code runs
- **Control Flow Integration**: Proper handling of return, break, and continue
- **Unified System**: Consistent exception objects across C++ and Zephyr code
- **Developer Friendly**: Clear error messages and flexible syntax

This system enables you to write robust, maintainable code that handles errors gracefully while maintaining excellent performance and developer experience.