# Zephyr Programming Language

A modern, dynamically-typed scripting language implemented in C++17 with comprehensive features for both functional and object-oriented programming.

## Overview

Zephyr is a powerful scripting language that combines the simplicity of dynamic typing with advanced features like asynchronous programming, lambda expressions, and optional explicit typing. Originally known as GeminiLang, the language has been renamed to Zephyr to better reflect its modern, streamlined design philosophy.

## Key Features

- **Dynamic Typing**: Variables can hold any type of value
- **Optional Explicit Typing**: Enhanced type safety with optional type annotations
- **Object-Oriented Programming**: Classes, inheritance patterns, and method chaining
- **Functional Programming**: Lambda expressions, closures, and higher-order functions
- **Asynchronous Programming**: Full async/await support with parallel execution
- **Modern Syntax**: Clean, readable syntax with compound assignment operators
- **Built-in Data Structures**: Lists, dictionaries, and complex nesting support
- **Error Handling**: Try-catch blocks and comprehensive error reporting
- **String Operations**: Advanced string manipulation and formatting
- **Control Flow**: If-else, loops, switch statements with fall-through

## Language Rename

This project was previously known as **GeminiLang** and has been comprehensively renamed to **Zephyr**. All references, file extensions, documentation, and code have been updated:

- **Language Name**: GeminiLang → Zephyr
- **File Extension**: `.gemini` → `.zephyr`
- **Executable**: `gemini_lang` → `zephyr`
- **Namespace**: `gemini::` → `zephyr::`
- **Documentation**: All references updated throughout

## Quick Start

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd zephyr_lang

# Build the project
make

# Run the REPL
./bin/zephyr

# Run a script file
./bin/zephyr examples/01_basic_features.zephyr
```

### System Requirements

- C++17 compatible compiler (GCC 7+ or Clang 5+)
- CMake 3.10 or higher
- Make or compatible build system

## Language Examples

### Basic Syntax

```zephyr
# Variables and basic operations
name = "Zephyr"
version = 1.0
active = true

print("Welcome to", name, "version", version)

# Functions
func greet(name) {
    return "Hello, " + name + "!"
}

message = greet("World")
print(message)
```

### Optional Explicit Typing

```zephyr
# Type annotations for enhanced safety
name : string = "Alice"
age : int = 25
scores : list = [95, 87, 92]

# Typed function parameters and return types
func calculate_average(numbers : list) : float {
    total = 0
    for num in numbers {
        total += num
    }
    return total / len(numbers)
}

average : float = calculate_average(scores)
print("Average score:", average)
```

### Object-Oriented Programming

```zephyr
class Person {
    func init(name, age) {
        this.name = name
        this.age = age
    }
    
    func greet() {
        return "Hi, I'm " + this.name
    }
    
    func have_birthday() {
        this.age += 1
        print(this.name, "is now", this.age, "years old")
    }
}

person = Person("Alice", 30)
print(person.greet())
person.have_birthday()
```

### Asynchronous Programming

```zephyr
# Async functions
async func fetch_data(url) {
    print("Fetching from:", url)
    return "Data from " + url
}

async func process_data() {
    # Sequential execution
    result1 = await fetch_data("api.example.com/users")
    result2 = await fetch_data("api.example.com/posts")
    
    # Parallel execution
    parallel_results = await all([
        spawn fetch_data("api.example.com/comments"),
        spawn fetch_data("api.example.com/likes")
    ])
    
    return [result1, result2] + parallel_results
}
```

### Lambda Functions

```zephyr
# Simple lambda expressions
square = x -> x * x
add = (a, b) -> a + b

# Lambda with explicit types
multiply : lambda = (x : int, y : int) -> x * y

