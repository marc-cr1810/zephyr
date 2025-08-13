# Zephyr Lambda Dictionaries & Nested Calls

This document explains advanced usage of lambda functions in Zephyr, including storing lambdas in lists and dictionaries, nested and recursive calls, function factories, composition, pipelines, error handling, and safe operation patterns.

---

## Overview

Lambda functions in Zephyr are first-class citizens. You can store them in any data structure, pass them around, compose them, and use them for advanced functional programming. This enables dynamic behavior, pipelines, and safe operations in complex systems.

---

## Lambdas in Lists of Dictionaries

**Description:**  
Store multiple lambda functions in lists and dictionaries for organized, dynamic access.

```zephyr
ops = [
    {"add": (a, b) -> a + b, "mul": (a, b) -> a * b},
    {"sub": (a, b) -> a - b, "div": (a, b) -> a / b}
]
print(ops[0]["add"](2, 3))
```

---

## Nested Lambda Calls

**Description:**  
Create higher-order functions and factories that return other lambdas, enabling currying and composition.

```zephyr
func_factory = x -> y -> x + y
adder = func_factory(10)
print(adder(5))  # 15
```

---

## Recursive Lambda Functions

**Description:**  
Lambdas can reference themselves for recursion, supporting algorithms like factorial and Fibonacci.

```zephyr
factorial = n -> {
    if n <= 1 { return 1 }
    return n * factorial(n - 1)
}
print(factorial(5))
```

---

## Lambda Function Factories

**Description:**  
Generate specialized lambda functions at runtime for flexible, reusable logic.

```zephyr
make_multiplier = factor -> x -> x * factor
times3 = make_multiplier(3)
print(times3(7))  # 21
```

---

## Function Composition and Currying

**Description:**  
Compose multiple functions into pipelines, or curry functions for partial application.

```zephyr
compose = (f, g) -> x -> f(g(x))
double = x -> x * 2
inc = x -> x + 1
composed = compose(double, inc)
print(composed(5))  # 12
```

---

## Pipeline Processing

**Description:**  
Chain multiple lambda transformations for staged data processing.

```zephyr
pipeline = [
    x -> x * 2,
    x -> x + 10,
    x -> x / 3
]
value = 5
for stage in pipeline { value = stage(value) }
print(value)
```

---

## Error Handling Patterns

**Description:**  
Implement safe operations with lambdas that return structured error results.

```zephyr
safe_divide = (a, b) -> {
    if b == 0 { return none }
    return a / b
}
print(safe_divide(10, 0))  # none
```

---

## Complex Nested Data Structures

**Description:**  
Integrate lambdas into deeply nested data structures for dynamic, context-sensitive behavior.

```zephyr
data = [
    {"func": x -> x * x, "val": 3},
    {"func": x -> x + 10, "val": 5}
]
for item in data {
    print(item["func"](item["val"]))
}
```

---

## Dynamic Lambda Creation

**Description:**  
Create lambdas based on runtime parameters for flexible filtering and transformation.

```zephyr
make_filter = kind -> {
    if kind == "even" { return x -> x % 2 == 0 }
    return x -> true
}
even_filter = make_filter("even")
print(even_filter(4))  # true
```

---

## Higher-Order Functions

**Description:**  
Pass lambdas as arguments to other functions for generic, reusable code.

```zephyr
apply = (f, x) -> f(x)
print(apply(x -> x * 2, 7))  # 14
```

---

## Safe Operation Patterns

**Description:**  
Use lambdas to safely access data and handle errors gracefully.

```zephyr
safe_access = (lst, idx) -> {
    if idx < 0 or idx >= len(lst) { return none }
    return lst[idx]
}
print(safe_access([1,2,3], 5))  # none
```
