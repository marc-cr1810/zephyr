# Zephyr Comprehensive Examples

This document presents detailed, real-world scenarios and user applications that demonstrate how Zephyr's features integrate in practical contexts. Each example is annotated to explain the design and language features used.

---

## To-Do List Manager

A simple class-based application for managing tasks, demonstrating lists, classes, methods, control flow, and member access.

```zephyr
class TodoList {
    func init() {
        this.tasks = []
    }
    func add_task(task : string) {
        append(this.tasks, {"task": task, "done": false})
    }
    func complete_task(index : int) {
        if index >= 0 and index < len(this.tasks) {
            this.tasks[index]["done"] = true
        }
    }
    func list_tasks() {
        for i, t in enumerate(this.tasks) {
            status = t["done"] ? "[x]" : "[ ]"
            print(status, t["task"])
        }
    }
}

todo = TodoList()
todo.add_task("Write Zephyr docs")
todo.add_task("Implement parser")
todo.complete_task(0)
todo.list_tasks()
```

---

## Simple Banking System

Demonstrates classes, explicit typing, arithmetic, error handling, and member access.

```zephyr
class Account {
    func init(owner : string, balance : float) {
        this.owner = owner
        this.balance = balance
    }
    func deposit(amount : float) {
        this.balance += amount
    }
    func withdraw(amount : float) {
        if amount <= this.balance {
            this.balance -= amount
        } else {
            print("Insufficient funds")
        }
    }
    func statement() {
        print(this.owner, "Balance:", this.balance)
    }
}

acc = Account("Alice", 1000.0)
acc.deposit(250.0)
acc.withdraw(100.0)
acc.statement()
```

---

## Library Management System

Shows nested data structures, classes, loops, and control flow.

```zephyr
class Book {
    func init(title : string, author : string) {
        this.title = title
        this.author = author
        this.checked_out = false
    }
}

class Library {
    func init() {
        this.books = []
    }
    func add_book(title, author) {
        append(this.books, Book(title, author))
    }
    func checkout(title) {
        for book in this.books {
            if book.title == title and not book.checked_out {
                book.checked_out = true
                print("Checked out:", title)
                return
            }
        }
        print("Book not available")
    }
    func list_books() {
        for book in this.books {
            status = book.checked_out ? "Out" : "In"
            print(book.title, "-", book.author, "[", status, "]")
        }
    }
}

lib = Library()
lib.add_book("1984", "Orwell")
lib.add_book("Dune", "Herbert")
lib.checkout("1984")
lib.list_books()
```

---

## Data Processing Pipeline

Demonstrates functional programming with map, filter, reduce, and lambdas.

```zephyr
func filter_even(nums) {
    return filter(nums, x -> x % 2 == 0)
}
func double(nums) {
    return map(nums, x -> x * 2)
}
func sum(nums) {
    return reduce(nums, (a, b) -> a + b, 0)
}

data = [1,2,3,4,5,6]
evens = filter_even(data)
doubled = double(evens)
total = sum(doubled)
print("Total:", total)
```

---

## User Question Example

Demonstrates optional chaining and nullish coalescing.

```zephyr
user = none
name = user?.profile?.name ?? "Guest"
print("Hello,", name)
```

---

## Basic Test Example

Shows simple assertion and error handling.

```zephyr
func add(a, b) { return a + b }
assert = (cond, msg) -> {
    if not cond { print("Test failed:", msg) }
}
assert(add(2, 3) == 5, "Addition works")
assert(add(-1, 1) == 0, "Negative addition works")
```

---

## Final Verification Test Suite

Demonstrates lambdas, higher-order functions, and test automation.

```zephyr
tests = [
    () -> assert(1 + 1 == 2, "Simple addition"),
    () -> assert("a" * 3 == "aaa", "String multiplication"),
    () -> assert(len([1,2,3]) == 3, "List length"),
    () -> assert({"x": 1}["x"] == 1, "Dict access"),
    () -> assert((x -> x * x)(4) == 16, "Lambda square"),
    () -> assert((a, b -> a + b)(2, 3) == 5, "Lambda add"),
    () -> assert((() -> 42)() == 42, "Lambda no params")
]
for test in tests { test() }
```

---

## Real-World Application Patterns

Integrates classes, lambdas, async, and error handling.

```zephyr
class Service {
    async func fetch_data(id) {
        if id < 0 { exit(10) }
        return "Data for " + str(id)
    }
}
service = Service()
try {
    print(await service.fetch_data(5))
    print(await service.fetch_data(-1)) # Will exit
} catch e {
    print("Error:", e)
}
```

---

## Summary

These comprehensive examples show how Zephyr's features combine to solve real problems, from data processing and management to error handling and async workflows. Use these patterns as a foundation for your own applications.
