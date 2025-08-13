# Zephyr Debugging & Edge Cases

This document covers debugging techniques, edge case scenarios, and troubleshooting patterns in Zephyr. Learn how to handle errors, debug member access, test chaining, and address known issues.

---

## Stack Trace and Error Debugging

Use `try`/`catch` to handle and print errors.

```zephyr
try {
    risky = 10 / 0
} catch e {
    print("Error:", e)
}
```

---

## Member Access Debugging Patterns

Test member access and corruption scenarios.

```zephyr
class Test {
    func init() { this.value = 42 }
    func corrupt() { this.value = none }
}
t = Test()
t.corrupt()
print(t.value) # Output: none
```

---

## Chaining Debug Patterns

Debug complex chaining and nested access.

```zephyr
obj = {"a": [{"b": {"c": 1}}]}
print(obj["a"][0]["b"]["c"]) # Output: 1
```

---

## Token Debug and Parsing Edge Cases

Test edge cases in parsing and tokenization.

```zephyr
x = -(-(-5))    # Triple negation
y = +(+3)       # Double plus
z = !false      # Logical not
```

---

## Member Corruption and Memory Issues

Test for member corruption and memory safety.

```zephyr
class Corruptor {
    func init() { this.data = [1,2,3] }
    func corrupt() { this.data = none }
}
c = Corruptor()
c.corrupt()
print(c.data) # Output: none
```

---

## Problematic Patterns and Known Issues

Test patterns that may cause issues.

```zephyr
try {
    x = none
    x += 1
} catch e {
    print("Error:", e)
}
```

---

## Method Call Debugging

Test method calls and edge cases.

```zephyr
class Debug {
    func test() { print("Testing") }
}
d = Debug()
d.test()
```

---

## Edge Case Scenarios

Test edge cases for operators and control flow.

```zephyr
x = 0
if !x { print("Zero is falsy") }
```

---

## Simple Unary Operator Tests

Test unary operators for correctness.

```zephyr
a = -(-5)
b = +(+3)
c = !false
```

---

## Clean Version Tests

Test clean, minimal code for correctness.

```zephyr
x = 1
y = 2
z = x + y
print(z)
```

---

## Summary

Debugging and edge case handling in Zephyr is straightforward and robust. Use try/catch, test chaining, and validate edge cases to ensure reliable code.
