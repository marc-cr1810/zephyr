# Zephyr FAQ

## What platforms does Zephyr support?
Zephyr runs anywhere you can build C++17 code (Linux, macOS, Windows).

## How do I declare a constant?
Use `const` before the variable name:  
```zephyr
const pi = 3.14159
```

## How do I run a script?
Build the interpreter and run:  
```bash
./bin/zephyr your_script.zephyr
```

## How do I handle errors?
Use `try`/`catch` blocks:  
```zephyr
try { risky = 10 / 0 } catch e { print("Error:", e) }
```

## Can I use Unicode and emojis?
Yes, Zephyr fully supports Unicode and emojis in strings and identifiers.

## How do I contribute?
Fork the repo, add your changes, and submit a pull request.
