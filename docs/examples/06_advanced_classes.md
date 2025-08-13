# Zephyr Advanced Classes

This document details advanced object-oriented patterns and design patterns in Zephyr, including constructors, factory/builder/observer/composite/strategy patterns, inheritance-like behavior, mathematical classes, lambda integration, and method chaining.

---

## Advanced Class Constructors and Initialization

Classes can have complex constructors to initialize multiple members and relationships.

```zephyr
class User {
    func init(name, age, role) {
        this.name = name
        this.age = age
        this.role = role
        this.active = true
    }
}
u = User("Alice", 30, "admin")
```

---

## Factory Pattern for Object Creation

The factory pattern centralizes object creation logic.

```zephyr
class Factory {
    func create(name) {
        return {"name": name}
    }
}
f = Factory()
obj = f.create("Widget")
```

---

## Builder Pattern for Complex Objects

The builder pattern enables step-by-step construction of objects.

```zephyr
class Builder {
    func init() { this.data = {} }
    func set(key, value) { this.data[key] = value; return this }
    func build() { return this.data }
}
b = Builder()
result = b.set("x", 1).set("y", 2).build()
```

---

## Observer Pattern for Event Handling

The observer pattern allows objects to react to events.

```zephyr
class Subject {
    func init() { this.listeners = [] }
    func subscribe(listener) { append(this.listeners, listener) }
    func notify(msg) {
        for l in this.listeners { l(msg) }
    }
}
subject = Subject()
subject.subscribe(msg -> print("Received:", msg))
subject.notify("Hello!")
```

---

## Composite Pattern for Hierarchical Structures

The composite pattern models tree-like data.

```zephyr
class Node {
    func init(name) { this.name = name; this.children = [] }
    func add(child) { append(this.children, child) }
}
root = Node("root")
child = Node("child")
root.add(child)
```

---

## Strategy Pattern for Algorithm Selection

The strategy pattern allows dynamic selection of algorithms.

```zephyr
class Context {
    func set_strategy(strategy) { this.strategy = strategy }
    func execute(x) { return this.strategy(x) }
}
ctx = Context()
ctx.set_strategy(x -> x * 2)
print(ctx.execute(5)) # Output: 10
```

---

## Advanced Class Inheritance-like Behavior

Use composition and delegation to mimic inheritance.

```zephyr
class Base {
    func greet() { print("Hello from Base") }
}
class Derived {
    func init() { this.base = Base() }
    func greet() {
        print("Hello from Derived")
        this.base.greet()
    }
}
d = Derived()
d.greet()
```

---

## Mathematical Classes and Operations

Classes can encapsulate mathematical logic.

```zephyr
class Vector {
    func init(x, y) { this.x = x; this.y = y }
    func add(other) { return Vector(this.x + other.x, this.y + other.y) }
}
v1 = Vector(1, 2)
v2 = Vector(3, 4)
v3 = v1.add(v2)
print(v3.x, v3.y) # Output: 4 6
```

---

## Classes with Lambda Integration

Store lambdas in classes for dynamic behavior.

```zephyr
class MathOps {
    func init() {
        this.ops = {
            "square": x -> x * x,
            "cube": x -> x * x * x
        }
    }
    func apply(op, value) { return this.ops[op](value) }
}
m = MathOps()
print(m.apply("cube", 2)) # Output: 8
```

---

## Complex Object Interactions

Classes can interact and compose for advanced systems.

```zephyr
class Engine { func init(power) { this.power = power } }
class Car {
    func init(model, power) {
        this.model = model
        this.engine = Engine(power)
    }
    func describe() { print(this.model, this.engine.power) }
}
car = Car("Sedan", 150)
car.describe()
```

---

## Method Chaining Patterns

Return `this` from methods to enable chaining.

```zephyr
class Counter {
    func init() { this.value = 0 }
    func inc() { this.value++; return this }
    func dec() { this.value--; return this }
}
c = Counter()
c.inc().inc().dec()
print(c.value) # Output: 1
```

---

## Summary

Advanced classes in Zephyr enable robust, maintainable, and flexible object-oriented designs. Use design patterns, composition, and lambda integration for scalable, expressive code.
