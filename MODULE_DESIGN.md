# Zephyr Module System Design

This document outlines the proposed design for implementing a module system in the Zephyr language.

---

## Core Concepts

The module system is designed to be simple and intuitive, promoting code sharing while providing clear encapsulation. It is based on the following principles:

-   **File-based Modules:** Each `.zephyr` file is a module.
-   **Public by Default:** Symbols (functions, classes, variables) declared at the top level of a module are public and can be imported by other modules by default.
-   **Internal Visibility:** An `internal` keyword is introduced to restrict symbols to the scope of the module in which they are defined. This provides an "opt-out" privacy model.

### The `__main__` Module

The script that begins execution is treated as a special module named `__main__`. This allows a file to be used as both a reusable library and an executable script.

A built-in variable, `__module_name__`, will be available in every module's scope, containing the module's name as a string.

-   For the entry-point script, `__module_name__` will be `"__main__"`.
-   For an imported module (e.g., `import ... from math`), its `__module_name__` will be `"math"`.

This enables the following pattern:

```zephyr
// File: my_utility.zephyr

func useful_function() {
    print("Doing something useful...")
}

// This block only executes when this file is run directly.
if __module_name__ == "__main__" {
    print("Running utility as a standalone script.")
    useful_function()
}
```

---

## 1. Syntax and Keywords

### `internal` Keyword

The `internal` keyword prevents a top-level declaration from being imported by other modules.

**Example:**
```zephyr
// File: math.zephyr

// This constant is public by default and can be imported.
const PI = 3.14159

// This function is also public by default.
func add(a, b) {
    return a + b
}

// This function is private to math.zephyr and cannot be imported.
internal func secret_helper_func() {
    return 42
}
```

### `import` Keyword

The `import` statement is used to bring public symbols from other modules into the current scope. It supports two ways of specifying the module source: by path or by name, and three different import patterns.

-   **Importing from a Path:** Uses a string literal for the path. The `.zephyr` extension is required.
    -   Named imports: `import PI, add from "./math.zephyr"`
    -   Direct star import: `import * from "./math.zephyr"`
    -   Namespace import: `import * as math from "./math.zephyr"`

-   **Importing from a Module Name:** Uses an identifier for the name. The `.zephyr` extension is omitted.
    -   Named imports: `import PI, add from math`
    -   Direct star import: `import * from math`
    -   Namespace import: `import * as math from math`

**Import Patterns:**
-   **Named Import:** `import symbol1, symbol2 from module` - Imports specific symbols by name
-   **Direct Star Import:** `import * from module` - Imports all public symbols directly into current scope
-   **Namespace Import:** `import * as alias from module` - Imports all public symbols into a namespace dictionary

An attempt to import an `internal` symbol will always result in an error.

---

## 2. Implementation Details

### AST (Abstract Syntax Tree)

-   The grammar in `grammar/Zephyr.g4` will be updated to:
    -   Recognize the `internal` keyword as a declaration modifier.
    -   Support the brace-less `import` syntax.
    -   Allow the module source in an `import` statement to be either a string literal (path) or an identifier (name).
-   An `is_internal` boolean flag will be added to all top-level declaration nodes (e.g., `function_definition_t`, `class_definition_t`).
-   The `import_statement_t` AST node will be designed to store the module specifier (e.g., `"./math.zephyr"` or `math`) and a flag to indicate whether it is a path or a name.

### Module Loader

A `module_loader_t` class will be implemented with a multi-step resolution strategy:

1.  **Path-based Resolution:** If the import specifier is a string literal path (e.g., `"./math.zephyr"`), the loader will resolve it relative to the importing file's location.

2.  **Name-based Resolution:** If the import specifier is an identifier (e.g., `math`), the loader will search for a corresponding file (`math.zephyr`) in the following locations, in order:
    a.  The current working directory where the interpreter was launched.
    b.  Directories listed in the `ZEPHYRPATH` environment variable. The interpreter will read this variable on startup, which contains a list of paths separated by `:` (Linux/macOS) or `` (Windows).

3.  **Caching:** Resolved modules will be cached using their absolute file path as the key. This prevents the same module from being loaded and executed multiple times.

4.  **Circular Dependency Detection:** The loader will track the import chain to detect and report circular dependencies.

### Interpreter

The `interpreter_t` will be updated to manage module execution and exports:

1.  When execution begins, the interpreter will establish the entry script or REPL session as the `__main__` module.
2.  Before executing any module's code, the interpreter will inject a `__module_name__` variable into its global scope, with the value being the module's assigned name (`__main__` for the entry point).
3.  When executing a module, the interpreter will maintain an `exports` map for it.
4.  For each top-level declaration in the module's AST:
    -   If the `is_internal` flag is `true`, the symbol is added to the module's local scope only.
    -   If the `is_internal` flag is `false` (the default), the symbol is added to both the local scope and the module's public `exports` map.
5.  When handling an `import` statement, the interpreter will use the `module_loader_t` to get the target module. It will then make the symbols from the module's `exports` map available to the importing scope.
