# Zephyr Import System

This document provides a comprehensive guide to Zephyr's modern import system, covering all import patterns, best practices, module organization, and real-world examples. The import system is designed to be intuitive, flexible, and powerful for organizing code across projects of any size.

---

## Overview

Zephyr's import system supports eight distinct import patterns, each optimized for different use cases. The system emphasizes clarity, flexibility, and natural syntax that feels familiar to developers from various language backgrounds.

### Core Principles

- **Intuitive Syntax**: Natural language patterns that are easy to read and write
- **Flexible Resolution**: Support for both name-based and path-based imports  
- **Smart Module Objects**: Direct member access via dot notation (`module.function()`)
- **Subdirectory Support**: Automatic conversion of dotted names to directory structures
- **Privacy Control**: `internal` keyword for module-private symbols

---

## Import Pattern Reference

### 1. Lazy Import

**Syntax:** `import module_name`

**Description:** Imports everything that isn't marked `internal` from a module, accessible via `module_name.symbol`.

```zephyr
import math_utils
result = math_utils.add(5, 3)
pi_value = math_utils.PI
area = math_utils.circle_area(radius)
```

**When to Use:**
- When you need multiple symbols from a module
- For clean namespace organization
- When module names are short and descriptive

---

### 2. Lazy Import with Alias

**Syntax:** `import module_name as alias`

**Description:** Same as lazy import, but with a custom alias for the module.

```zephyr
import math_utilities as math
import very_long_module_name as vm
import third_party_library as tpl

result = math.multiply(4, 6)
vm.do_something()
tpl.initialize()
```

**When to Use:**
- When module names are too long
- To avoid naming conflicts
- For better code readability

---

### 3. Named Imports

**Syntax:** `import symbol1, symbol2 from module_name`

**Description:** Imports specific symbols directly into the current scope.

```zephyr
import add, subtract, multiply from math_utils
import PI, E from constants

result = add(multiply(5, PI), subtract(E, 1))
```

**When to Use:**
- When you only need a few specific functions/constants
- For frequently used symbols
- To reduce typing and improve readability

---

### 4. Named Imports with Module Alias

**Syntax:** `import symbol1, symbol2 from module_name as alias`

**Description:** Imports specific symbols AND creates a module alias for accessing other symbols.

```zephyr
import PI, E from math_constants as constants
import add, multiply from math_utils as math

# Use both direct imports and module access
result = add(PI, constants.GOLDEN_RATIO)
area = multiply(PI, math.square(radius))
```

**When to Use:**
- When you need some symbols frequently and others occasionally
- For flexible access patterns
- When building complex mathematical or scientific code

---

### 5. String Imports

**Syntax:** `import "path/to/file.zephyr"`

**Description:** Imports a module using a file path. Module name becomes the filename.

```zephyr
import "./lib/utilities.zephyr"
import "./helpers/string_utils.zephyr"
import "../shared/common.zephyr"

utilities.format_date(date)
string_utils.capitalize(text)
common.log_message(message)
```

**When to Use:**
- For relative imports within a project
- When importing from specific file locations
- For explicit path-based module loading

---

### 6. String Imports with Alias

**Syntax:** `import "path/to/file.zephyr" as alias`

**Description:** Imports a file-based module with a custom alias.

```zephyr
import "./lib/math_functions.zephyr" as math
import "./utils/database_helpers.zephyr" as db
import "../../common/validation.zephyr" as validator

result = math.calculate_distance(x1, y1, x2, y2)
user = db.find_user_by_id(123)
is_valid = validator.check_email(email)
```

**When to Use:**
- When filename doesn't make a good module name
- To avoid conflicts with other modules
- For cleaner, more semantic naming

---

### 7. Subdirectory Imports

**Syntax:** `import module.submodule`

**Description:** Imports modules from subdirectories. Dots are converted to path separators.

```zephyr
import math.advanced      # Searches ./math/advanced.zephyr
import utils.string       # Searches ./utils/string.zephyr  
import graphics.geometry  # Searches ./graphics/geometry.zephyr

result = advanced.factorial(10)
text = string.trim_whitespace(input)
area = geometry.triangle_area(base, height)
```

**When to Use:**
- For hierarchical code organization
- When building larger projects with logical groupings
- For namespace organization similar to packages

---

### 8. Subdirectory Imports with Alias

