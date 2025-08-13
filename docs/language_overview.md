# Zephyr Language Overview

Zephyr is a modern, expressive scripting language designed for clarity, flexibility, and robust programming. This overview provides a detailed look at Zephyr's features, syntax, and design philosophy, with practical examples and explanations.

---

## Key Features

- **Dynamic Typing:** Assign any value to variables without declaring a type; great for rapid prototyping.
- **Optional Explicit Typing:** Add type annotations for safety and clarity; helps catch errors early.
- **Variables & Constants:** Use mutable variables and immutable `const` for reliable code.
- **Arithmetic Operators:** Perform calculations with numbers and strings; supports negative values.
- **String Multiplication:** Easily repeat strings for formatting and patterns.
- **Lists & Dictionaries:** Flexible collections for storing and manipulating data.
- **Functions & Lambdas:** Define reusable code blocks and anonymous functions; supports async and const parameters.
- **Object-Oriented Programming:** Create classes and objects for structured, modular code.
- **Control Flow:** Direct program execution with conditionals, switches, and loops.
- **Error Handling:** Catch and handle runtime errors gracefully.
- **Asynchronous Programming:** Write non-blocking code with async/await and parallel execution.
- **Advanced Assignment:** Update values in-place with compound operators and increment/decrement.
- **Optional Chaining:** Safely access nested properties and indexes, avoiding errors on `none`.
- **Functional Programming:** Use map, filter, reduce, and higher-order functions for concise data processing.
- **Pipe Operator:** Chain function calls for readable, linear data flow.
- **Membership & Nullish Coalescing:** Test for membership and provide defaults only for `none`.
- **Destructuring Assignment:** Unpack lists into variables for concise code.
- **Unicode & Emoji Support:** Use international characters and emojis in strings and identifiers.

---

## Data Types

- **Numbers:** Integers and floats for calculations.
- **Strings:** Unicode text for messages and formatting.
- **Booleans:** `true` and `false` for logic and control flow.
- **Lists:** Ordered, mutable collections for sequences.
- **Dictionaries:** Key-value mappings for structured data.
- **Classes & Objects:** User-defined types for encapsulation.
- **Lambdas & Functions:** First-class functions for abstraction.
- **Promises:** Represent pending results in async code.

---

## Operators

- **Arithmetic:** Basic math and string repetition.
- **Comparison:** Test equality and order.
- **Logical:** Combine and invert boolean values.
- **Membership:** Check if a value exists in a collection.
- **Nullish Coalescing:** Provide defaults only for `none`.
- **Optional Chaining:** Safely access nested data.
- **Pipe Operator:** Chain operations for clarity.
- **Ternary:** Concise conditional expressions.
- **Compound Assignment:** Update values efficiently.
- **Increment/Decrement:** Quick value changes.
- **Destructuring:** Unpack collections into variables.

---

## Control Flow

- **Conditional Statements:** Branch logic with `if`/`else`.
- **Switch Statements:** Multi-way branching with fall-through.
- **Loops:** Iterate with `for`, `while`, and infinite `loop`.
- **Break/Continue:** Control loop execution.
- **Error Handling:** Use `try`/`catch` for safe code.

---

## Functions and Lambdas

- **Function Definitions:** Organize code into reusable blocks.
- **Lambdas:** Write concise, anonymous functions.
- **Higher-Order Functions:** Pass functions as arguments.
- **Async Functions:** Run code without blocking.

---

## Object-Oriented Programming

- **Classes:** Model real-world entities and behaviors.
- **Member Access:** Use dot notation for properties and methods.
- **Composition:** Build complex objects from simpler ones.
- **Method Chaining:** Call multiple methods in sequence.
- **Inheritance-like Patterns:** Share behavior via composition.

---

## Asynchronous Programming

- **Async Functions:** Write non-blocking code.
- **Async Lambdas:** Concise async functions.
- **Await:** Pause until a result is ready.
- **Spawn:** Run tasks in parallel.
- **All:** Wait for multiple tasks.

---

## Advanced Data Access

- **Chained Member/Index Access:** Work with deeply nested data.
- **Direct Dictionary Member Access:** Use dot notation for string keys.
- **Destructuring Assignment:** Unpack lists easily.
- **Optional Chaining:** Avoid errors on missing data.

---

## String Processing

- **Concatenation:** Join strings together.
- **Multiplication:** Repeat strings for formatting.
- **Formatting:** Create headers, tables, and patterns.
- **Unicode & Emoji Support:** Use any character or symbol.

---

## Functional Programming

- **Map/Filter/Reduce:** Transform and aggregate data.
- **Function Composition:** Build pipelines of operations.
- **Currying:** Create specialized functions.
- **Lambdas in Data Structures:** Store functions for dynamic behavior.

