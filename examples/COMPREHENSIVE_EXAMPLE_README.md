# Comprehensive Embedded Zephyr C++ Example

This directory contains a comprehensive example demonstrating how to embed Zephyr in a C++ application with advanced features including classes, functions, variables, and complex system integration.

## Overview

The comprehensive example showcases:

- **Object-Oriented Design**: Classes with inheritance, polymorphism, and virtual functions
- **Template Programming**: Generic functions and template specialization
- **STL Integration**: Vectors, maps, algorithms, and smart pointers
- **Memory Management**: RAII principles and automatic resource management
- **Real-time Systems**: Game loop simulation with entity management
- **Mathematical Computing**: Advanced math utilities and statistical analysis
- **Native Module System**: Integration between C++ and Zephyr scripting

## Files

### C++ Implementation
- `comprehensive_embedded_example.cpp` - Main C++ application with embedded Zephyr
- `comprehensive_embedded_demo.zephyr` - Zephyr script demonstrating the C++ integration

### Key Components

#### 1. Entity System
```cpp
class Entity {
    // Abstract base class for all game objects
    // Demonstrates inheritance and polymorphism
};

class Player : public Entity {
    // Player-specific functionality with inventory and progression
};

class Enemy : public Entity {
    // AI-driven enemies with combat abilities
};

class PowerUp : public Entity {
    // Collectible items with time-based expiration
};
```

#### 2. Game World Management
```cpp
class GameWorld {
    // Entity container with template-based factory methods
    // STL algorithms for entity management
    // Statistics tracking and performance monitoring
};
```

#### 3. Utility Classes
```cpp
class MathUtils {
    // Template functions for mathematical operations
    // Random number generation and interpolation
    // Geometric calculations
};

class Timer {
    // High-resolution timing using chrono library
    // Performance measurement capabilities
};
```

#### 4. Native Modules
```cpp
class game_engine_module_t : public native_module_t {
    // Entity creation and management functions
    // World simulation and update mechanics
    // Player progression and inventory system
};

class math_utils_module_t : public native_module_t {
    // Mathematical utility functions
    // Constants and conversion utilities
    // Statistical analysis tools
};
```

## Building the Example

### Prerequisites
- C++17 compatible compiler
- CMake 3.10 or higher
- Zephyr development environment

### Build Commands
```bash
# Configure with embedding examples enabled
mkdir build && cd build
cmake -DBUILD_EMBEDDING_EXAMPLE=ON ..

# Build the comprehensive example
make comprehensive_embedded_example

# The executable will be available in:
# - build/comprehensive_embedded_example
# - bin/comprehensive_embedded_example (copied automatically)
```

### Running the Example
```bash
# Run the C++ embedded example
./bin/comprehensive_embedded_example

# Or run the Zephyr script version
./bin/zephyr examples/comprehensive_embedded_demo.zephyr
```

## Features Demonstrated

### 1. C++ Language Features

#### Object-Oriented Programming
- **Inheritance**: Entity base class with Player, Enemy, PowerUp derived classes
- **Polymorphism**: Virtual functions for type-specific behavior
- **Encapsulation**: Private/protected members with public interfaces
- **Abstract Classes**: Pure virtual functions defining interfaces

#### Template Programming
- **Function Templates**: Generic mathematical operations
- **Template Specialization**: Type-specific optimizations
- **Factory Methods**: Template-based entity creation

#### Modern C++ Features
- **Smart Pointers**: Automatic memory management with unique_ptr
- **Lambda Functions**: Inline function definitions for callbacks
- **Range-based Loops**: Modern iteration syntax
- **Auto Type Deduction**: Simplified variable declarations

#### STL Integration
- **Containers**: vector, map for data storage
- **Algorithms**: find_if, remove_if, sort for data manipulation
- **Iterators**: Standard iteration patterns
- **Functors**: Function objects for custom operations

### 2. System Design Patterns

#### Factory Pattern
```cpp
template<typename T, typename... Args>
T* GameWorld::create_entity(Args&&... args) {
    // Template-based factory for entity creation
}
```

#### Observer Pattern
- Entity update system with polymorphic behavior
- Event-driven architecture for game events

#### RAII (Resource Acquisition Is Initialization)
- Automatic resource cleanup with destructors
- Exception-safe resource management

### 3. Mathematical Computing

#### Vector Mathematics
- 3D position and distance calculations
- Geometric transformations and projections
- Interpolation and smoothing functions

#### Statistical Analysis
- Random number generation with multiple distributions
- Statistical measure calculations (mean, median, standard deviation)
- Data sampling and analysis

#### Numerical Methods
- Clamping and normalization functions
- Angle calculations and conversions
- Interpolation algorithms (linear, cubic)

### 4. Real-time Systems

