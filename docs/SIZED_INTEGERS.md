# Zephyr Sized Integer System

## Overview

Zephyr features a powerful unified integer system that supports both regular integers and explicitly sized integer types. This system provides type safety, automatic conversions, and seamless interoperability while maintaining high performance through a single unified implementation.

## Supported Integer Types

### Signed Integer Types
- `i8` - 8-bit signed integer (-128 to 127)
- `i16` - 16-bit signed integer (-32,768 to 32,767)
- `i32` - 32-bit signed integer (-2,147,483,648 to 2,147,483,647)
- `i64` - 64-bit signed integer (-9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)

### Unsigned Integer Types
- `u8` - 8-bit unsigned integer (0 to 255)
- `u16` - 16-bit unsigned integer (0 to 65,535)
- `u32` - 32-bit unsigned integer (0 to 4,294,967,295)
- `u64` - 64-bit unsigned integer (0 to 18,446,744,073,709,551,615)

### Default Integer Type
- `int` - Default integer type (equivalent to `i32` but maintains separate identity)

## Architecture

### Unified Implementation

All integer types in Zephyr are implemented using a single C++ class `int_object_t`. This unified approach provides:

- **Consistent Behavior**: All integer operations work the same way regardless of size
- **Memory Efficiency**: Single implementation reduces code bloat
- **Seamless Interoperability**: All integer types can work together naturally
- **Performance**: No overhead for type conversions between compatible sizes

### Type System Integration

The sized integer system is fully integrated with Zephyr's type system:

- **Type Annotations**: Use explicit type annotations for variables and function parameters
- **Type Inference**: Automatic type selection based on literal values and context
- **Type Promotion**: Automatic promotion to larger types when needed for arithmetic
- **Type Safety**: Runtime validation and appropriate error handling

## Basic Usage

### Variable Declarations

```zephyr
# Explicit type annotations
small_val: u8 = 255
medium_val: i16 = -32000
large_val: u64 = 18446744073709551615

# Type inference from literals
auto_int = 42          # Defaults to int
auto_large = 5000000000 # Automatically becomes u32/i64 based on size
```

### Arithmetic Operations

```zephyr
a: u8 = 100
b: u8 = 50

sum = a + b            # Result type based on operation
difference = a - b
product = a * b
quotient = a / b       # May promote to float for fractional results
```

### Type Promotion Rules

Zephyr automatically promotes integer types in arithmetic operations:

1. **Same Type Operations**: `u8 + u8` → Result type depends on value magnitude
2. **Mixed Type Operations**: Promote to the larger/more capable type
3. **Overflow Handling**: Promote to larger integer types (not float) when possible
4. **Fractional Results**: Only promote to float when result is truly fractional

## Function Parameter and Return Types

### Explicit Parameter Types

Functions can specify exact parameter types with automatic conversion:

```zephyr
func process_byte(value: u8) {
    print("Processing byte:", value)
    return value * 2
}

# All of these work - automatic conversion from int literals
result1 = process_byte(42)     # int literal → u8 parameter
result2 = process_byte(255)    # int literal → u8 parameter
```

### Explicit Return Types

Functions can specify return types with automatic conversion:

```zephyr
func compute_checksum(data: [u8]) : u16 {
    total = 0
    for byte in data {
        total = total + byte  # int expression
    }
    return total  # Automatic conversion: int → u16
}
```

### Mixed Parameter and Return Types

```zephyr
func convert_temperature(celsius: i16) : u8 {
    # Complex computation returns int
    fahrenheit = celsius * 9 / 5 + 32
    
    # Automatic conversion: int → u8 (with range validation)
    return fahrenheit
}

# Call with int literal - automatic parameter conversion
temp_f = convert_temperature(25)  # int(25) → i16, result → u8
```

## Type Conversion Rules

### Implicit Conversions

The system supports implicit conversions between compatible integer types:

#### Function Parameters
- `int` literals can be passed to any sized integer parameter
- Sized integers can be passed to other sized integer parameters
- Conversion happens automatically at call site

#### Function Returns
- `int` expressions can be returned as any sized integer type
- Sized integer expressions can be returned as other sized integer types
- Conversion happens automatically at return statement

#### Conversion Priority
1. **Exact Match**: No conversion needed (highest priority)
2. **int → Sized**: `int` literals to sized parameters (high priority)
3. **Sized → Sized**: Between different sized types (medium priority)
4. **Sized → int**: Sized types back to int (lower priority)

### Range Handling

When converting between types with different ranges:

- **In-Range Values**: Converted exactly
- **Out-of-Range Values**: Truncated/wrapped according to target type
- **Negative to Unsigned**: Uses two's complement wraparound
- **Error Handling**: Clear error messages for conversion failures

```zephyr
func demo_range_handling() {
    # These work fine
    small: u8 = 200        # 200 fits in u8
    
    # Out-of-range handling
    overflow: u8 = 300     # 300 becomes 44 (300 % 256)
    negative: u8 = -10     # -10 becomes 246 (wraparound)
}
```

