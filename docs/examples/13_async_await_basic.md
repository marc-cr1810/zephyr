# Zephyr Async/Await Basics

This document describes Zephyr's asynchronous programming model, including async function definitions, await expressions, sequential and parallel async operations, async lambdas, error handling, and integration with classes and other features.

---

## Overview

Zephyr's async/await system allows you to write non-blocking code that is easy to read and maintain. Async functions return promises, which can be awaited for their results. You can spawn multiple async operations in parallel and handle errors gracefully.

---

## Async Function Definitions

**Description:**  
Define async functions using the `async` keyword. These functions return promises and can be awaited.

```zephyr
async func fetch_data(url) {
    print("Fetching:", url)
    return "Data from " + url
}
```

---

## Await Expressions

**Description:**  
Pause execution until an async function or promise resolves.

```zephyr
result = await fetch_data("http://example.com")
print(result)  # Output: Data from http://example.com
```

---

## Sequential Async Operations

**Description:**  
Chain multiple async operations in sequence using `await`.

```zephyr
async func step1() { return "Step 1" }
async func step2(prev) { return prev + " -> Step 2" }
async func step3(prev) { return prev + " -> Step 3" }

async func workflow() {
    a = await step1()
    b = await step2(a)
    c = await step3(b)
    print(c)
}

await workflow()
```

---

## Async Lambdas

**Description:**  
Create anonymous async functions for concise, inline asynchronous logic.

```zephyr
async_processor = async x -> x * 2
print(await async_processor(5))  # Output: 10

async_block = async x -> {
    print("Processing", x)
    return x + 100
}
print(await async_block(7))  # Output: 107
```

---

## Error Handling in Async Contexts

**Description:**  
Use `try`/`catch` to handle errors in async functions and lambdas.

```zephyr
async func risky() {
    return 10 / 0
}
try {
    await risky()
} catch e {
    print("Async error:", e)
}
```

---

## Promise Object Types and States

**Description:**  
Async functions return promise objects, which can be inspected and awaited.

```zephyr
async func delayed(val) {
    return val
}
promise = spawn delayed(42)
print(type(promise))  # Output: promise
print(await promise)  # Output: 42
```

---

## Integration with Existing Language Features

**Description:**  
Async functions and lambdas work seamlessly with classes, loops, and data structures.

```zephyr
class Loader {
    async func load(name) {
        print("Loading", name)
        return name + " loaded"
    }
}
loader = Loader()
print(await loader.load("file.txt"))
```

---

## Simple Async Workflows

**Description:**  
Combine async functions and await for readable, maintainable asynchronous code.

```zephyr
async func double(x) { return x * 2 }
async func add(x, y) { return x + y }

async func run() {
    a = await double(10)
    b = await add(a, 5)
    print(b)
}
await run()
```
