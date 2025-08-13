# Zephyr Async Parallel Execution

This document explains parallel execution in Zephyr using the `spawn` keyword, including comparisons with sequential execution, complex workflows, error handling, async lambdas in parallel, performance patterns, and real-world examples.

---

## Overview

Parallel execution lets you run multiple tasks concurrently, improving performance and responsiveness. Zephyr's `spawn` keyword creates promises for async functions and methods, which can be awaited individually or in batches.

---

## Spawn Keyword for Concurrent Task Execution

**Description:**  
Use `spawn` to start async functions or methods in parallel.

```zephyr
async func task(n) {
    print("Task", n, "started")
    return n * n
}

promise1 = spawn task(2)
promise2 = spawn task(3)
result1 = await promise1
result2 = await promise2
print(result1, result2)  # Output: 4 9
```

---

## Parallel vs Sequential Execution

**Description:**  
Compare the performance and behavior of sequential and parallel async operations.

```zephyr
async func slow(n) {
    print("Slow", n)
    return n * 10
}

# Sequential
a = await slow(1)
b = await slow(2)
print(a, b)

# Parallel
pa = spawn slow(1)
pb = spawn slow(2)
print(await pa, await pb)
```

---

## Complex Parallel Workflows

**Description:**  
Manage multiple concurrent tasks with lists of promises and batch processing.

```zephyr
async func fetch(id) { return "Data " + str(id) }
promises = []
for i = 1; i <= 5; i++ {
    append(promises, spawn fetch(i))
}
results = []
for p in promises {
    append(results, await p)
}
print(results)
```

---

## Error Handling in Parallel Execution

**Description:**  
Handle errors in parallel tasks using try/catch and promise resolution.

```zephyr
async func risky(n) {
    if n == 2 { return 1 / 0 }
    return n
}
promises = [spawn risky(1), spawn risky(2)]
try {
    for p in promises { print(await p) }
} catch e {
    print("Parallel error:", e)
}
```

---

## Async Lambda Functions in Parallel Scenarios

**Description:**  
Use async lambdas for parallel data processing and transformation.

```zephyr
async_square = async x -> x * x
promises = [spawn async_square(4), spawn async_square(5)]
print(await promises[0], await promises[1])
```

---

## Performance Patterns and Optimization

**Description:**  
Optimize parallel execution for large data sets and intensive computations.

```zephyr
async func heavy(n) { return n * 100 }
promises = []
for i = 1; i <= 100; i++ { append(promises, spawn heavy(i)) }
results = await all(promises)
print("Sum:", reduce(results, (a, b) -> a + b, 0))
```

---

## Real-World Parallel Processing Example

**Description:**  
Apply parallel execution to practical scenarios like downloading files or processing batches.

```zephyr
class Downloader {
    async func download(url) { return "Downloaded " + url }
}
urls = ["a.com", "b.com", "c.com"]
downloader = Downloader()
promises = []
for url in urls { append(promises, spawn downloader.download(url)) }
files = await all(promises)
print(files)
```
