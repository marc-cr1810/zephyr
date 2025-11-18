# Number Format Support in Zephyr

This document describes the implementation of hexadecimal, binary, and octal number literal support in the Zephyr programming language.

## Overview

Zephyr now supports four number literal formats:

- **Decimal**: `42`, `255`, `1024` (existing functionality)
- **Hexadecimal**: `0x2A`, `0xFF`, `0x400` (case-insensitive)
- **Binary**: `0b101010`, `0B11111111` (case-insensitive)
- **Octal**: `0o52`, `0O377` (case-insensitive)

All formats represent the same underlying integer values and can be used interchangeably in expressions, functions, and data structures.

## Syntax

### Hexadecimal Numbers
- Prefix: `0x` or `0X` (case-insensitive)
- Digits: `0-9`, `a-f`, `A-F` (case-insensitive)
- Examples: `0x1A`, `0XFF`, `0xaBcD`

### Binary Numbers
- Prefix: `0b` or `0B` (case-insensitive)
- Digits: `0`, `1`
- Examples: `0b1010`, `0B11111111`

### Octal Numbers
- Prefix: `0o` or `0O` (case-insensitive)
- Digits: `0-7`
- Examples: `0o52`, `0O377`

## Implementation Details

### Lexer Changes
- Added new token types: `hex_number`, `binary_number`, `octal_number`
- Extended lexer to recognize number patterns with prefixes
- Case-insensitive prefix recognition

### Parser Changes
- Added parsing support for new token types
- Proper prefix stripping before conversion
- Error handling for numbers that exceed integer limits

### Grammar Updates
- Updated ANTLR grammar to include new number literal tokens
- Maintains backward compatibility with existing decimal numbers

## Examples

### Basic Usage
```zephyr
# All represent the value 255
decimal = 255
hex = 0xFF
binary = 0b11111111
octal = 0o377

print(decimal == hex and hex == binary and binary == octal)  # true
```

### Arithmetic Operations
```zephyr
result1 = 0x10 + 0b1010      # 16 + 10 = 26
result2 = 0o20 * 0x2         # 16 * 2 = 32
result3 = 0b1111 - 0o7       # 15 - 7 = 8
```

### Bit Manipulation
```zephyr
mask = 0xFF00      # Upper byte mask
value = 0x1234     # Test value
flags = 0b1010     # Some flags

masked_value = value & mask  # Bitwise AND
combined_flags = flags | 0b0101  # Bitwise OR
```

### In Functions
```zephyr
func process_address(base, offset) {
    return base + offset
}

memory_addr = process_address(0x1000, 0b1100)  # 4096 + 12 = 4108
```

### In Data Structures
```zephyr
# Lists
mixed_numbers = [42, 0x2A, 0b101010, 0o52]  # All equal to 42

# Dictionaries
colors = {
    "red": 0xFF0000,
    "green": 0x00FF00,
    "blue": 0x0000FF
}

# File permissions
permissions = [0o644, 0o755, 0o777]
```

## Common Use Cases

### Memory Addresses
```zephyr
base_address = 0x10000000
page_offset = 0x1000
final_address = base_address + page_offset
```

### File Permissions (Unix-style)
```zephyr
read_only = 0o444      # r--r--r--
read_write = 0o644     # rw-r--r--
executable = 0o755     # rwxr-xr-x
full_access = 0o777    # rwxrwxrwx
```

### Bit Flags
```zephyr
flag_read = 0b001      # 1
flag_write = 0b010     # 2
flag_execute = 0b100   # 4
all_flags = flag_read | flag_write | flag_execute  # 7
```

### Network and Protocol Values
```zephyr
http_port = 0x50       # Port 80
https_port = 0x1BB     # Port 443
status_ok = 0x200      # HTTP 200 OK
```

### Color Values (RGB)
```zephyr
red = 0xFF0000         # Pure red
green = 0x00FF00       # Pure green  
blue = 0x0000FF        # Pure blue
white = 0xFFFFFF       # White
black = 0x000000       # Black
```

## Error Handling

The parser includes proper error handling for:

- Numbers that exceed integer limits (throws `SyntaxError`)
- Malformed number literals (caught by lexer)
- Invalid digits for each base (e.g., `8` in octal, `G` in hex)

### Error Examples
```zephyr
# These would cause syntax errors:
# 0xG123      # Invalid hex digit 'G'
# 0b012       # Invalid binary digit '2'  
# 0o89        # Invalid octal digit '8' or '9'
# 0xFFFFFFFFFFFFFFF  # Number too large for integer
```

## Testing

Comprehensive test suites are available:

- `examples/29_number_formats.zephyr` - Full feature demonstration
- `examples/30_number_formats_simple.zephyr` - Basic validation
- `examples/31_number_formats_integration.zephyr` - Integration with language features
- `examples/32_number_formats_error_test.zephyr` - Edge cases and error conditions

## Backward Compatibility

This implementation maintains full backward compatibility:

- Existing decimal number literals continue to work unchanged
- No breaking changes to existing syntax
- All existing code remains valid

## Performance Considerations

- Lexing performance impact is minimal (only affects number token recognition)
- Parsing uses standard library functions (`std::stoi` with base parameter)
- Runtime performance is identical to decimal numbers (all become integers)
- No memory overhead compared to decimal literals

## Technical Implementation

### Files Modified

1. **Token Types** (`include/zephyr/token.hpp`)
   - Added `hex_number`, `binary_number`, `octal_number` token types

2. **Lexer** (`src/lexer.cpp`)  
   - Extended number recognition logic
   - Added prefix detection and validation
   - Case-insensitive prefix handling

3. **Parser** (`src/parser.cpp`)
   - Added parsing cases for new token types
   - Prefix stripping before conversion
   - Error handling for overflow conditions

4. **Grammar** (`grammar/Zephyr.g4`)
   - Added grammar rules for new number tokens
   - Updated factor rule to include all number types

### Conversion Logic

The parser converts each format using `std::stoi()` with appropriate base:
- Hexadecimal: `std::stoi(digits, nullptr, 16)`
- Binary: `std::stoi(digits, nullptr, 2)`
- Octal: `std::stoi(digits, nullptr, 8)`

Prefixes (`0x`, `0b`, `0o`) are stripped before conversion to avoid confusion.

## Future Enhancements

Potential future improvements could include:

- Support for 64-bit integer literals (`0x1234567890ABCDEFL`)
- Hexadecimal floating-point literals (`0x1.ABCp+10`)
- Underscore separators for readability (`0x1234_5678`)
- Additional numeric bases (base64, etc.)

## Conclusion

The number format implementation provides comprehensive support for multiple numeric representations while maintaining backward compatibility and robust error handling. This enhancement makes Zephyr more suitable for systems programming, embedded development, and applications requiring precise numeric control.