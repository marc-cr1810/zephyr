# Function Overloading in Zephyr

Function overloading is a powerful feature that allows you to define multiple functions with the same name but different parameter signatures. The Zephyr interpreter automatically selects the best matching function based on the number and types of arguments provided.

## Table of Contents

1. [Basic Function Overloading](#basic-function-overloading)
2. [Type-Based Overloading](#type-based-overloading)
3. [Class Method Overloading](#class-method-overloading)
4. [Overload Resolution Rules](#overload-resolution-rules)
5. [Advanced Examples](#advanced-examples)
6. [Error Handling](#error-handling)
7. [Best Practices](#best-practices)

## Basic Function Overloading

You can define multiple functions with the same name but different parameter counts:

```zephyr
func greet() {
    return "Hello, World!"
}

func greet(name) {
    return "Hello, " + name + "!"
}

func greet(name, title) {
    return "Hello, " + title + " " + name + "!"
}

print(greet())                    # "Hello, World!"
print(greet("Alice"))             # "Hello, Alice!"
print(greet("Smith", "Dr."))      # "Hello, Dr. Smith!"
```

## Type-Based Overloading

Functions can be overloaded based on parameter types when explicit type annotations are provided:

```zephyr
func process(value : int) {
    return "Processing integer: " + str(value * 2)
}

func process(value : string) {
    return "Processing string: " + value + " (length: " + str(len(value)) + ")"
}

func process(value : float) {
    return "Processing float: " + str(value * 1.5)
}

func process(value) {
    return "Processing unknown type: " + str(value)
}

print(process(10))        # "Processing integer: 20"
print(process("hello"))   # "Processing string: hello (length: 5)"
print(process(3.14))      # "Processing float: 4.71"
print(process(true))      # "Processing unknown type: true"
```

## Class Method Overloading

Class methods can also be overloaded using the same principles:

```zephyr
class Calculator {
    func init() {
        this.name = "Calculator"
    }

    func add(a : int, b : int) {
        return a + b
    }

    func add(a : float, b : float) {
        return a + b
    }

    func add(a : string, b : string) {
        return a + b
    }

    func add(a) {
        return a + a  # Double the value
    }
}

calc = Calculator()
print(calc.add(5, 3))           # 8 (int version)
print(calc.add(2.5, 1.5))       # 4.0 (float version)
print(calc.add("Hello", " World")) # "Hello World" (string version)
print(calc.add(10))             # 20 (single parameter version)
```

## Overload Resolution Rules

The Zephyr interpreter uses a sophisticated algorithm to resolve function calls to the best matching overload:

### 1. Parameter Count Matching

Functions must have the exact same number of parameters as arguments provided.

### 2. Type Matching Priority (from highest to lowest)

1. **Exact type match** (score: 1000) - Parameter type exactly matches argument type
2. **Untyped parameter match** (score: 800) - Parameter has no type annotation
3. **Implicit conversion match** (score: 500 - distance) - Argument can be implicitly converted to parameter type

### 3. Implicit Conversions Supported

- `int` → `float`
- `int` → `string`
- `float` → `string`
- `bool` → `string`

### 4. Ambiguity Detection

If multiple functions have the same highest score, an ambiguity error is thrown.

## Advanced Examples

### Mixed Parameter Types

```zephyr
func calculate(a : int, b : string) {
    return "int + string: " + str(a) + " " + b
}

func calculate(a : string, b : int) {
    return "string + int: " + a + " " + str(b)
}

func calculate(a, b, c) {
    return "three parameters: " + str(a) + ", " + str(b) + ", " + str(c)
}

print(calculate(42, "test"))      # "int + string: 42 test"
print(calculate("hello", 99))     # "string + int: hello 99"
print(calculate(1, 2, 3))        # "three parameters: 1, 2, 3"
```

### Nested Scope Overloading

Functions can be overloaded within nested scopes:

```zephyr
func outer() {
    func inner(x : int) {
        return "Inner int: " + str(x)
    }

    func inner(x : string) {
        return "Inner string: " + x
    }

    return [inner(10), inner("test")]
}

results = outer()  # ["Inner int: 10", "Inner string: test"]
```

## Error Handling

### No Matching Overload

```zephyr
func test(a : int) {
    return "integer version"
}

# This will throw an error:
# test("string")  # TypeError: No matching overload for function 'test'
```

### Ambiguous Call

```zephyr
func ambiguous(a : int) {
    return "first"
}

func ambiguous(a : float) {
    return "second" 
}

# This might be ambiguous depending on implicit conversion rules
# ambiguous(3.0)  # Could match either int or float version
```

### Duplicate Signature

```zephyr
func duplicate(a : int) {
    return "first"
}

# This will throw an error:
# func duplicate(a : int) {  # TypeError: Function 'duplicate' with signature already defined
#     return "second"
# }
```

## Best Practices

### 1. Use Explicit Types for Overloading

```zephyr
# Good: Clear type-based overloading
func format(value : int) { return str(value) }
func format(value : string) { return "\"" + value + "\"" }

# Less clear: Relies on runtime behavior
func format(value) { 
    if (type(value) == "int") return str(value)
    # ...
}
```

### 2. Provide Untyped Fallback

```zephyr
func process(value : int) {
    return "Specialized int processing: " + str(value)
}

func process(value : string) {
    return "Specialized string processing: " + value
}

func process(value) {
    return "Generic processing: " + str(value)
}
```

### 3. Keep Overloads Consistent

```zephyr
# Good: Consistent behavior across overloads
func calculate(a : int) { return a * a }
func calculate(a : float) { return a * a }
func calculate(a : int, b : int) { return a + b }

# Avoid: Inconsistent behavior
# func calculate(a : string) { return len(a) }  # Different operation
```

### 4. Document Complex Overloads

```zephyr
# Converts various types to formatted strings
func stringify(value : int) {
    # Integer formatting with thousand separators
    return format_number(value)
}

func stringify(value : float) {
    # Float formatting with 2 decimal places
    return format_decimal(value, 2)
}

func stringify(value) {
    # Generic string conversion
    return str(value)
}
```

## Implementation Details

The function overloading system in Zephyr is implemented using:

- **Function Overload Resolver**: Manages multiple function signatures per name
- **Signature Matching**: Compares argument types with parameter types
- **Scope-Aware Resolution**: Each scope level has its own overload resolver
- **Priority Scoring**: Assigns scores to potential matches and selects the best one

This system seamlessly integrates with:
- Global functions
- Class methods (including constructors)
- Interface definitions
- Nested scopes
- Lambda functions (stored as variables)

Function overloading makes Zephyr code more expressive and allows for cleaner APIs while maintaining type safety where desired.