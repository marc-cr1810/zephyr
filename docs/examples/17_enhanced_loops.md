# Zephyr Enhanced Loops

This document describes Zephyr's advanced loop constructs, including infinite loops, enhanced for-each loops with filtering, enumerate, zip, complex patterns, and integration with other features.

---

## Overview

Enhanced loops in Zephyr provide powerful iteration capabilities, including filtering, parallel iteration, and index-value pairs. These features make data processing and transformation concise and expressive.

---

## Loop Construct for Infinite Loops

**Description:**  
Use `loop` for infinite or event-driven loops, with `break` and `continue` for control.

```zephyr
loop {
    print("Running...")
    break
}
```

---

## Enhanced For-Each Loops with Where Clauses

**Description:**  
Filter items during iteration using the `where` clause.

```zephyr
nums = [1, 2, 3, 4, 5, 6]
for n in nums where n % 2 == 0 {
    print("Even:", n)
}
```

---

## Enumerate Function for Index-Value Pairs

**Description:**  
Iterate over lists with both index and value using `enumerate`.

```zephyr
fruits = ["apple", "banana", "cherry"]
for i, fruit in enumerate(fruits) {
    print(i, fruit)
}
```

---

## Zip Function for Parallel Iteration

**Description:**  
Iterate over multiple lists in parallel using `zip`.

```zephyr
names = ["Alice", "Bob"]
scores = [95, 88]
for name, score in zip(names, scores) {
    print(name, score)
}
```

---

## Complex Filtering and Iteration Patterns

**Description:**  
Combine filtering, destructuring, and advanced indexing for powerful data processing.

```zephyr
data = [{"x": 1}, {"x": 2}, {"x": 3}]
for item in data where item["x"] > 1 {
    print(item)
}
```

---

## Integration with Other Language Features

**Description:**  
Loops work seamlessly with classes, lambdas, and data structures.

```zephyr
class Counter {
    func init() { this.value = 0 }
    func inc() { this.value++ }
}
c = Counter()
for i = 1; i <= 5; i++ { c.inc() }
print(c.value)
```

---