# Higher-order functions
numbers = [1, 2, 3, 4, 5]
squared = map(numbers, square)
print("Squared:", squared)
```

## Project Structure

```
zephyr_lang/
├── src/                    # C++ source files
├── include/                # Header files
├── examples/               # Language examples (21 comprehensive files)
├── grammar/                # ANTLR grammar definition
├── build/                  # Build output directory
├── CMakeLists.txt          # CMake configuration
├── Makefile                # Build automation
├── ZEPHYR.md              # Comprehensive language documentation
├── TYPING_ROADMAP.md      # Future typing system features
├── ideas.md               # Future enhancement ideas
└── test_all_examples.sh   # Test suite for all examples
```

## Documentation

- **[ZEPHYR.md](ZEPHYR.md)**: Complete language reference and feature documentation
- **[TYPING_ROADMAP.md](TYPING_ROADMAP.md)**: Planned enhancements to the type system
- **[ideas.md](ideas.md)**: Future language enhancement proposals
- **[examples/README.md](examples/README.md)**: Comprehensive guide to all example files

## Example Files

The `examples/` directory contains 21 comprehensive example files demonstrating all language features:

1. **01_basic_features.zephyr** - Core language fundamentals
2. **02_data_structures.zephyr** - Lists, dictionaries, and data manipulation
3. **03_functions_and_lambdas.zephyr** - Functions and functional programming
4. **04_classes_and_objects.zephyr** - Object-oriented programming
5. **05_advanced_indexing.zephyr** - Complex indexing patterns
6. **06_advanced_classes.zephyr** - Advanced OOP patterns
7. **07_debugging_and_edge_cases.zephyr** - Debugging techniques
8. **08_comprehensive_examples.zephyr** - Real-world applications
9. **09_string_operations.zephyr** - String manipulation and formatting
10. **10_compound_assignment.zephyr** - Modern assignment operators
11. **11_compound_assignment_comprehensive.zephyr** - Advanced assignment patterns
12. **12_lambda_dictionaries_nested.zephyr** - Complex functional programming
13. **13_async_await_basic.zephyr** - Basic async programming
14. **14_async_parallel.zephyr** - Parallel execution patterns
15. **15_async_promise_all.zephyr** - Promise.all patterns
16. **16_async_comprehensive_test.zephyr** - Advanced async scenarios
17. **17_enhanced_loops.zephyr** - Enhanced loop constructs
18. **18_optional_chaining.zephyr** - Safe navigation operators
19. **19_async_class_methods.zephyr** - Async OOP patterns
20. **20_explicit_typing.zephyr** - Type system demonstrations
21. **21_exit_function.zephyr** - Error handling and program termination

## Testing

Run the comprehensive test suite to verify all language features:

```bash
# Run all example tests
bash test_all_examples.sh

# Run individual examples
./bin/zephyr examples/01_basic_features.zephyr
./bin/zephyr examples/13_async_await_basic.zephyr
./bin/zephyr examples/20_explicit_typing.zephyr
```

## Development Tools

- **Grammar Definition**: `grammar/Zephyr.g4` - ANTLR4 grammar specification
- **Parser Generator**: `generate_parser.py` - Helps maintain C++ parser sync with grammar
- **Build System**: CMake with Make wrapper for easy compilation
- **Test Suite**: Automated testing of all language features

## Code Organization

The C++ implementation follows modern naming conventions:

- **Header Files**: Use `.hpp` extension (e.g., `ast.hpp`, `parser.hpp`)
- **Source Files**: Use `.cpp` extension with snake_case naming (e.g., `ast.cpp`, `async_scheduler.cpp`)
- **Directory Structure**: Organized into logical modules (`objects/`, `types/`)
- **Namespace**: All code is in the `zephyr::` namespace
- **File Naming**: Consistent snake_case throughout (e.g., `class_object.hpp`, `builtin_function_type.cpp`)

## Contributing

This project demonstrates a complete programming language implementation with:

- Lexical analysis and parsing (lexer, parser modules)
- Abstract Syntax Tree (AST) generation with visitor pattern
- Interpretation with proper scoping and environment management
- Memory management with smart pointers and RAII
- Comprehensive error handling with location tracking
- Modern C++ design patterns and snake_case naming conventions
- Modular architecture with separate object and type systems

## Language Philosophy

Zephyr aims to provide:

- **Simplicity**: Easy to learn and use syntax
- **Power**: Advanced features when needed
- **Safety**: Optional typing for large projects
- **Performance**: Efficient C++ implementation
- **Flexibility**: Support for multiple programming paradigms

## License

[Add your license information here]

## Acknowledgments

Zephyr represents a comprehensive study in programming language design and implementation, showcasing modern language features in a clean, accessible package.