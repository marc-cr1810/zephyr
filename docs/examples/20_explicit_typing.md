# Zephyr Explicit Typing

This document describes Zephyr's optional explicit typing system for enhanced type safety, including variable type declarations, typed constants, function/lambda/class parameter typing, mixed typing, user-defined types, none assignment, type inheritance, async integration, validation, and real-world usage.

---

## Overview

Explicit typing in Zephyr allows you to declare the intended type of variables, constants, function parameters, and class members. This provides robust type safety while retaining dynamic flexibility and none compatibility.

---

## Basic Variable Type Declarations

**Description:**  
Declare variables with explicit types for clarity and safety.

```zephyr
x : string = "hello"
y : int = 42
z : float = 3.14
flag : bool = true
```

---

## Typed Constant Declarations

**Description:**  
Declare immutable constants with explicit types.

```zephyr
const pi : float = 3.14159
const name : string = "Zephyr"
```

---

## Function Parameters with Explicit Types

**Description:**  
Specify parameter and return types for functions and methods.

```zephyr
func add(a : int, b : int) : int {
    return a + b
}
```

---

## Lambda Parameters with Explicit Typing

**Description:**  
Create typed lambda functions for safe, expressive functional programming.

```zephyr
multiply = (a : int, b : int) -> a * b
```

---

## Mixed Typed and Untyped Parameters

**Description:**  
Mix typed and untyped parameters for flexible function signatures.

```zephyr
func process(a : int, b, c : string) {
    print(a, b, c)
}
```

---

## User-Defined Class Types

**Description:**  
Use custom class types in variable and parameter declarations.

```zephyr
class Person {
    name : string
    age : int
}
person : Person = Person()
```

---

## None Assignment with Type Constraint Retention

**Description:**  
Assign `none` to typed variables while retaining type constraints.

```zephyr
x : int = none
x = 5
```

---

## Type Inheritance Across Scopes

**Description:**  
Maintain type constraints across function and control structure scopes.

```zephyr
func outer(a : int) {
    func inner(b : int) {
        print(a, b)
    }
    inner(10)
}
outer(5)
```

---

## Async Functions and Lambdas with Typed Parameters

**Description:**  
Use explicit typing in async functions and lambdas for safe concurrency.

```zephyr
async func fetch(id : int) : string {
    return "User " + str(id)
}
async_lambda = async (x : int) -> x * 2
print(await async_lambda(5))
```

---

## Comprehensive Type Validation and Error Handling

**Description:**  
Catch and report type errors with clear messages and location info.

```zephyr
x : int = 10
try { x = "oops" } catch e { print("Type error:", e) }
```

---

## Real-World Usage Patterns

**Description:**  
Apply explicit typing in practical applications for maintainable, robust code.

```zephyr
class Product {
    name : string
    price : float
}
p : Product = Product()
p.name = "Book"
p.price = 12.99
```
