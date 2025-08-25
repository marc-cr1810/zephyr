# Zephyr Examples

This directory contains comprehensive examples and demonstrations of all Zephyr programming language features, organized into 12 main files that cover everything from basic syntax to advanced patterns.

## Main Example Files

These files provide complete, organized demonstrations of Zephyr features with comprehensive examples:

### 01_basic_features.zephyr
**Core language fundamentals and syntax**
- Variables and data types (numbers, strings, booleans)
- Arithmetic and comparison operations (including negative numbers)
- String multiplication/repetition operations ("text" * count)
- Logical operations and boolean conversion (including edge cases)
- Ternary operator examples
- Control flow (if/else, while loops, for loops)
- Break and continue statements
- Comments (including trailing comments and documentation)
- Built-in functions (str, int, float, len, append, pop, keys, values, type)
- Switch statements (including fall-through behavior)
- Error handling (try/catch for various error types)
- Unary operators (+, -, !, multiple unary combinations)
- Negative number arithmetic and expressions

### 02_data_structures.zephyr
**Lists, dictionaries, and complex data manipulation**
- Lists (creation, indexing, modification, operations)
- Nested lists and matrices
- Dictionaries (creation, access, modification, operations)
- Complex nested structures (lists of dicts, dicts of lists)
- Advanced indexing patterns and chained access
- Mixed member/index chaining (obj.member[index].property)
- Indexed assignment with objects (member.dict[key] = value)
- Chained indexed assignment (complex nested access patterns)
- For-each loops with collections
- Destructuring assignment
- Cross-object indexed assignment

### 03_functions_and_lambdas.zephyr
**Functions, lambda expressions, and functional programming**
- Basic function definitions with parameters and return values
- String formatting functions with multiplication
- Functions with negative number handling
- Recursive functions (factorial, fibonacci)
- Lambda expressions (single/multiple/no parameters)
- Lambda expressions with block bodies
- Simple and advanced lambda examples
- Closures and variable capture
- Higher-order functions
- Functional programming with collections (map, filter, reduce)
- Custom implementations of functional operations
- Lambda utilities for data access
- Lambda predicates and type operations
- Function composition and currying
- Lambda functions in data structures
- Lambda functions stored in dictionaries and lists
- Nested lambda function calls and recursion
- Lambda function factories and higher-order functions

### 04_classes_and_objects.zephyr
**Object-oriented programming fundamentals**
- Basic class definition and instantiation
- Constructor methods and member variables
- Method calls and member access
- Advanced member access and chaining patterns
- Object composition and relationships
- Member variables and nested object access
- Complex nested object structures
- Data container classes
- Classes with lambda integration
- Factory pattern examples
- Method chaining patterns
- Mixed member/index access: obj.list[i].method()
- Classes without constructors and empty classes

### 05_advanced_indexing.zephyr
**Complex indexing patterns and member access**
- Basic indexed assignment patterns
- Object member indexed assignment
- Chained member and index access
- Complex assignment patterns
- Matrix and grid operations
- Dictionary of lists and lists of dictionaries
- Assignment chain debugging patterns
- Cross-object chained indexing
- Extreme chained indexing patterns (triple indexing, mixed chains)
- Verification of all indexing patterns

### 06_advanced_classes.zephyr
**Advanced object-oriented patterns and design patterns**
- Advanced class constructors and initialization
- Factory pattern for object creation
- Builder pattern for complex objects
- Observer pattern for event handling
- Composite pattern for hierarchical structures
- Strategy pattern for algorithm selection
- Advanced class inheritance-like behavior
- Mathematical classes and operations
- Classes with lambda integration
- Complex object interactions
- Method chaining patterns

### 07_debugging_and_edge_cases.zephyr
**Debugging techniques and edge case scenarios**
- Stack trace and error debugging
- Member access debugging patterns
- Chaining debug patterns
- Token debug and parsing edge cases
- Unary operator edge cases
- Member corruption and memory issues
- Problematic patterns and known issues
- Method call debugging
- Edge case scenarios
- Simple unary operator tests
- Clean version tests

### 10_compound_assignment.zephyr
**Compound assignment operators and increment/decrement**
- Variable compound assignment (+=, -=, *=, /=)
- Increment and decrement operators (++, --, prefix and postfix)
- String concatenation and repetition with compound operators
- Class member compound assignment
- Collection element compound assignment (lists and dictionaries)
- Nested structure compound assignment
- Integration with loops and expressions
- Real-world examples (game character progression, shopping cart)
- Error handling for compound operations
- Performance demonstrations and best practices

### 11_compound_assignment_comprehensive.zephyr
**Advanced compound assignment scenarios and edge cases**
- Complete test suite for all compound assignment features
- Complex nested structure operations
- Class method integration with this.member++ syntax
- Lambda function integration with compound operators
- Mathematical applications (vectors, data processing)
- String processing and pattern generation
- Task management system simulation
- Performance and scalability testing
- Comprehensive error handling examples
- Best practices and usage guidelines

