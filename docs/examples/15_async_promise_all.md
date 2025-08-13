# Zephyr Promise.all Patterns

This document details the use of Promise.all in Zephyr for batch async operations, including basic usage, complex data fetching, mixed sources, nested Promise.all, error handling, performance comparisons, and batch processing.

---

## Overview

Promise.all enables you to wait for multiple async operations to complete in parallel, collecting all results in a single list. This is essential for batch processing, data aggregation, and efficient workflows.

---

## Basic Promise.all Usage

**Description:**  
Wait for a list of promises to resolve and collect their results.

```zephyr
async func get(n) { return n * 2 }
promises = [spawn get(1), spawn get(2), spawn get(3)]
results = await all(promises)
print(results)  # Output: [2, 4, 6]
```

---

## Complex Data Fetching with Parallel Requests

**Description:**  
Fetch data from multiple sources in parallel and aggregate results.

```zephyr
async func fetch(url) { return "Data from " + url }
urls = ["a.com", "b.com", "c.com"]
promises = []
for url in urls { append(promises, spawn fetch(url)) }
data = await all(promises)
print(data)
```

---

## Mixed Data Sources and Heterogeneous Operations

**Description:**  
Combine different types of async operations in a single batch.

```zephyr
async func get_num(n) { return n }
async func get_str(s) { return s }
promises = [spawn get_num(10), spawn get_str("abc")]
results = await all(promises)
print(results)  # Output: [10, "abc"]
```

---

## Nested Promise.all Operations

**Description:**  
Compose multiple Promise.all calls for hierarchical or staged batch processing.

```zephyr
async func double(n) { return n * 2 }
outer = []
for i = 1; i <= 3; i++ {
    inner = [spawn double(i), spawn double(i+10)]
    append(outer, await all(inner))
}
print(outer)  # Output: [[2, 22], [4, 24], [6, 26]]
```

---

## Error Handling with Promise.all

**Description:**  
Catch and handle errors that occur in any of the parallel operations.

```zephyr
async func risky(n) {
    if n == 2 { return 1 / 0 }
    return n
}
promises = [spawn risky(1), spawn risky(2)]
try {
    results = await all(promises)
    print(results)
} catch e {
    print("Promise.all error:", e)
}
```

---

## Performance: Sequential vs Parallel

**Description:**  
Compare the efficiency of sequential and parallel batch processing.

```zephyr
async func slow(n) { return n * 10 }
# Sequential
results_seq = []
for i = 1; i <= 5; i++ { append(results_seq, await slow(i)) }
print(results_seq)

# Parallel
promises = []
for i = 1; i <= 5; i++ { append(promises, spawn slow(i)) }
results_par = await all(promises)
print(results_par)
```

---

## Batch Processing and Data Aggregation

**Description:**  
Use Promise.all for real-world batch tasks like processing lists or aggregating data.

```zephyr
async func process(item) { return item + "_done" }
items = ["a", "b", "c"]
promises = []
for item in items { append(promises, spawn process(item)) }
results = await all(promises)
print(results)  # Output: ["a_done", "b_done", "c_done"]
```
