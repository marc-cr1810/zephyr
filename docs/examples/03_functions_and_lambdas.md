# Zephyr Functions & Lambdas

This document provides detailed, well-worded descriptions and annotated examples for Zephyr's function definitions, lambdas, recursion, higher-order functions, closures, and lambdas in data structures.

---

## Functions

Functions are reusable blocks of code that accept parameters and return values. They help organize logic, promote code reuse, and improve readability.

```zephyr
func add(a, b) {
    return a + b
}
print(add(2, 3))    # Output: 5
```

Functions can have explicit parameter and return types for enhanced safety and clarity.

---

## Functions with Explicit Typing

Explicit typing in functions ensures that parameters and return values are of the expected type, catching errors early.

```zephyr
func multiply(a : int, b : int) : int {
    return a * b
}
```

Mixing typed and untyped parameters is supported for flexibility.

---

## Recursive Functions

Recursion allows a function to call itself, enabling elegant solutions for problems like factorial or Fibonacci.

```zephyr
func factorial(n) {
    if n <= 1 { return 1 }
    return n * factorial(n - 1)
}
print(factorial(5))    # Output: 120
```

Recursion is powerful for algorithms that naturally break down into smaller subproblems.

---

## Lambdas

Lambdas are anonymous functions defined with arrow syntax. They are concise and ideal for short, inline operations.

```zephyr
square = x -> x * x
print(square(5))    # Output: 25
```

Lambdas can be assigned to variables, passed as arguments, or stored in data structures.

---

## Lambdas with Block Bodies

Block-bodied lambdas allow for multi-line logic and local variables, making them suitable for more complex operations.

```zephyr
power = (base, exp) -> {
    result = 1
    for i = 1; i <= exp; i++ {
        result *= base
    }
    return result
}
print(power(2, 3))    # Output: 8
```

---

## Higher-Order Functions

Higher-order functions accept other functions as arguments, enabling powerful functional programming patterns.

```zephyr
numbers = [1,2,3]
doubled = map(numbers, x -> x * 2)
print(doubled)    # Output: [2, 4, 6]
```

This allows you to write generic, reusable code for data transformation.

---

## Closures

Closures are lambdas that capture variables from their surrounding scope, maintaining state across invocations.

```zephyr
factor = 10
multiplier = x -> x * factor
print(multiplier(2))    # Output: 20
```

Closures are useful for creating specialized functions or maintaining state.

---

## Lambda in Data Structures

Lambdas can be stored in dictionaries and lists for organized, dynamic functional programming. This enables flexible, data-driven function selection and supports advanced patterns such as function factories, pipelines, and recursive lambdas.

```zephyr
ops = {
    "add": (a, b) -> a + b,
    "mul": (a, b) -> a * b,
    "pow": (base, exp) -> {
        result = 1
        for i = 1; i <= exp; i++ { result *= base }
        return result
    }
}
print(ops["add"](2, 3))    # Output: 5
print(ops["mul"](4, 5))    # Output: 20
print(ops["pow"](2, 4))    # Output: 16

# Lambdas in lists
funcs = [
    x -> x + 1,
    x -> x * 2,
    x -> x * x
]
for f in funcs {
    print(f(3))    # Output: 4, 6, 9
}

# Function factories: lambdas that return other lambdas
make_multiplier = factor -> x -> x * factor
times5 = make_multiplier(5)
print(times5(7))   # Output: 35

# Recursive lambdas (self-referencing)
factorial = n -> {
    if n <= 1 { return 1 }
    return n * factorial(n - 1)
}
print(factorial(5)) # Output: 120

# Pipelines: chaining lambdas for staged transformations
pipeline = [
    x -> x * 2,
    x -> x + 10,
    x -> x / 3
]
value = 5
for stage in pipeline { value = stage(value) }
print(value) # Output: 6.666...

# Error handling with lambdas
safe_divide = (a, b) -> {
    if b == 0 { return none }
    return a / b
}
print(safe_divide(10, 0))  # Output: none

# Dynamic lambda creation based on runtime parameters
make_filter = kind -> {
    if kind == "even" { return x -> x % 2 == 0 }
    return x -> true
}
even_filter = make_filter("even")
print(even_filter(4))  # Output: true
```

---

## Summary

Zephyr's functions and lambdas provide powerful tools for abstraction, code reuse, and functional programming. With support for explicit typing, recursion, closures, and higher-order functions, you can write expressive, maintainable code for any task.