**Syntax:** `import module.submodule as alias`

**Description:** Imports from subdirectories with a custom alias.

```zephyr
import math.advanced as adv_math
import graphics.rendering.opengl as gl
import data.processing.statistics as stats

result = adv_math.fibonacci(20)
gl.render_triangle(vertices)
mean = stats.calculate_mean(data_points)
```

**When to Use:**
- When subdirectory names are too long
- For shorter, more convenient aliases
- When integrating multiple similar modules

---

## Privacy and Encapsulation

### The Internal Keyword

Use `internal` to make functions, constants, and classes private to their module:

```zephyr
# math_utils.zephyr

# Public - can be imported
const PI = 3.14159
func add(a, b) { return a + b }

# Private - cannot be imported  
internal const SECRET_MULTIPLIER = 1.618
internal func validate_input(x) {
    return x != none and x > 0
}

func safe_divide(a, b) {
    if not validate_input(b) {
        return none
    }
    return a / b
}
```

**Benefits:**
- Clear separation between public API and implementation details
- Prevents accidental usage of helper functions
- Enables refactoring without breaking external code

---

## Module Resolution

### Search Paths

Zephyr uses a multi-step resolution process:

1. **Current Directory**: Where the importing file is located
2. **ZEPHYRPATH**: Environment variable with additional search paths

```bash
# Set additional module search paths
export ZEPHYRPATH="/usr/local/lib/zephyr:/home/user/zephyr_modules"
```

### Resolution Examples

```zephyr
# Name-based resolution
import math              # Searches: ./math.zephyr, then ZEPHYRPATH
import utils.string      # Searches: ./utils/string.zephyr, then ZEPHYRPATH

# Path-based resolution  
import "./lib/math.zephyr"      # Relative to current file
import "../shared/utils.zephyr" # Relative to current file
```

---

## Best Practices

### 1. Choose the Right Import Pattern

```zephyr
# Good: Lazy import for multiple symbols
import math_utils as math
result = math.add(math.multiply(x, math.PI), math.E)

# Good: Named imports for frequently used symbols
import add, multiply, PI from math_utils
result = add(multiply(x, PI), 2.71828)

# Avoid: Mixed patterns that reduce clarity
import add from math_utils
import math_utils as math  # Confusing - pick one approach
```

### 2. Use Descriptive Aliases

```zephyr
# Good: Clear, descriptive aliases
import database_connection_pool as db_pool
import user_authentication_service as auth
import email_notification_system as notifications

# Avoid: Cryptic abbreviations
import database_connection_pool as dcp
import user_authentication_service as uas
```

### 3. Group Related Imports

```zephyr
# System/library imports first
import math
import string_utils
import data_structures.queue

# Local project imports
import "./models/user.zephyr" as user_model
import "./services/email.zephyr" as email_service
import "./utils/validation.zephyr" as validator
```

### 4. Use Internal for Implementation Details

```zephyr
# user_service.zephyr
import "./models/user.zephyr" as user_model
import "./utils/crypto.zephyr" as crypto

# Public API
func create_user(name, email, password) {
    if not validate_user_data(name, email, password) {
        return none
    }
    
    hashed_password = hash_password(password)
    return user_model.User(name, email, hashed_password)
}

# Private helpers
internal func validate_user_data(name, email, password) {
    return name != none and email != none and password != none
}

internal func hash_password(password) {
    return crypto.sha256(password + SECRET_SALT)
}

internal const SECRET_SALT = "zephyr_secret_2024"
```

---

## Real-World Examples

### Example 1: Web Application Structure

```
project/
├── main.zephyr
├── models/
│   ├── user.zephyr
│   ├── product.zephyr
│   └── order.zephyr
├── services/
│   ├── auth.zephyr
│   ├── email.zephyr
│   └── payment.zephyr
├── utils/
│   ├── validation.zephyr
│   ├── formatting.zephyr
│   └── crypto.zephyr
└── lib/
    ├── database.zephyr
    └── http_client.zephyr
```