### 12_lambda_dictionaries_nested.zephyr
**Lambda functions in complex data structures and nested calling patterns**
- Lambda functions stored in lists of dictionaries
- Nested lambda function calls using intermediate variables
- Recursive lambda functions (factorial, fibonacci)
- Lambda function factories that create other lambda functions
- Function composition and currying with lambdas
- Pipeline processing with staged lambda transformations
- Error handling patterns in lambda functions
- Complex nested data structures with lambda chains
- Dynamic lambda creation based on runtime parameters
- Higher-order functions that accept and return lambdas
- Safe operation patterns with structured error returns
- Advanced functional programming patterns and best practices

### 13_async_await_basic.zephyr
**Basic async/await functionality and patterns**
- Async function definitions with return promises
- Await expressions for pausing execution
- Sequential async operations and proper ordering
- Async lambdas with expression and block bodies
- Error handling in async contexts with try/catch
- Promise object types and states
- Integration with existing language features
- Simple async workflows and patterns

### 14_async_parallel.zephyr
**Parallel execution using spawn for concurrent operations**
- Spawn keyword for concurrent task execution
- Parallel vs sequential execution comparisons
- Complex parallel workflows with multiple data sources
- Error handling in parallel execution contexts
- Async lambda functions in parallel scenarios
- Performance patterns and optimization techniques
- Real-world parallel processing examples

### 15_async_promise_all.zephyr
**Promise.all patterns using the all() built-in function**
- Basic Promise.all usage with multiple async operations
- Complex data fetching with parallel requests
- Mixed data sources and heterogeneous operations
- Nested Promise.all operations and composition
- Error handling with Promise.all scenarios
- Performance comparisons between sequential and parallel approaches
- Real-world examples of batch processing and data aggregation

### 16_async_comprehensive_test.zephyr
**Comprehensive async/await testing and validation**
- REPL-like usage patterns and edge cases
- Lambda variable reassignment and scoping
- Complex async lambda bodies and nested calls
- Stress testing with multiple async operations
- Data structure integration with async lambdas
- Error handling across various async scenarios
- Validation of dangling pointer fixes and memory safety

### 17_enhanced_loops.zephyr
**Enhanced loop constructs and iteration patterns**
- Loop construct for infinite loops with break/continue
- Enhanced for-each loops with where clauses for filtering
- Enumerate function for index-value iteration pairs
- Zip function for parallel iteration over multiple collections
- Complex filtering and iteration patterns
- Integration with other language features

### 18_optional_chaining.zephyr
**Optional chaining and safe navigation operators**
- Optional property access with ?. operator
- Optional index access with ?[] operator
- Safe navigation through potentially none values
- Direct dictionary member access with dot notation
- Chaining optional operators for complex navigation
- Error prevention in nested data access

### 19_async_class_methods.zephyr
**Async class methods with spawn and const parameters**
- Async method definitions in classes with const parameters
- Spawn functionality with both sync and async method calls
- Parallel execution of object-oriented async operations
- Promise.all patterns with spawned method calls
- Mixed function and method call async programming
- Complete const parameter protection in async class context
- Advanced async object-oriented programming patterns

### 20_explicit_typing.zephyr
**Optional explicit typing system for enhanced type safety**
- Basic variable type declarations with colon syntax (x : string = "hello")
- Typed constant declarations (const pi : float = 3.14159)
- Function parameters with explicit types (func add(a : int, b : int))
- Lambda parameters with explicit typing ((a : int, b : int) -> a + b)
- Mixed typed and untyped parameters in functions and lambdas
- User-defined class types as explicit types (person : Person = Person())
- None assignment with type constraint retention
- Type inheritance across function and control structure scopes
- Async functions and lambdas with typed parameters
- Comprehensive type validation and error handling
- Real-world usage patterns with explicit typing

### 21_exit_function.zephyr
**Comprehensive exit() function usage and error handling patterns**
- Introduction to exit() function and proper usage patterns
- Configuration validation with meaningful exit codes
- System resource validation and critical error handling
- Business logic validation and compliance checking
- Class system integrity validation with exit codes
- Async system validation and error recovery
- Graceful shutdown patterns and cleanup procedures
- Exit code documentation and standardization (ranges 10-59)
- Best practices for exit() vs try/catch usage
- Real-world scenarios requiring immediate program termination
- Production-level error handling and monitoring integration

### 22_interfaces.zephyr
**Defining and implementing interfaces for contract-based programming**
- Basic interface definition with method signatures
- Class implementation of one or more interfaces
- Using interfaces as type annotations for variables and function parameters
- Multiple interface implementation by a single class
- Error handling for incomplete or incorrect interface implementations

