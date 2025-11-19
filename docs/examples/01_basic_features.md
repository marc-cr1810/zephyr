# Zephyr Basic Features

This document provides a thorough, well-explained overview of Zephyr's core language fundamentals and syntax. Each section includes detailed descriptions and annotated examples to help you understand not just how to use the features, but why they work the way they do.

---

## Variables and Data Types

Variables in Zephyr are dynamically typed by default, meaning you can assign any value to a variable without declaring its type. This flexibility allows rapid prototyping and easy experimentation.

```zephyr
x = 10                # Integer assignment
y = 3.14              # Floating-point assignment
name = "Alice"        # String assignment
active = true         # Boolean assignment
none_val = none       # None/null value, representing absence of data
```

For enhanced type safety and code clarity, Zephyr supports **explicit typing**. This lets you declare the intended type of a variable, ensuring only compatible values can be assigned.

```zephyr
score : int = 100
pi : float = 3.14159
user : string = "Bob"
flag : bool = false
```

Explicitly typed variables can be declared without an initial value, defaulting to `none` but retaining their type constraint:

```zephyr
email : string      # Defaults to none, but only strings can be assigned later
```

---

## Arithmetic and Comparison Operations

Zephyr provides a full suite of arithmetic operators for both integers and floats, supporting negative numbers and complex expressions.

```zephyr
a = 5 + 3           # Addition
b = 10 - 2          # Subtraction
c = 4 * 7           # Multiplication
d = 20 / 4          # Division (returns float if needed)
e = 17 % 5          # Modulo (remainder)

neg = -a            # Unary minus for negative numbers
neg_float = -3.5    # Negative float

# Comparison operators return booleans
equal = (a == b)        # Equality check
not_equal = (a != b)    # Inequality check
less = (a < b)          # Less than
greater = (a > b)       # Greater than
less_eq = (a <= b)      # Less than or equal
greater_eq = (a >= b)   # Greater than or equal

---

## Basic Error Handling

Zephyr provides robust exception handling to manage errors gracefully. This allows your programs to handle unexpected situations without crashing.

### Simple Try-Catch

```zephyr
try {
    # Code that might cause an error
    result = 10 / 0  # This would cause a ZeroDivisionError
    print("Result:", result)
} catch (e) {
    # Handle any error
    print("An error occurred:", e.message)
    print("Error type:", e.type)
}
```

### Handling Specific Error Types

You can catch specific types of errors using the `as` keyword:

```zephyr
try {
    numbers = [1, 2, 3]
    value = numbers[10]  # IndexError - trying to access non-existent index
} catch (e as IndexError) {
    print("Index out of bounds:", e.message)
} catch (e) {
    print("Some other error:", e.message)
}
```

### Creating and Throwing Exceptions

You can create and throw your own exceptions:

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

try {
    user_age = validate_age(-5)
} catch (e as ValueError) {
    print("Invalid age:", e.message)
}
```

### Always Execute Code with Finally

Use `finally` blocks to ensure cleanup code always runs:

```zephyr
try {
    # Open a file or resource
    data = load_important_data()
    process_data(data)
} catch (e) {
    print("Processing failed:", e.message)
} finally {
    # This always executes, even if there's an error
    print("Cleanup completed")
}
```

