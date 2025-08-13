# Zephyr Compound Assignment Comprehensive

This document provides in-depth explanations and practical examples for all advanced compound assignment scenarios in Zephyr. Compound assignment operators (`+=`, `-=`, `*=`, `/=`, `%=`), increment/decrement (`++`, `--`), and their integration with classes, lambdas, collections, and nested structures are covered. You'll also find best practices, error handling, and performance notes.

---

## Overview

Compound assignment operators in Zephyr allow you to update variables, class members, and collection elements in place, making code concise and expressive. These operators work with all supported data types (numbers, strings, lists, dictionaries, class members) and integrate seamlessly with other language features.

---

## Complete Test Suite

```zephyr
x = 10
x += 5
x -= 2
x *= 3
x /= 2
x %= 4

s = "abc"
s += "def"
s *= 2

lst = [1,2,3]
lst[0] += 10
lst[1] *= 2

d = {"a": 1, "b": 2}
d["a"] += 5
d["b"] *= 3
```

---

## Complex Nested Structure Operations

**Description:**  
Compound assignment works with deeply nested lists and dictionaries, enabling direct updates to values several layers deep.

```zephyr
data = {"users": [{"score": 100}, {"score": 85}]}
data["users"][0]["score"] += 50      # Add bonus to first user
data["users"][1]["score"] *= 1.2     # Apply multiplier to second user
```

---

## Class Method Integration

**Description:**  
Class methods can use compound assignment to update member variables, including increment/decrement operators for counters and statistics.

```zephyr
class Counter {
    func init() { this.value = 0 }
    func inc() { this.value++ }         # Postfix increment
    func add(n) { this.value += n }     # Compound addition
}
c = Counter()
c.inc()
c.add(5)
```

---

## Lambda Function Integration

**Description:**  
Compound assignment can be combined with lambda functions for functional programming patterns and dynamic calculations.

```zephyr
adder = x -> x + 10
val = 5
val += adder(2)    # val = val + adder(2)
```

---

## Mathematical Applications

**Description:**  
Use compound assignment in mathematical algorithms, such as vector operations, accumulators, and iterative calculations.

```zephyr
vec = [1,2,3]
for i in vec { vec[i-1] *= 2 }   # Double each element
```

---

## String Processing and Pattern Generation

**Description:**  
Compound assignment with strings enables efficient pattern generation and formatting.

```zephyr
pattern = "-"
for i = 1; i <= 5; i++ {
    pattern *= 2
    print(pattern)
}
```

---

## Task Management System Simulation

**Description:**  
Simulate task completion and status updates using compound assignment in class-based systems.

```zephyr
class Task {
    func init(name) { this.name = name; this.done = false }
    func complete() { this.done = true }
}
tasks = [Task("A"), Task("B")]
tasks[0].complete()
```

---

## Performance and Scalability Testing

**Description:**  
Test compound assignment performance with large data sets and batch updates.

```zephyr
big = []
for i = 1; i <= 10000; i++ { append(big, i) }
for i in big { big[i-1] += 1 }
```

---

## Comprehensive Error Handling

**Description:**  
Compound assignment operations automatically check for errors such as division/modulo by zero and type mismatches.

```zephyr
try {
    x = 10
    x /= 0
} catch e {
    print("Error:", e)
}
```

---

## Best Practices

**Description:**  
- Use compound assignment for concise, readable code.
- Prefer `obj.member += 1` for member increment outside methods.
- Always handle possible errors in division/modulo.
- Use compound assignment in loops for efficient accumulation and transformation.

---
