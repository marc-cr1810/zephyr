# Zephyr String Operations

This document covers advanced string manipulation and formatting in Zephyr, including concatenation, multiplication, edge cases, formatting functions, integration with data structures, and Unicode support.

---

## String Concatenation

Combine strings using `+` or `+=` for readable, efficient text assembly.

```zephyr
greeting = "Hello"
greeting += " World"
print(greeting)  # "Hello World"
```

---

## String Multiplication / Repetition

Repeat strings using the `*` operator with integers, supporting both directions.

```zephyr
border = "=" * 40
pattern = "*" * 10
repeat = 3 * "Hi"
empty = "abc" * 0      # ""
negative = "abc" * -2  # ""
```

---

## String Multiplication Edge Cases

Handles zero, negative, and single repetitions gracefully.

```zephyr
zero = "test" * 0      # ""
negative = "test" * -1 # ""
single = "word" * 1    # "word"
```

---

## Practical String Formatting Functions

Create reusable formatting utilities for headers, tables, and patterns.

```zephyr
func header(title) {
    border = "=" * (len(title) + 4)
    return border + "\n  " + title + "\n" + border
}
print(header("Welcome"))
```

---

## String Formatting with Data Structures

Format and display structured data with string operations.

```zephyr
users = [
    {"name": "Alice", "score": 95},
    {"name": "Bob", "score": 88}
]
for user in users {
    print(user["name"] + ": " + str(user["score"]))
}
```

---

## Advanced String Operations with Chaining

Integrate string multiplication and concatenation with member/index access.

```zephyr
class Formatter {
    func init() {
        this.style = "*"
        this.length = 10
    }
    func make_line() {
        return this.style * this.length
    }
}
f = Formatter()
print(f.make_line())
```

---

## Document Generation with String Formatting

Generate tables and documents programmatically.

```zephyr
func make_table(rows) {
    for row in rows {
        print("| " + row["name"] + " | " + str(row["score"]) + " |")
    }
}
make_table(users)
```

---

## Integration with All Language Features

String operations work seamlessly with loops, classes, and data structures.

```zephyr
for i = 1; i <= 5; i++ {
    print("Row " + str(i) + ": " + "-" * (i * 5))
}
```

---

## Unicode and Emoji Support

Full support for Unicode characters and emojis in string operations.

```zephyr
emoji_line = "🚀" * 5
print(emoji_line)  # "🚀🚀🚀🚀🚀"
```

---

## Summary

Zephyr's string operations are powerful, flexible, and deeply integrated with the language, enabling advanced formatting, document generation, and internationalization.
