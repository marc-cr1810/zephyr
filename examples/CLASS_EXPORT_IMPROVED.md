# Improved Class Export Functionality

This document describes the enhanced `export_class` functionality that allows C++ plugins to export classes with full member variable access and method calls.

## Overview

The improved class export system provides:

- **Real Class Instances**: Create actual C++ object instances accessible from Zephyr
- **Member Variable Access**: Direct read/write access to public member variables
- **Method Calls**: Call C++ methods on instances with automatic type conversion
- **Multiple Constructors**: Support for different constructor overloads
- **Automatic Memory Management**: C++ objects are automatically managed through shared_ptr

## Key Improvements Over the Original System

### Before (Original System)
```cpp
// Only constructor functions that returned string representations
export_function("Vector2D", [](const std::vector<value_t>& args) -> value_result_t {
    // ... argument parsing ...
    Vector2D vec(x, y);
    return value_result_t::success(std::make_shared<string_object_t>(vec.to_string()));
});

// Usage in Zephyr:
vec = plugin.Vector2D(3, 4)  // Returns "Vector2D(3, 4)" string
// vec.x  // Not possible - it's just a string
// vec.magnitude()  // Not possible
```

### After (Improved System)
```cpp
// Real class export with member access
auto vector2d_exporter = make_class_exporter<Vector2D>("Vector2D")
    .constructor([](double x, double y) { return std::make_shared<Vector2D>(x, y); })
    .member("x", &Vector2D::x)
    .member("y", &Vector2D::y)
    .method("magnitude", &Vector2D::magnitude);

export_class("Vector2D", vector2d_exporter.get_native_class());

// Usage in Zephyr:
vec = plugin.Vector2D(3.0, 4.0)  // Returns actual C++ object wrapper
print(vec.x)  // Direct member access: 3.0
vec.x = 5.0   // Direct member modification
print(vec.magnitude())  // Method call: 5.0
```

## How to Use the Improved System

### 1. Include the Required Headers

```cpp
#include "zephyr/api/plugin_interface.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/api/class_exporter.hpp"
```

### 2. Define Your C++ Class

```cpp
class Vector2D {
public:
    double x, y;  // Public members will be accessible from Zephyr
    
    Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    double magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    void normalize() {
        double mag = magnitude();
        if (mag > 0.0) {
            x /= mag;
            y /= mag;
        }
    }
    
    std::string to_string() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};
```

### 3. Export the Class in Your Plugin Module

```cpp
class my_plugin_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        // Create class exporter
        auto vector2d_exporter = make_class_exporter<Vector2D>("Vector2D")
            // Register constructors
            .constructor([](double x, double y) {
                return std::make_shared<Vector2D>(x, y);
            })
            .constructor([]() {
                return std::make_shared<Vector2D>();
            })
            // Register member variables
            .member("x", &Vector2D::x)
            .member("y", &Vector2D::y)
            // Register methods
            .method("magnitude", &Vector2D::magnitude)
            .method("normalize", &Vector2D::normalize)
            .method("to_string", &Vector2D::to_string);
        
        // Export the class
        export_class("Vector2D", vector2d_exporter.get_native_class());
        
        return void_result_t::success();
    }
};
```

### 4. Use the Class in Zephyr Code

```python
# Import the plugin
import "my_plugin.so"

# Create instances using different constructors
vec1 = my_plugin.Vector2D(3.0, 4.0)  # Constructor with arguments
vec2 = my_plugin.Vector2D()          # Default constructor

# Access and modify member variables
print("vec1.x =", vec1.x)  # Output: vec1.x = 3.0
print("vec1.y =", vec1.y)  # Output: vec1.y = 4.0

vec1.x = 5.0
vec1.y = 12.0
print("Modified vec1:", vec1.to_string())  # Output: Vector2D(5.0, 12.0)

# Call methods
print("Magnitude:", vec1.magnitude())  # Output: Magnitude: 13.0

vec1.normalize()
print("After normalization:", vec1.to_string())  # Output: Vector2D(0.384615, 0.923077)
```

## Supported Type Conversions

The system automatically handles conversion between Zephyr and C++ types:

| C++ Type | Zephyr Type | Bidirectional |
|----------|-------------|---------------|
| `int` | `int_object_t` | ✅ |
| `double`, `float` | `float_object_t` | ✅ |
| `std::string` | `string_object_t` | ✅ |
| `bool` | `boolean_object_t` | ✅ |

## API Reference

### `class_exporter_t<T>`

The main class for exporting C++ classes.

#### Constructors

- `constructor(std::function<std::shared_ptr<T>()>)` - Default constructor (0 args)
- `constructor(std::function<std::shared_ptr<T>(double, double)>)` - 2 double args
- `constructor(std::function<std::shared_ptr<T>(const std::string&, int)>)` - string + int args
- `constructor(std::function<std::shared_ptr<T>(const std::string&)>)` - single string arg

#### Member Registration

- `member(const std::string& name, MemberType T::*member_ptr)` - Export a public member variable

#### Method Registration

- `method(const std::string& name, ReturnType (T::*method_ptr)())` - No-argument method
- `method(const std::string& name, ReturnType (T::*method_ptr)() const)` - Const no-argument method  
- `method(const std::string& name, ReturnType (T::*method_ptr)(ArgType))` - Single-argument method

#### Export

- `get_native_class()` - Get the native class for use with `export_class()`

### Helper Functions

- `make_class_exporter<T>(const std::string& class_name)` - Create a new class exporter

## Examples

See the following example files:

- `class_export_plugin_improved.cpp` - Complete plugin implementation
- `test_improved_class_export.zephyr` - Basic functionality test
- `improved_class_export_usage.zephyr` - Comprehensive usage examples

## Limitations

1. **Constructor Overloading**: Currently limited to specific type combinations. Can be extended by adding more constructor template specializations.

2. **Method Overloading**: Not fully supported yet. Each method name can only be registered once.

3. **Complex Types**: Only basic types (int, double, string, bool) have automatic conversion. Complex types need custom conversion logic.

4. **Static Members**: Static member variables and methods are not currently supported.

5. **Inheritance**: C++ inheritance relationships are not exposed to Zephyr.

## Future Enhancements

- Generic constructor template parameter deduction
- Method overloading support
- Static member support
- Custom type conversion registry
- Inheritance support
- Property accessors (getters/setters)

## Migration from Old System

To migrate from the old function-based class export:

1. Replace `export_function("ClassName", ...)` with `export_class("ClassName", ...)`
2. Use `class_exporter_t` instead of manual lambda functions
3. Register members and methods explicitly
4. Update Zephyr code to use direct member access instead of utility functions

The old system will continue to work alongside the new system for backward compatibility.