---

## Error Handling

- **Try/Catch:** Handle errors gracefully.
- **Safe Operations:** Prevent crashes with optional chaining.
- **Exit Function:** Terminate on critical errors.

---

## Unicode and Emoji Support

Zephyr supports Unicode and emojis in strings, identifiers, and output, enabling internationalization and expressive formatting.

---

## Practical Examples

### Variables and Typing

```zephyr
x = 10                # Dynamic integer
y : float = 2.5       # Explicit float
name : string = "Alice"
flag : bool = true
const pi : float = 3.14159
```

### Lists and Dictionaries

```zephyr
numbers = [1, 2, 3]
info = {"name": "Zephyr", "version": 1.0}
users = [
    {"name": "Alice", "scores": [95, 87]},
    {"name": "Bob", "scores": [88, 91]}
]
```

### Arithmetic, Comparison, and Logical Operators

```zephyr
sum = x + y
is_greater = x > y
neg = -x
flag = true and false
result = not flag
value = x > 5 or y < 10
```

### String Multiplication and Concatenation

```zephyr
border = "=" * 40
repeat = 3 * "Hi"
greeting = "Hello"
greeting += " World"
greeting *= 2
```

### Functions and Lambdas

```zephyr
func add(a, b) { return a + b }
square = x -> x * x
power = (base, exp) -> {
    result = 1
    for i = 1; i <= exp; i++ { result *= base }
    return result
}
```

### Classes and Objects

```zephyr
class Person {
    func init(name, age) { this.name = name; this.age = age }
    func greet() { return "Hello, " + this.name }
}
p = Person("Alice", 25)
print(p.greet())
```

### Control Flow

```zephyr
if x > 5 { print("x is large") } else { print("x is small") }
switch x {
    case 10: print("Ten"); break
    case 20: print("Twenty")
    default: print("Other")
}
for i = 0; i < 3; i++ { print(i) }
while x < 20 { x += 1 }
loop { print("Infinite loop"); break }
```

### Error Handling

```zephyr
try { risky = 10 / 0 } catch e { print("Error:", e) }
```

### Asynchronous Programming

```zephyr
async func fetch_data(url) { return "Data from " + url }
result = await fetch_data("http://example.com")
promise = spawn fetch_data("http://example.com")
results = await all([promise])
```

### Compound Assignment and Increment/Decrement

```zephyr
x += 5; x -= 2; x *= 3; x /= 2; x %= 4
count++; ++count; count--; --count
scores[0] += 10
info["version"] += 1.0
```

### Optional Chaining and Nullish Coalescing

```zephyr
profile = none
profile?.score += 10
name = user?.profile?.name ?? "Guest"
```

### Functional Programming

```zephyr
numbers = [1,2,3]
doubled = map(numbers, x -> x * 2)
evens = filter(numbers, x -> x % 2 == 0)
total = reduce(numbers, (a, b) -> a + b, 0)
compose = (f, g) -> x -> f(g(x))
```

### Pipe Operator

```zephyr
result = value |> func1 |> func2
```

### Destructuring Assignment

```zephyr
[a, b] = [1, 2]
[x, y, z] = [3, 4, 5]
```

### Unicode and Emoji Support

```zephyr
emoji_line = "🚀" * 5
print(emoji_line)  # "🚀🚀🚀🚀🚀"
```

---

## Design Philosophy

Zephyr is built for:

- **Readability:** Concise, clear syntax for maintainable code.
- **Expressiveness:** Modern features for functional, object-oriented, and asynchronous programming.
- **Safety:** Optional explicit typing, robust error handling, and const protection.
- **Flexibility:** Dynamic typing, advanced data structures, and seamless feature integration.
- **Productivity:** Rapid prototyping, easy learning curve, and comprehensive built-in functions.

---

## Advanced Features

- **Async/await and parallel execution:** Write non-blocking code with simple syntax.
- **Comprehensive error handling:** Catch and recover from runtime errors and type mismatches.
- **Advanced data access:** Unlimited depth of member/index chaining, direct dictionary member access, and destructuring.
- **String processing:** Powerful formatting, multiplication, and Unicode support.
- **Integration:** All features work together—lambdas in classes, async in OOP, compound assignment in nested structures.

---

## Getting Started

- See [docs/getting_started.md](get_started.md) for installation and first steps.
- Explore [docs/examples/](examples/) for detailed, categorized feature demonstrations.
- Review [ZEPHYR.md](../ZEPHYR.md) for the full language specification.

---

## Summary

Zephyr is a feature-rich, modern scripting language for learning, prototyping, and building robust applications. Its syntax and features are designed to be intuitive, powerful, and enjoyable for both beginners and experienced developers.

---