## Advanced Features

### Collections with Mixed Types

```zephyr
# Create collections with different integer types
mixed_data = [
    cast(u8, 255),      # Max u8 value
    cast(i16, -32768),  # Min i16 value
    cast(u32, 4000000)  # Large u32 value
]

for value in mixed_data {
    print("Value:", value, "Type:", type(value))
}
```

### Type-Specific Operations

```zephyr
# Bitwise operations work naturally
flags: u8 = 0b10101010
mask: u8 = 0b11110000
result = flags & mask

# Type information at runtime
func analyze_integer(val) {
    print("Value:", val)
    print("Type:", type(val))
    
    if type(val) == "u8" {
        print("8-bit unsigned integer")
    } else if type(val) == "i16" {
        print("16-bit signed integer")
    }
}
```

### Memory-Efficient Data Structures

```zephyr
# Use appropriate types for memory efficiency
sensor_readings: [u8] = []      # Temperature values 0-100°C
device_ids: [u16] = []          # Device IDs up to 65535
timestamps: [u32] = []          # Unix timestamps
large_counters: [u64] = []      # High-precision counters

# Functions can work with specific types
func process_sensor_data(readings: [u8]) : u16 {
    total: u16 = 0
    for reading in readings {
        total = total + reading
    }
    return total / len(readings)  # Average as u16
}
```

## Power Operations and Type Promotion

The power operator (`**`) has special handling to ensure mathematical correctness:

```zephyr
# Integer power operations promote appropriately
small_power = 2 ** 8      # 256 → int
large_power = 10 ** 12    # 1000000000000 → i64 (not float!)
huge_power = 2 ** 100     # Too large → float (mathematical necessity)

# Fractional exponents always produce floats
fractional = 2 ** -3      # 0.125 → float (mathematically correct)
```

## Best Practices

### When to Use Sized Integers

1. **Memory Constraints**: Use smaller types when memory usage matters
2. **API Boundaries**: Match external API expectations (network protocols, file formats)
3. **Range Validation**: Enforce value ranges through type system
4. **Performance**: Use native machine word sizes when possible

### Function Design

```zephyr
# Good: Clear parameter types for API boundaries
func read_network_packet(length: u16, flags: u8) : [u8] {
    # Implementation
}

# Good: Return types that match expected ranges
func calculate_percentage(part: u32, total: u32) : u8 {
    return (part * 100) / total  # Result fits in u8
}

# Good: Use int for general computation
func general_math(a, b) {
    return a * b + 42  # Let the system handle types naturally
}
```

### Error Handling

```zephyr
func safe_conversion(large_value) : u8 {
    if large_value < 0 or large_value > 255 {
        print("Warning: Value out of u8 range")
    }
    return large_value  # Automatic truncation with warning
}
```

## Examples and Use Cases

### Network Protocol Handling

```zephyr
func parse_packet_header(data: [u8]) : dictionary {
    return {
        "version": data[0],           # u8
        "length": (data[1] << 8) | data[2],  # u16
        "sequence": parse_u32(data[3:7]),    # u32
        "checksum": data[7]           # u8
    }
}
```

### Game Development

```zephyr
func update_health(current: u8, damage: u8) : u8 {
    if damage >= current {
        return 0  # Dead
    }
    return current - damage
}

func calculate_experience(base: u32, multiplier: u8) : u32 {
    return base * multiplier
}
```

### Data Processing

```zephyr
func process_measurements(data: [u16]) : dictionary {
    total: u32 = 0
    count: u16 = 0
    
    for measurement in data {
        total = total + measurement
        count = count + 1
    }
    
    return {
        "average": total / count,
        "total": total,
        "count": count
    }
}
```

## Performance Considerations

### Unified System Benefits

- **No Conversion Overhead**: Compatible types convert without object creation
- **Single Implementation**: Reduced code size and better cache utilization  
- **Optimized Operations**: Single arithmetic implementation for all integer types
- **Memory Efficiency**: Optimal storage based on actual value ranges

### When Performance Matters

- Use native word sizes (`int`, `u32`, `i64`) for computational loops
- Use smaller types (`u8`, `u16`) for large collections
- Let the system handle type promotion in mixed operations
- Avoid unnecessary explicit conversions

## Integration with Existing Code

The sized integer system is fully backward compatible:

```zephyr
# Existing code continues to work
old_function(42, "hello", 3.14)

# New code can gradually adopt sizing
func new_function(port: u16, buffer_size: u32) {
    # Mix old and new patterns
    result = old_function(port, "data", buffer_size * 0.5)
    return result
}
```

## Conclusion

Zephyr's unified sized integer system provides the best of both worlds: the safety and expressiveness of explicit integer sizing with the simplicity and performance of a unified implementation. The automatic conversion system makes the syntax natural while maintaining type safety, and the intelligent promotion rules ensure mathematical correctness in all operations.

Whether you're writing system-level code that needs precise control over data sizes, or application-level code that benefits from natural integer semantics, the sized integer system adapts to your needs while maintaining consistent, predictable behavior.