### 23_built_in_functions.zephyr
**Comprehensive demonstration of all built-in functions**
- Detailed examples for print, len, input, str, int, float, append, pop, keys, values, type, map, filter, reduce, enumerate, zip, and all.
- Practical use cases and common scenarios for each function.
- Demonstrates type conversions, list/dictionary manipulations, and asynchronous operations.

### 24_explicit_typing_and_init_restrictions.zephyr
**Demonstrates explicit return types and init method restrictions**
- Explicit return types for functions, class methods, and lambdas.
- Restrictions on `init` methods, which cannot have a return type or return a value.

### 25_bitwise_operations.zephyr
**Bitwise operations and their precedence**
- Bitwise AND (`&`), OR (`|`), XOR (`^`), NOT (`~`)
- Left Shift (`<<`), Right Shift (`>>`)
- Operator precedence with other arithmetic and logical operators
- Error handling for invalid bitwise operations

### 09_string_operations.zephyr
**Advanced string manipulation and formatting**
- String concatenation and basic operations
- String multiplication/repetition ("char" * count)
- String multiplication edge cases (zero, negative, empty strings)
- Practical string formatting functions
- String formatting with data structures
- Advanced string operations with chaining
- Document generation with string formatting
- Integration of string operations with all language features
- Unicode and emoji support in string multiplication

### 08_comprehensive_examples.zephyr
**Real-world examples and user scenarios**
- To-Do List Manager (complete application)
- Simple Banking System (accounts, transfers, statements)
- Library Management System (books, members, operations)
- Data Processing Pipeline (filters, transformers, functional programming)
- User question examples and common patterns
- Basic test examples
- User-provided example tests
- Final verification test suite
- Real-world application patterns

## Learning Path

Follow this recommended sequence to master Zephyr:

1. **Start with `01_basic_features.zephyr`** - Master fundamental language features including variables, operators, control flow, built-ins, error handling, and string multiplication

2. **Progress to `02_data_structures.zephyr`** - Learn about lists, dictionaries, complex nesting, and advanced indexing patterns

3. **Explore `03_functions_and_lambdas.zephyr`** - Master functions, lambda expressions, closures, and functional programming concepts

4. **Study `04_classes_and_objects.zephyr`** - Understand object-oriented programming, member variables, and complex object relationships

5. **Advance to `05_advanced_indexing.zephyr`** - Master complex indexing patterns and chained member access

6. **Learn `06_advanced_classes.zephyr`** - Explore design patterns and advanced object-oriented techniques

7. **Practice with `07_debugging_and_edge_cases.zephyr`** - Learn debugging techniques and handle edge cases

8. **Study `09_string_operations.zephyr`** - Master advanced string manipulation, formatting, and integration patterns

9. **Explore `10_compound_assignment.zephyr`** - Learn modern compound assignment operators and increment/decrement syntax

10. **Master `11_compound_assignment_comprehensive.zephyr`** - Advanced compound assignment scenarios and integration patterns

11. **Explore `12_lambda_dictionaries_nested.zephyr`** - Master lambda functions in complex data structures and advanced functional programming

12. **Learn `13_async_await_basic.zephyr`** - Master asynchronous programming with async functions and await expressions

13. **Study `14_async_parallel.zephyr`** - Understand parallel execution patterns using spawn for concurrent operations

14. **Master `15_async_promise_all.zephyr`** - Learn Promise.all patterns for waiting on multiple async operations

16. **Test with `16_async_comprehensive_test.zephyr`** - Validate understanding with comprehensive async scenarios

17. **Study `21_exit_function.zephyr`** - Learn proper error handling with exit codes and graceful shutdown patterns

18. **Explore `22_interfaces.zephyr`** - Understand how to define and implement interfaces for contract-based programming

19. **Master `23_built_in_functions.zephyr`** - Learn the usage of all built-in functions with practical examples

21. **Study `25_bitwise_operations.zephyr`** - Learn about bitwise operations and their precedence

22. **Apply knowledge with `08_comprehensive_examples.zephyr`** - See real-world applications and complete examples

## Key Features Demonstrated

### Language Fundamentals
- **Complete syntax coverage** - All operators, control structures, and language constructs
- **Data type operations** - Comprehensive examples of working with all data types
- **Explicit typing** - Optional type annotations for variables, functions, and lambdas with type safety
- **Compound assignment** - Modern operators (+=, -=, *=, /=, ++, --) for efficient variable manipulation
- **Asynchronous programming** - async/await patterns, parallel execution, and Promise objects
- **Error handling** - Proper use of try/catch and debugging techniques

### Data Management
- **Complex data structures** - Nested lists, dictionaries, and mixed collections
- **Advanced indexing** - Multi-level indexing, chained access, and assignment patterns
- **Mixed chaining** - Any combination of member access and indexing (obj.list[i].member[key])
- **Data manipulation** - Sorting, filtering, transforming, and processing data

