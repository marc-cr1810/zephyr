# Zephyr Async Comprehensive Test

This document covers advanced async/await usage, including REPL-like patterns, edge cases, lambda variable reassignment, complex async lambda bodies, stress testing, data structure integration, error handling, and memory safety validation.

---

## Overview

Comprehensive async testing ensures that Zephyr's asynchronous features are robust, flexible, and safe. This includes handling edge cases, integrating with data structures, and validating error recovery.

---

## REPL-like Usage Patterns

**Description:**  
Interactively test async functions and await their results.

```zephyr
async func echo(x) { return x }
print(await echo("Hello"))
```

---

## Lambda Variable Reassignment and Scoping

**Description:**  
Reassign async lambdas and test variable scope in asynchronous contexts.

```zephyr
async func test_scope() {
    f = async x -> x + 1
    print(await f(5))
    f = async x -> x * 10
    print(await f(5))
}
await test_scope()
```

---

## Complex Async Lambda Bodies and Nested Calls

**Description:**  
Build complex async workflows with nested lambdas and function calls.

```zephyr
async func outer(x) {
    inner = async y -> y * 2
    return await inner(x) + 1
}
print(await outer(10))  # Output: 21
```

---

## Stress Testing with Multiple Async Operations

**Description:**  
Run large numbers of async tasks in parallel to test performance and reliability.

```zephyr
async func heavy(n) { return n * n }
promises = []
for i = 1; i <= 100; i++ { append(promises, spawn heavy(i)) }
results = await all(promises)
print("Sum:", reduce(results, (a, b) -> a + b, 0))
```

---

## Data Structure Integration with Async Lambdas

**Description:**  
Store and invoke async lambdas from lists, dictionaries, and class members.

```zephyr
processors = [
    async x -> x * 2,
    async x -> x + 100
]
for p in processors { print(await p(5)) }
```

---

## Error Handling Across Async Scenarios

**Description:**  
Catch and recover from errors in async functions, lambdas, and workflows.

```zephyr
async func risky(n) {
    if n == 0 { return 1 / 0 }
    return n
}
try {
    print(await risky(0))
} catch e {
    print("Async error:", e)
}
```

---

## Validation of Dangling Pointer Fixes and Memory Safety

**Description:**  
Ensure async operations do not cause memory corruption or dangling references.

```zephyr
class Safe {
    async func run(x) { return x * 2 }
}
s = Safe()
promise = spawn s.run(10)
print(await promise)
```
