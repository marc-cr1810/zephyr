# Zephyr Interfaces

This document provides a comprehensive overview of Zephyr's interface feature, detailing how to define, implement, and use interfaces for contract-based programming. Interfaces promote polymorphism, enhance code organization, and improve type safety by ensuring classes adhere to specific behavioral contracts.

---

## Overview

Interfaces in Zephyr define a contract that classes can implement. They specify a set of method signatures that a class must provide, ensuring adherence to a common behavior. This promotes polymorphism and allows for writing more generic and flexible code. Unlike classes, interfaces cannot be instantiated and do not contain method implementations or member variables.

---

## Basic Interface Definition

Interfaces are declared using the `interface` keyword, followed by the interface name and a block containing method signatures. Interface methods do not have bodies; they only define the method name, its parameters, and optionally its return type.

```zephyr
interface Drawable {
    func draw()
    func get_area() : float
}

interface Loggable {
    func log(message : string)
}
```

-   **`interface` keyword**: Declares a new interface.
-   **Method Signatures**: Define the methods that implementing classes must provide. These include the method name, parameter list (with optional types), and an optional return type.

---

## Class Implementation of Interfaces

A class implements one or more interfaces by listing them after the class name, separated by commas. The class must provide concrete implementations for all methods defined in the interfaces it implements. If a class fails to implement all required methods or implements them with incorrect signatures (e.g., wrong number of parameters, incompatible return type), a compile-time error will occur.

```zephyr
class Rectangle : Drawable {
    width : float
    height : float

    func init(w : float, h : float) {
        this.width = w
        this.height = h
    }

    func draw() {
        return "Rectangle(" + str(this.width) + "x" + str(this.height) + ")"
    }

    func get_area() : float {
        return this.width * this.height
    }
}

class Circle : Drawable {
    radius : float

    func init(r : float) {
        this.radius = r
    }

    func draw() {
        return "Circle(r=" + str(this.radius) + ")"
    }

    func get_area() : float {
        return 3.14159 * this.radius * this.radius
    }
}
```

-   **`:` (colon)**: Used to indicate that a class implements an interface.
-   **Multiple Interfaces**: A class can implement multiple interfaces by listing them comma-separated (e.g., `class MyClass : InterfaceA, InterfaceB { ... }`).

---

## Using Interfaces as Types

Interfaces can be used as type annotations for variables and function parameters. This enables writing polymorphic code that operates on any object implementing a specific interface, without needing to know its concrete class type. This is a powerful feature for creating flexible and extensible designs.

```zephyr
func render_shape(shape : Drawable) {
    print("Rendering shape:", shape.draw())
    print("Shape area:", shape.get_area())
}

rect = Rectangle(10.0, 5.0)
render_shape(rect) # Works with Rectangle instance

circl = Circle(7.0)
render_shape(circl) # Works with Circle instance

# Variables can also be typed with an interface
drawable_shape : Drawable = Rectangle(3.0, 4.0)
render_shape(drawable_shape)

drawable_shape = Circle(5.0) # Can reassign to another implementing class
render_shape(drawable_shape)
```

-   **Polymorphism**: The `render_shape` function can accept any object that implements the `Drawable` interface, regardless of its actual class.
-   **Type Safety**: The compiler ensures that only objects implementing `Drawable` can be passed to `render_shape` or assigned to `drawable_shape`.

---

## Multiple Interface Implementation

A single class can implement multiple interfaces, inheriting the contractual obligations from all of them. This allows a class to fulfill various roles or provide diverse functionalities.

```zephyr
class AdvancedRectangle : Drawable, Loggable {
    width : float
    height : float

    func init(w : float, h : float) {
        this.width = w
        this.height = h
    }

    func draw() {
        return "AdvancedRectangle(" + str(this.width) + "x" + str(this.height) + ")"
    }

    func get_area() : float {
        return this.width * this.height
    }

    func log(message : string) {
        print("[LOG] AdvancedRectangle:", message)
    }
}

adv_rect = AdvancedRectangle(15.0, 8.0)
adv_rect.log("Creating advanced rectangle.")
render_shape(adv_rect) # Can be passed to render_shape because it implements Drawable
adv_rect.log("Advanced rectangle rendered.")
```

---

## Edge Cases and Error Handling

Zephyr's compiler provides robust checks for interface implementation. Errors will be raised if:
-   A class attempts to implement a non-existent interface.
-   A class implements an interface but fails to provide all required methods.
-   A class implements an interface method with an incorrect signature (e.g., wrong number of parameters, incompatible types).

These checks ensure that the contracts defined by interfaces are strictly enforced, leading to more reliable and maintainable code.

```zephyr
# Example of compile-time error for incomplete implementation (conceptual)
# try {
#     class IncompleteRectangle : Drawable {
#         func draw() { return "Incomplete" }
#         # Missing get_area() - will cause a compile-time error
#     }
# } catch e {
#     print("Caught expected error:", e)
# }
```

---

## Summary

Zephyr's interfaces provide a powerful mechanism for defining contracts and achieving polymorphism. They enable developers to write more modular, flexible, and type-safe code by enforcing behavioral agreements between different parts of an application. By using interfaces, you can design systems that are easier to extend, maintain, and test.