### Programming Paradigms
- **Functional programming** - Lambdas, higher-order functions, map/filter/reduce, lambda factories, composition
- **Object-oriented programming** - Classes, inheritance patterns, design patterns
- **Asynchronous programming** - async/await patterns, concurrent execution, Promise-based workflows
- **Event-driven programming** - Observer pattern, event handling systems
- **Modern syntax patterns** - Compound assignment, increment/decrement, and concise variable manipulation

### String Operations
- **String manipulation** - Concatenation, multiplication/repetition, formatting
- **Advanced formatting** - Document generation, table formatting, template systems
- **Integration** - String operations work seamlessly with all language features

### Real-World Applications
- **Complete applications** - Todo manager, banking system, library management
- **Async workflows** - Data fetching, parallel processing, batch operations
- **Design patterns** - Factory, Builder, Observer, Composite, Strategy patterns
- **Best practices** - Code organization, error handling, testing strategies

## Standardized Output Format

All examples now follow a consistent output format for better readability and learning experience:

### Format Structure
- **Header**: `=== Zephyr [Topic] Demo ===`
- **Sections**: Numbered sections with equal-sign dividers
- **Spacing**: Consistent blank lines between sections
- **Footer**: `=== [Topic] Demo Complete ===`

### Example Output Format
```
=== Zephyr Basic Features Demo ===

1. Variables and Data Types
===========================
Integer x = 10
Float y = 20.5

2. Arithmetic Operations
=========================
x + y = 30.5
...

=== Basic Features Demo Complete ===
```

### Testing All Examples
A test script is provided to verify all examples run successfully:

```bash
# Run the comprehensive test suite
./test_all_examples.sh
```

## How to Run Examples

To run any example file, use the Zephyr interpreter:

```bash
# Run individual example files
./bin/zephyr examples/01_basic_features.zephyr
./bin/zephyr examples/02_data_structures.zephyr
./bin/zephyr examples/03_functions_and_lambdas.zephyr

# Run async/await examples
./bin/zephyr examples/13_async_await_basic.zephyr
./bin/zephyr examples/14_async_parallel.zephyr
./bin/zephyr examples/15_async_promise_all.zephyr

# Run specific examples
./bin/zephyr examples/05_advanced_indexing.zephyr
./bin/zephyr examples/09_string_operations.zephyr
./bin/zephyr examples/10_compound_assignment.zephyr
./bin/zephyr examples/11_compound_assignment_comprehensive.zephyr
./bin/zephyr examples/12_lambda_dictionaries_nested.zephyr

./bin/zephyr examples/08_comprehensive_examples.zephyr

# Run interface example
./bin/zephyr examples/22_interfaces.zephyr
```

## File Organization Benefits

This organization provides several advantages:

1. **Progressive Learning** - Each file builds upon concepts from previous files
2. **Comprehensive Coverage** - All language features are thoroughly demonstrated
3. **Feature Integration** - Advanced features like chaining and string operations are shown throughout
4. **Real-World Context** - Examples show practical applications of language features
5. **Easy Navigation** - Clear file names and organization make finding examples simple
6. **Complete Reference** - Serves as both tutorial and reference documentation

## Contributing Examples

When adding new examples:

- **Follow the numbering scheme** for main example files (01-09)
- **Use descriptive comments** to explain what each section demonstrates
- **Include expected output** in comments where helpful
- **Group related functionality** together within sections
- **Showcase new features** like string multiplication and advanced chaining
- **Consider the target audience** - beginners to advanced users
- **Test all examples** to ensure they work correctly
- **Update this README** when adding significant new content

## Example Statistics

- **22 main organized files** covering all language aspects including async/await and error handling
- **Comprehensive coverage** of syntax, data structures, functions, classes, string operations, compound assignment, asynchronous programming, and exit handling
- **Advanced patterns** including chaining, design patterns, compound operators, async workflows, graceful shutdown, and real-world applications
- **Modern features** like string multiplication, mixed member/index chaining, compound assignment operators, async/await programming, and proper exit handling
- **Complete operator coverage** including +=, -=, *=, /=, ++, -- with all variants
- **Async programming support** with comprehensive examples of concurrent execution patterns
- **Error handling coverage** including try/catch patterns and exit() function usage for critical failures
- **Exit code documentation** with standardized ranges and best practices for production systems
- **Debugging support** with edge cases, troubleshooting examples, and critical system validation
- **Progressive difficulty** from basic syntax to complex applications, concurrent programming, and production-level error handling

This collection represents a complete learning resource for Zephyr, suitable for both newcomers learning the language and experienced developers looking for reference examples of specific features or patterns, including modern asynchronous programming capabilities and robust error handling practices.