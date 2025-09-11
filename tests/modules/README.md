# Zephyr Module System Test Suite

This directory contains comprehensive tests for the modern Zephyr module system. The module system provides intuitive import syntax and proper module encapsulation.

## 🎯 Module System Overview

The Zephyr module system supports clean, intuitive import patterns:

### ✅ Import Patterns

- **Lazy Import**: `import math` → Access as `math.add()`, `math.PI`
- **Lazy Import with Alias**: `import math_extra as math` → Access as `math.add()`
- **Named Imports**: `import add, PI from math` → Direct access to `add()`, `PI`
- **Named with Module Alias**: `import add from math as mathlib` → Both `add()` and `mathlib.PI`
- **String Imports**: `import "./lib/math.zephyr"` → Access as `math.add()`
- **String with Alias**: `import "./lib/math.zephyr" as utils` → Access as `utils.add()`
- **Subdirectory Imports**: `import math.advanced` → Access as `advanced.factorial()`

### ✅ Core Features

- **Module Objects**: Direct member access via dot notation (`module.function()`)
- **Internal Keyword**: `internal` functions/variables are private to the module
- **Proper Scoping**: Module functions maintain access to their module's global scope
- **Module Identity**: `__module_name__` variable available in every module
- **Smart Resolution**: Supports both name-based and path-based module resolution
- **Subdirectory Support**: `math.advanced` searches for `./math/advanced.zephyr`

## 📁 Test Files

### Library Modules

| File | Purpose |
|------|---------|
| `math_utils.zephyr` | Basic math functions and constants |
| `math_with_internal.zephyr` | Module with internal (private) functions |
| `math/advanced.zephyr` | Advanced math functions in subdirectory |

### Test Scripts

| File | Tests |
|------|-------|
| `import_test.zephyr` | Comprehensive test of all import patterns |
| `basic_test.zephyr` | Simple syntax validation tests |

## 🚀 Running Tests

### Prerequisites
- Built Zephyr interpreter in `../../bin/zephyr`
- Run from the `tests/modules` directory

### Test Execution

```bash
cd tests/modules

# Comprehensive test suite
../../bin/zephyr import_test.zephyr

# Basic syntax tests  
../../bin/zephyr basic_test.zephyr

# Run library modules directly
../../bin/zephyr math_utils.zephyr
../../bin/zephyr math/advanced.zephyr
```

## 📋 Expected Test Output

### import_test.zephyr
```
Testing Zephyr import system...

=== Test 1: Lazy Import ===
math_utils.PI: 3.14159
math_utils.add(5, 3): 8
math_utils.multiply(4, 6): 24

=== Test 2: Lazy Import with Alias ===
math.PI: 3.14159
math.add(10, 20): 30
math.circle_area(5): 78.53975

=== Test 3: Named Imports ===
Named import add(7, 8): 15
Named import PI: 3.14159

=== Test 4: Named Imports with Module Alias ===
Named import multiply(3, 9): 27
Named import E: 2.71828
Module alias mathlib.PI: 3.14159

=== Test 5: String Import ===
math_utils.add(1, 2): 3

=== Test 6: String Import with Alias ===
mymath.multiply(5, 5): 25
mymath.E: 2.71828

=== Test 7: Subdirectory Import ===
advanced.GOLDEN_RATIO: 1.61803398874989
advanced.factorial(5): 120
advanced.fibonacci(8): 21
advanced.is_prime(17): true

=== Test 8: Subdirectory Import with Alias ===
advmath.power_of_two(6): 64
advmath.gcd(48, 18): 6
advmath.lcm(12, 8): 24

All import patterns working correctly!
```

## 🔧 Usage Examples

### Basic Import Patterns

```zephyr
# Lazy import - clean and simple
import math_utils
result = math_utils.add(5, 3)
value = math_utils.PI

# Alias for shorter names
import math_utils as math  
result = math.multiply(4, 6)

# Named imports for frequently used symbols
import add, subtract, PI from math_utils
result = add(PI, subtract(10, 5))

# String imports for relative files
import "./utils/helpers.zephyr" as helpers
helpers.format_string("hello")

# Subdirectory imports
import math.advanced
result = advanced.factorial(5)
```

### Internal/Private Declarations

```zephyr
# Public - can be imported
const PI = 3.14159
func add(a, b) { return a + b }

# Private - cannot be imported  
internal const SECRET = "hidden"
internal func helper() { return "private" }

# Module identity
if __module_name__ == "__main__" {
    print("Running as main script")
}
```

### Module Resolution

```zephyr
# Name-based: searches current dir and ZEPHYRPATH
import math

# Path-based: relative to current file
import "./lib/utils.zephyr"

# Subdirectory: converts dots to path separators
import math.advanced  # → ./math/advanced.zephyr
```

## ✨ Key Advantages

### Clean Syntax
- No confusing `import *` or bracket notation
- Natural dot notation: `module.function()`
- Intuitive alias system

### Proper Scoping
- Module functions can access module variables
- No variable collision between modules
- Predictable scope resolution

### Flexible Resolution
- Smart subdirectory handling
- Both relative and absolute imports
- Environment variable support (ZEPHYRPATH)

## 🔄 Module System Status

| Feature | Status |
|---------|--------|
| Lazy Imports | ✅ Complete |
| Named Imports | ✅ Complete |
| String Imports | ✅ Complete |
| Module Aliases | ✅ Complete |
| Subdirectory Imports | ✅ Complete |
| Internal Keyword | ✅ Complete |
| Proper Scoping | ✅ Complete |
| Module Objects | ✅ Complete |
| Direct Function Calls | ✅ Complete |

**Status: PRODUCTION READY** 🎉

## 📖 References

- [MODULE_DESIGN.md](../../MODULE_DESIGN.md) - Technical specification
- [NEW_IMPORT_SYSTEM.md](../../NEW_IMPORT_SYSTEM.md) - Complete documentation
- [ZEPHYR.md](../../ZEPHYR.md) - Full language reference