---
is_equal = a == b
is_not_equal = a != b
is_greater = a > b
is_less = a < b
is_ge = a >= b
is_le = a <= b
```

These operators work seamlessly with variables, constants, and expressions, allowing you to build complex calculations with ease.

---

## String Multiplication and Repetition

One of Zephyr's unique features is **string multiplication**, which allows you to repeat a string a specified number of times using the `*` operator. This is especially useful for formatting, creating patterns, or generating repeated content.

```zephyr
border = "=" * 40           # Creates a border of 40 equals signs
repeat = 3 * "Hi"           # "HiHiHi"
pattern = "*" * 10          # "**********"
empty = "abc" * 0           # "" (zero repetitions yields empty string)
negative = "abc" * -2       # "" (negative repetitions also yield empty string)
```

String multiplication works in both directions (`string * int` and `int * string`), and integrates with variables and expressions.

---

## Logical Operations

Zephyr supports both word-based (`and`, `or`, `not`) and symbol-based (`&&`, `||`, `!`) logical operators. These allow you to combine and invert boolean values for control flow and decision-making.

```zephyr
flag = true and false       # false
result = not flag           # true
value = x > 5 or y < 10     # true if either condition is true
```

The `or` operator is particularly useful for providing fallback values, as it returns the first "truthy" operand.

---

## Ternary Operator

The ternary operator provides a concise way to choose between two values based on a condition. It is especially useful for inline conditional assignments and expressions.

```zephyr
status = x > 10 ? "big" : "small"
```

This is equivalent to a full if/else statement, but much more concise:

```zephyr
if x > 10 {
    status = "big"
} else {
    status = "small"
}
```

The ternary operator can be nested or used in expressions, making it a powerful tool for readable, compact code.

---

## Control Flow

Zephyr's control flow constructs allow you to direct the execution of your program based on conditions and values. The `if` statement is used for branching logic, while the `switch` statement is ideal for multi-way branching based on a single value.

```zephyr
if x > 5 {
    print("x is large")
} else {
    print("x is small")
}

switch x {
    case 10:
        print("Ten")
        break
    case 20:
        print("Twenty")
    default:
        print("Other")
}
```

The `switch` statement supports fall-through behavior, meaning execution continues into subsequent cases unless a `break` is encountered. This allows for grouped case handling and more flexible control flow.

---

## Loops

Zephyr provides several looping constructs for iterating over ranges, collections, or indefinitely. The `for` loop is ideal for counting or iterating over ranges, while `while` and `loop` are suited for more flexible or infinite iteration.

```zephyr
for i = 0; i < 3; i++ {
    print(i)
}

while x < 20 {
    x += 1
}

loop {
    print("Infinite loop")
    break
}
```

Loops can be combined with control statements like `break` and `continue` for fine-grained control over iteration.

---

## Built-in Functions

Zephyr includes a rich set of built-in functions for common tasks, making your code concise and expressive. These functions are designed to work seamlessly with Zephyr's data types and structures.

```zephyr
print(len([1,2,3]))         # Prints the length of a list
print(type(name))           # Prints the type of a variable
print(str(x))               # Converts a value to string
print(int("42"))            # Converts a string to integer
print(float("3.14"))        # Converts a string to float
append(numbers, 4)          # Adds an item to a list
removed = pop(numbers, 1)   # Removes and returns item at index 1
keys_list = keys(info)      # Gets all keys from a dictionary
values_list = values(info)  # Gets all values from a dictionary
```

---

## Error Handling

Robust error handling is essential for reliable programs. Zephyr's `try`/`catch` construct lets you gracefully handle runtime errors and recover from unexpected situations. This ensures your program doesn't crash due to common errors like division by zero or type mismatches.

```zephyr
try {
    risky = 10 / 0
} catch e {
    print("Error:", e)
}
```

You can catch and handle any runtime error, making your code safer and more robust.

---

## Comments

Comments are essential for documenting your code and explaining your logic. Zephyr supports single-line comments starting with `#`.

```zephyr
# This is a comment explaining the next line
x = 42
```

Use comments liberally to make your code easier to understand and maintain.

---

## Unary Operators

Zephyr supports unary plus, minus, and logical not operators for concise value manipulation. These operators work with numbers and booleans, and can be combined for complex expressions.

```zephyr
a = -(-5)      # Double negation yields 5
b = +(+3)      # Double plus yields 3
c = !false     # Logical not yields true
```

---

## Summary

Zephyr's basic features are designed for clarity, flexibility, and expressiveness. With dynamic typing, optional explicit typing, powerful operators, and robust control flow, you can write concise, readable code for any task. Each feature integrates smoothly with the others, allowing you to build complex programs with minimal effort.

For more advanced features, see the other example documentation files.
