# Zephyr Advanced Indexing

This document explores Zephyr's advanced indexing capabilities, including chained member/index access, matrix/grid operations, dictionary/list combinations, and debugging patterns. These features allow you to manipulate deeply nested and complex data structures with concise, readable syntax.

---

## Basic Indexed Assignment Patterns

Indexed assignment lets you directly modify elements within lists, dictionaries, or nested structures.

```zephyr
matrix = [[1,2,3],[4,5,6],[7,8,9]]
matrix[0][1] = 99         # Sets second element of first row to 99
```

---

## Object Member Indexed Assignment

You can combine member access and indexing for powerful data manipulation.

```zephyr
class Data {
    func init() {
        this.values = [10, 20, 30]
    }
}
d = Data()
d.values[2] += 5           # Increments third value by 5
```

---

## Chained Member and Index Access

Chaining allows you to traverse multiple levels of lists, dictionaries, and object members.

```zephyr
obj = {"list": [{"val": 1}, {"val": 2}]}
obj["list"][1]["val"] += 10    # Adds 10 to second object's "val"
```

---

## Matrix and Grid Operations

Multi-dimensional arrays and grids are easy to manipulate with chained indexing.

```zephyr
grid = [
    [{"x": 1}, {"x": 2}],
    [{"x": 3}, {"x": 4}]
]
grid[1][0]["x"] *= 2           # Doubles value at row 2, column 1
```

---

## Dictionary of Lists and Lists of Dictionaries

Mixing dictionaries and lists enables flexible data modeling.

```zephyr
dict_of_lists = {"row1": [10, 20], "row2": [30, 40]}
dict_of_lists["row2"][1] += 5

list_of_dicts = [{"name": "first"}, {"name": "second"}]
list_of_dicts[1]["name"] = "updated"
```

---

## Assignment Chain Debugging Patterns

Debugging complex assignments is easier with Zephyr's readable syntax.

```zephyr
complex = {"a": [{"b": {"c": [1,2,3]}}]}
complex["a"][0]["b"]["c"][2] = 99
print(complex["a"][0]["b"]["c"]) # Output: [1, 2, 99]
```

---

## Cross-Object Chained Indexing

You can assign values across multiple objects and structures in a single statement.

```zephyr
class Holder {
    func init() { this.data = [{"score": 0}, {"score": 0}] }
}
h1 = Holder()
h2 = Holder()
h1.data[0]["score"] = 50
h2.data[1]["score"] = 75
```

---

## Extreme Chained Indexing Patterns

Zephyr supports unlimited depth of mixed chaining for advanced use cases.

```zephyr
deep = {"x": [{"y": {"z": [1,2,3]}}]}
deep["x"][0]["y"]["z"][1] += 100
```

---

## Verification of All Indexing Patterns

Test and verify all combinations of member/index chaining to ensure correctness.

```zephyr
obj = {"a": [{"b": {"c": [1,2,3]}}]}
obj["a"][0]["b"]["c"][2] = 42
print(obj["a"][0]["b"]["c"][2]) # Output: 42
```

---

## Summary

Advanced indexing in Zephyr empowers you to work with complex, deeply nested data structures using concise, readable syntax. Chaining member and index access, combined with assignment, enables powerful data manipulation and modeling.
