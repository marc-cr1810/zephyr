# Zephyr Data Structures

This document provides detailed, well-worded descriptions and annotated examples for Zephyr's powerful data structures, including lists, dictionaries, nesting, advanced indexing, destructuring, and for-each loops.

---

## Lists

Lists are ordered, mutable collections that can hold any type of value. They are ideal for storing sequences, stacks, queues, or any group of items.

```zephyr
numbers = [1, 2, 3]           # Create a list of integers
numbers[0] = 10               # Modify the first element
append(numbers, 4)            # Add an item to the end
removed = pop(numbers, 1)     # Remove and return the item at index 1
```

Lists can be nested, indexed, and manipulated with built-in functions. They grow and shrink dynamically, supporting flexible data modeling.

---

## Dictionaries

Dictionaries are collections of key-value pairs, perfect for representing structured data, mappings, or configurations.

```zephyr
info = {"name": "Zephyr", "version": 1.0}
info["version"] = 2.0
info["author"] = "Marc"
keys_list = keys(info)        # Get all keys
values_list = values(info)    # Get all values
```

Keys are typically strings or numbers, and values can be any type. Dictionaries support fast lookup and modification.

---

## Nested Structures

Lists and dictionaries can be nested to model complex, real-world data.

```zephyr
users = [
    {"name": "Alice", "scores": [95, 87]},
    {"name": "Bob", "scores": [88, 91]}
]
print(users[0]["scores"][1]) # Access nested list in dictionary
users[1]["scores"][0] += 10  # Modify nested value
```

Nesting enables hierarchical data, such as user profiles, configuration trees, or multi-dimensional arrays.

---

## Advanced Indexing

Zephyr supports advanced indexing, allowing you to chain access through multiple levels of lists and dictionaries.

```zephyr
obj = {"data": [{"val": 1}, {"val": 2}]}
print(obj["data"][1]["val"])     # Access nested value
obj["data"][0]["val"] *= 5       # Modify nested value
```

You can combine member access and indexing in any order for powerful data manipulation.

---

## Mixed Member/Index Chaining

Classes can contain lists or dictionaries as members, and you can chain member and index access for deep data structures.

```zephyr
class Container {
    func init() {
        this.items = [{"value": 10}, {"value": 20}]
    }
}
c = Container()
c.items[1]["value"] += 5         # Access and modify nested member
```

This pattern is useful for managing collections of objects or complex data.

---

## Destructuring Assignment

Destructuring allows you to unpack values from lists into distinct variables, making code more readable and concise.

```zephyr
[a, b] = [1, 2]
[x, y, z] = [3, 4, 5]
```

Destructuring is especially useful when working with tuples or fixed-size lists.

---

## For-Each Loops with Collections

Zephyr's for-each loops make it easy to iterate over lists and dictionaries, supporting both simple and advanced patterns.

```zephyr
for item in numbers {
    print(item)
}

for key in info {
    print(key, info[key])
}
```

You can combine for-each loops with destructuring, filtering, and advanced indexing for powerful data processing.

---

## Summary

Zephyr's data structures are flexible, dynamic, and deeply integrated with the language. Lists and dictionaries can be nested, indexed, and manipulated with ease, supporting everything from simple arrays to complex, real-world data models.