```zephyr
# main.zephyr
import models.user as user_model
import services.auth as auth_service
import services.email as email_service
import "./lib/database.zephyr" as db

func create_account(name, email, password) {
    # Validate input
    if not auth_service.validate_credentials(email, password) {
        return {"success": false, "error": "Invalid credentials"}
    }
    
    # Create user
    user = user_model.create_user(name, email, password)
    if user == none {
        return {"success": false, "error": "User creation failed"}
    }
    
    # Save to database
    if not db.save_user(user) {
        return {"success": false, "error": "Database error"}
    }
    
    # Send welcome email
    email_service.send_welcome_email(user.email, user.name)
    
    return {"success": true, "user_id": user.id}
}
```

### Example 2: Mathematical Computing Library

```zephyr
# scientific_computing.zephyr
import math.linear_algebra as linalg
import math.statistics as stats
import math.calculus as calc
import "./visualization/plotting.zephyr" as plot

func analyze_dataset(data) {
    # Statistical analysis
    mean = stats.mean(data)
    std_dev = stats.standard_deviation(data)
    correlation_matrix = stats.correlation_matrix(data)
    
    # Linear algebra operations
    eigenvalues = linalg.eigenvalues(correlation_matrix)
    eigenvectors = linalg.eigenvectors(correlation_matrix)
    
    # Calculus operations
    derivatives = []
    for column in data {
        derivative = calc.numerical_derivative(column)
        append(derivatives, derivative)
    }
    
    # Visualization
    plot.histogram(data, "Data Distribution")
    plot.correlation_heatmap(correlation_matrix, "Correlations")
    
    return {
        "summary": {"mean": mean, "std_dev": std_dev},
        "eigen_analysis": {"values": eigenvalues, "vectors": eigenvectors},
        "derivatives": derivatives
    }
}
```

### Example 3: Game Development

```zephyr
# game_engine.zephyr
import graphics.renderer as renderer
import graphics.sprites as sprites
import audio.sound_manager as audio
import input.keyboard as keyboard
import input.mouse as mouse
import physics.collision as collision
import "./game_objects/player.zephyr" as player_module
import "./game_objects/enemy.zephyr" as enemy_module
import "./levels/level_manager.zephyr" as levels

class Game {
    func init() {
        this.renderer = renderer.create_renderer(800, 600)
        this.player = player_module.create_player(400, 500)
        this.enemies = []
        this.current_level = 1
        
        # Load initial level
        levels.load_level(this.current_level)
        audio.play_background_music("theme.mp3")
    }
    
    func update(delta_time) {
        # Handle input
        if keyboard.is_pressed("LEFT") {
            this.player.move_left(delta_time)
        }
        if keyboard.is_pressed("RIGHT") {
            this.player.move_right(delta_time)
        }
        if keyboard.is_pressed("SPACE") {
            this.player.shoot()
        }
        
        # Update game objects
        this.player.update(delta_time)
        for enemy in this.enemies {
            enemy.update(delta_time)
            
            # Check collisions
            if collision.check_collision(this.player, enemy) {
                this.handle_player_enemy_collision(enemy)
            }
        }
        
        # Check level completion
        if len(this.enemies) == 0 {
            this.advance_to_next_level()
        }
    }
    
    func render() {
        renderer.clear_screen()
        this.player.render(this.renderer)
        for enemy in this.enemies {
            enemy.render(this.renderer)
        }
        renderer.present()
    }
}
```

---

## Module Organization Patterns

### 1. Layered Architecture

```
project/
├── presentation/     # UI layer
├── business/        # Business logic
├── data/           # Data access
└── infrastructure/ # External services
```

```zephyr
# Presentation layer imports from business
import business.user_service as user_service
import business.order_service as order_service

# Business layer imports from data
import data.user_repository as user_repo
import data.order_repository as order_repo

# Data layer imports from infrastructure
import infrastructure.database as db
import infrastructure.cache as cache
```

### 2. Feature-Based Organization

```
project/
├── users/
│   ├── user_model.zephyr
│   ├── user_service.zephyr
│   └── user_repository.zephyr
├── orders/
│   ├── order_model.zephyr
│   ├── order_service.zephyr
│   └── order_repository.zephyr
└── products/
    ├── product_model.zephyr
    ├── product_service.zephyr
    └── product_repository.zephyr
```

```zephyr
# Cross-feature imports
import users.user_service as user_service
import orders.order_service as order_service
import products.product_service as product_service

func create_order_for_user(user_id, product_ids) {
    user = user_service.get_user(user_id)
    products = []
    for id in product_ids {
        product = product_service.get_product(id)
        append(products, product)
    }
    return order_service.create_order(user, products)
}
```

