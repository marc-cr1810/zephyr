# Getting Started with Zephyr

Welcome to Zephyr, a modern, dynamically-typed scripting language implemented in C++. This guide will help you install, build, and run Zephyr scripts, and introduce you to the language's philosophy and learning resources.

---

## What is Zephyr?

Zephyr is a concise, expressive scripting language designed for rapid development, prototyping, and teaching modern programming concepts. It features dynamic typing, optional explicit typing, functional and object-oriented paradigms, advanced data structures, and robust error handling.

---

## Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/your-org/zephyr.git
   cd zephyr
   ```

2. **Build the interpreter:**
   ```bash
   make
   ```
   The executable will be created at `build/zephyr`.

   > **Note:** Zephyr requires a C++17-compatible compiler and CMake. If you encounter build issues, ensure your toolchain is up to date.

---

## Running Zephyr Scripts

To run a Zephyr script, use the interpreter and provide the path to your `.zephyr` file:

```bash
./bin/zephyr path/to/script.zephyr
```

**Example:**
```bash
./bin/zephyr examples/01_basic_features.zephyr
```

You can run any example or your own script this way.

---

## Exploring the Examples

The `examples/` directory contains comprehensive scripts demonstrating all language features, from basic syntax to advanced async programming and error handling.

**Recommended Learning Path:**
1. Start with `01_basic_features.zephyr` for syntax, variables, operators, and control flow.
2. Progress through the numbered examples to learn data structures, functions, classes, lambdas, async/await, and more.
3. Use `08_comprehensive_examples.zephyr` and others for real-world scenarios and integration patterns.

Each example file is documented in detail in [docs/examples/](examples/).

---

## Editing and Creating Zephyr Scripts

- Zephyr scripts use the `.zephyr` file extension.
- You can edit scripts with any text editor.
- Scripts are plain text and do not require compilation—just run them with the interpreter.

**Hello World Example:**
```zephyr
print("Hello, Zephyr!")
```

Save this as `hello.zephyr` and run:
```bash
./bin/zephyr hello.zephyr
```

---

## Language Philosophy

Zephyr is designed for:

- **Clarity:** Readable, concise syntax.
- **Flexibility:** Dynamic typing with optional explicit types.
- **Safety:** Robust error handling and type validation.
- **Modern Features:** Functional programming, object-oriented design, async/await, advanced data access.

---

## Documentation and Resources

- [docs/language_overview.md](language_overview.md): Feature summary and syntax highlights.
- [docs/examples/](examples/): Detailed, categorized example scripts and explanations.
- [README.md](../../examples/README.md): Example organization and learning path.
- [ZEPHYR.md](../../ZEPHYR.md): Full language specification and advanced features.

---

## Troubleshooting

- If you encounter build errors, check your compiler and CMake versions.
- For interpreter errors, review your script for syntax issues or type mismatches.
- Use the example scripts as references for correct syntax and patterns.

---

## Contributing

- Fork the repository and submit pull requests for new features, bug fixes, or documentation improvements.
- When adding new examples, follow the numbering and documentation conventions in `examples/README.md`.
- Test all examples before submitting.

---

## Community & Support

- For questions, open an issue on GitHub.
- Share your Zephyr scripts and feedback to help improve the language.

---

## Next Steps

- Read [docs/language_overview.md](language_overview.md) for a summary of Zephyr's features.
- See [docs/examples/](examples/) for categorized example scripts and explanations.
- Try editing and running your own scripts to explore Zephyr's capabilities.