#### Game Loop Architecture
- Fixed timestep simulation
- Entity lifecycle management
- Performance monitoring and profiling

#### Multi-threaded Considerations
- Thread-safe operations where applicable
- Atomic operations for shared state
- Lock-free data structures

### 5. Zephyr Integration

#### Native Module System
```cpp
class game_engine_module_t : public native_module_t {
    auto initialize() -> void_result_t override {
        // Register functions and constants
        register_function("create_player", ...);
        export_constant("PI", ...);
    }
};
```

#### Type Conversion
- Automatic conversion between C++ and Zephyr types
- Support for complex data structures
- Error handling and validation

#### Function Registration
- Lambda-based function binding
- Overloaded function support
- Variadic template arguments

## Example Output

When you run the comprehensive example, you'll see output demonstrating:

```
=== Comprehensive Embedded Zephyr C++ Example ===
Demonstrating classes, functions, variables, and system integration

Created Zephyr engine: Game Engine Demo
Registering native modules...
Modules registered successfully!

=== Test 1: Game World Initialization ===
Initializing game world...
Created entities:
  Player ID: 1
  Enemy 1 ID: 2
  Enemy 2 ID: 3
  PowerUp ID: 4
Total entities: 4
✅ World initialization: Game world initialized successfully

=== Test 2: Entity Management ===
Testing entity management...
Player position: ( -3.45 , 7.82 , 0)
Distance to Enemy 1: 8.92
Distance to Enemy 2: 12.33
Player info: Entity 1 (Player): Hero at (-3.45, 7.82, 0.00), Health: 100.00
Enemy 1 info: Entity 2 (Goblin): Goblin_01 at (2.18, -1.45, 0.00), Health: 50.00
✅ Entity management: Entity management completed

...and much more
```

## Learning Objectives

After studying this example, you should understand:

1. **C++ Best Practices**: Modern C++ idioms and patterns
2. **System Architecture**: How to design scalable, maintainable systems
3. **Memory Management**: RAII principles and smart pointer usage
4. **Template Programming**: Generic programming techniques
5. **STL Mastery**: Effective use of standard library components
6. **Real-time Systems**: Game loop and simulation architectures
7. **Language Interop**: Integrating C++ with scripting languages
8. **Mathematical Computing**: Numerical methods and algorithms
9. **Design Patterns**: Factory, Observer, and other common patterns
10. **Performance Considerations**: Optimization and profiling techniques

## Extensions and Modifications

### Adding New Entity Types
```cpp
class Projectile : public Entity {
public:
    Projectile(const std::string& name, double speed, double damage)
        : Entity(name), speed(speed), damage(damage) {}
    
    void update(double delta_time) override {
        // Physics simulation for projectile motion
    }
    
    std::string get_type() const override { return "Projectile"; }
};
```

### Custom Mathematical Functions
```cpp
// Add to math_utils_module_t
register_function("bezier_curve", [](double t, double p0, double p1, double p2, double p3) -> double {
    double u = 1.0 - t;
    return u*u*u*p0 + 3*u*u*t*p1 + 3*u*t*t*p2 + t*t*t*p3;
});
```

### Advanced AI Systems
```cpp
class AIController {
public:
    virtual void update_ai(Entity* entity, const GameWorld& world, double delta_time) = 0;
};

class AggressiveAI : public AIController {
    void update_ai(Entity* entity, const GameWorld& world, double delta_time) override {
        // Implement aggressive behavior
    }
};
```

## Performance Notes

- Entity updates use polymorphic dispatch (virtual function calls)
- STL containers provide amortized O(1) insertion for vectors
- Template functions are inlined for optimal performance
- Smart pointers have minimal overhead compared to raw pointers
- Function registration uses type erasure for flexibility

## Thread Safety

This example is designed for single-threaded execution. For multi-threaded applications, consider:

- Protecting shared GameWorld state with mutexes
- Using atomic operations for simple counters
- Implementing lock-free data structures for high-performance scenarios
- Separating rendering and simulation threads

## Debugging Tips

- Use debugger to inspect entity hierarchies
- Enable Zephyr debug mode for script debugging
- Add logging statements for performance profiling
- Use static analysis tools for code quality
- Implement unit tests for individual components

## Related Examples

- `embedded_native_classes.cpp` - Simpler class integration example
- `simple_embedded_classes.cpp` - Basic embedding patterns
- `embedding_example.cpp` - Minimal embedding setup
- Plugin examples in `examples/plugins/` - External module development

## Contributing

When extending this example:

1. Follow existing code style and patterns
2. Add comprehensive documentation for new features
3. Include test cases for new functionality
4. Update this README with new capabilities
5. Consider performance implications of changes

## License

This example is part of the Zephyr project and follows the same license terms.