### 3. Hexagonal Architecture

```
project/
├── core/           # Domain logic
├── ports/          # Interfaces
├── adapters/       # External integrations
└── config/         # Configuration
```

```zephyr
# Core domain
import ports.user_repository as user_repo_port
import ports.email_service as email_port

# Adapters implement ports
import adapters.postgresql_user_repo as pg_user_repo
import adapters.sendgrid_email as sendgrid_email

# Configuration wires everything together
func configure_services() {
    # Dependency injection
    user_repo = pg_user_repo.create_repository(db_config)
    email_service = sendgrid_email.create_service(api_key)
    
    return {
        "user_repository": user_repo,
        "email_service": email_service
    }
}
```

---

## Testing Patterns

### Test Module Organization

```zephyr
# tests/test_math_utils.zephyr
import "../src/math_utils.zephyr" as math_utils
import "./test_framework.zephyr" as test

func test_add() {
    result = math_utils.add(2, 3)
    test.assert_equals(result, 5, "add(2, 3) should equal 5")
}

func test_multiply() {
    result = math_utils.multiply(4, 5)
    test.assert_equals(result, 20, "multiply(4, 5) should equal 20")
}

func test_circle_area() {
    result = math_utils.circle_area(2)
    expected = math_utils.PI * 4
    test.assert_almost_equals(result, expected, 0.001, "circle_area should be accurate")
}

# Run tests
if __module_name__ == "__main__" {
    test_add()
    test_multiply()
    test_circle_area()
    print("All math_utils tests passed!")
}
```

### Mock Objects and Testing

```zephyr
# tests/test_user_service.zephyr
import "../src/services/user_service.zephyr" as user_service
import "./mocks/mock_database.zephyr" as mock_db
import "./test_framework.zephyr" as test

func test_create_user_success() {
    # Setup mock
    db = mock_db.create_mock()
    mock_db.expect_save_call(db, true)  # Will return true when save is called
    
    # Test
    result = user_service.create_user_with_db("John", "john@example.com", db)
    
    # Assertions
    test.assert_not_none(result, "User should be created")
    test.assert_equals(result.name, "John", "Name should be set correctly")
    test.assert_true(mock_db.was_save_called(db), "Database save should be called")
}
```

---

## Performance Considerations

### Module Loading and Caching

Zephyr automatically caches loaded modules, so multiple imports of the same module are efficient:

```zephyr
# These all reference the same cached module
import math_utils
import math_utils as math
import add, PI from math_utils
```

### Lazy Loading Best Practices

```zephyr
# Good: Import what you need
import specific_function from large_module

# Less optimal: Import entire large module for one function
import large_module  # Only if you need multiple symbols
```

### Circular Import Prevention

Zephyr detects and prevents circular imports:

```zephyr
# module_a.zephyr
import module_b  # This will cause an error if module_b imports module_a

# Better: Use dependency injection or shared interfaces
import shared_interface
```

---

## Migration and Compatibility

### Upgrading Import Patterns

When refactoring existing code:

```zephyr
# Old pattern (if you had a different system)
# from math_utils import add, multiply
# from math_utils import *

# New Zephyr patterns
import add, multiply from math_utils    # Named imports
import math_utils                       # Lazy import

# Gradual migration approach
import math_utils as math  # Start with alias
# Then gradually change math.add() to add() as needed
```

### Version Compatibility

```zephyr
# Use module metadata for version checking
import version_checker
import math_utils

if version_checker.supports_feature(math_utils, "advanced_functions") {
    result = math_utils.advanced_calculation(data)
} else {
    result = math_utils.basic_calculation(data)
}
```

---

## Summary

Zephyr's import system provides:

- **8 flexible import patterns** for different use cases
- **Intuitive syntax** that reads naturally
- **Smart module resolution** with search paths
- **Privacy control** with the `internal` keyword  
- **Direct member access** via dot notation
- **Subdirectory support** for project organization
- **Module caching** for performance
- **Circular import detection** for safety

Choose import patterns based on your specific needs:
- Use **lazy imports** for modules you use extensively
- Use **named imports** for frequently called functions
- Use **string imports** for explicit file paths
- Use **subdirectory imports** for organized projects
- Always use **internal** for implementation details

The system scales from simple scripts to large, complex applications while maintaining readability and maintainability.