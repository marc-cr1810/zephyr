# Zephyr Classes & Objects

This document provides detailed, well-worded descriptions and annotated examples for Zephyr's object-oriented programming features, including class definition, instantiation, member access, method chaining, composition, inheritance-like patterns, and lambdas in classes.

---

## Class Definition

Classes in Zephyr allow you to define custom types with encapsulated data (member variables) and behavior (methods). The `init` method acts as the constructor, initializing member variables when an object is created.

```zephyr
class Person {
    func init(name, age) {
        this.name = name
        this.age = age
    }
    func greet() {
        return "Hello, " + this.name
    }
}
```

Member variables are attached to `this`, representing the current instance.

---

## Instantiation & Member Access

Create instances of classes and interact with their members and methods using dot notation.

```zephyr
p = Person("Alice", 25)
print(p.greet())        # Output: Hello, Alice
p.age += 1              # Modify member variable
print("New age:", p.age)
```

You can freely access and modify member variables unless they are declared as `const`.

---

## Method Chaining

Method chaining allows you to call multiple methods in sequence, improving code readability and fluency. Methods can return `this` or other objects to enable chaining.

```zephyr
class Counter {
    func init() { this.value = 0 }
    func inc() { this.value++; return this }
    func dec() { this.value--; return this }
    func get() { return this.value }
}
c = Counter()
c.inc().inc().dec()
print(c.get())          # Output: 1
```

---

## Object Composition

Classes can contain instances of other classes as members, enabling complex relationships and hierarchies.

```zephyr
class Engine {
    func init(power) { this.power = power }
}
class Car {
    func init(model, power) {
        this.model = model
        this.engine = Engine(power)
    }
    func describe() {
        print("Car:", this.model, "with engine power", this.engine.power)
    }
}
car = Car("Sedan", 150)
car.describe() # Output: Car: Sedan with engine power 150
```

---

## Inheritance-like Patterns

While Zephyr does not have classical inheritance, you can achieve similar behavior through composition and delegation.

```zephyr
class Animal {
    func speak() { print("Animal sound") }
}
class Dog {
    func init() { this.animal = Animal() }
    func speak() {
        print("Woof!")
        this.animal.speak()  # Delegation
    }
}
d = Dog()
d.speak() # Output: Woof! Animal sound
```

---

## Classes with Lambdas

Classes can store lambda functions as member variables, integrating functional programming with OOP. This is useful for strategies, callbacks, or dynamic behaviors.

```zephyr
class MathBox {
    func init() {
        this.ops = {
            "square": x -> x * x,
            "cube": x -> x * x * x
        }
    }
    func apply(op, value) {
        return this.ops[op](value)
    }
}
box = MathBox()
print(box.apply("cube", 3))    # Output: 27
```

---

## Advanced Member Access and Chaining

Zephyr supports complex member/index chaining for deep data structures and method calls.

```zephyr
class DataManager {
    func init() {
        this.records = [
            {"id": 1, "data": {"values": [100, 200, 300]}},
            {"id": 2, "data": {"values": [400, 500, 600]}}
        ]
    }
    func get_value(record_idx, value_idx) {
        return this.records[record_idx]["data"]["values"][value_idx]
    }
}
dm = DataManager()
print(dm.get_value(1, 2)) # Output: 600
```

---

## Factory Pattern Example

Classes can implement the factory pattern to create other objects.

```zephyr
class PersonFactory {
    func create(name, age) {
        return Person(name, age)
    }
}
factory = PersonFactory()
bob = factory.create("Bob", 40)
print(bob.greet())
```

---

## Empty Classes and Classes Without Constructors

You can define classes without an `init` method or with no members, useful for marker types or simple grouping.

```zephyr
class Empty {}
e = Empty()
print(type(e)) # Output: Empty
```

---

## Summary

Zephyr's object-oriented features enable you to model real-world entities, relationships, and behaviors with clarity and flexibility. Classes, objects, and methods integrate seamlessly with other language features, supporting both simple and advanced programming patterns. Use composition, delegation, and lambdas for powerful, maintainable designs.
