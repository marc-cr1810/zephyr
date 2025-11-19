# Zephyr Documentation

Welcome to the comprehensive documentation for the Zephyr programming language and its C++ API. This documentation covers everything from basic language usage to advanced plugin development.

## 📚 Documentation Structure

### Core Language Documentation

- **[Language Overview](language_overview.md)** - Introduction to Zephyr syntax and features
- **[Getting Started](getting_started.md)** - Basic tutorial for the Zephyr language
- **[Exception Handling](examples/25_exception_handling.md)** - Comprehensive error handling with try-catch-finally
- **[Function Overloading](FUNCTION_OVERLOADING.md)** - Advanced function overloading features
- **[Style Guide](style_guide.md)** - Coding conventions and best practices
- **[FAQ](faq.md)** - Frequently asked questions

### C++ API Documentation

#### 🚀 Quick Start
- **[C++ API Overview](cpp_api_overview.md)** - Architecture, features, and design philosophy
- **[Getting Started with C++ API](getting_started_cpp.md)** - Step-by-step tutorial and examples
- **[C++ API User Guide](cpp_api_guide.md)** - Comprehensive usage guide with examples

#### 📖 Reference Documentation
- **[C++ API Reference](cpp_api_reference.md)** - Complete API reference with all classes and functions
- **[Plugin Development Guide](plugin_development.md)** - Creating dynamic plugins for Zephyr
- **[Plugin API Reference](plugin_api_reference.md)** - Detailed plugin API documentation

#### 🔧 Implementation Details
- **[Plugin System Implementation](plugin_system_implementation.md)** - Technical implementation status and architecture

### Community
- **[Contributing](contributing.md)** - How to contribute to the Zephyr project

## 🎯 Quick Navigation

### For New Users
If you're new to Zephyr, start here:
1. [Language Overview](language_overview.md) - Learn the Zephyr language basics
2. [Getting Started](getting_started.md) - Write your first Zephyr program

### For C++ Developers
If you want to embed Zephyr or create plugins:
1. [C++ API Overview](cpp_api_overview.md) - Understand the architecture and capabilities
2. [Getting Started with C++ API](getting_started_cpp.md) - Build your first embedded application
3. [C++ API User Guide](cpp_api_guide.md) - Master all the features

### For Plugin Developers
If you want to create dynamic plugins:
1. [Plugin Development Guide](plugin_development.md) - Learn plugin architecture and development
2. [Plugin API Reference](plugin_api_reference.md) - Complete plugin API documentation

### For Contributors
If you want to contribute to Zephyr:
1. [Contributing](contributing.md) - Contribution guidelines and process
2. [Plugin System Implementation](plugin_system_implementation.md) - Implementation details

## 🌟 Key Features

### Zephyr Language
- **Modern Syntax** - Clean, readable syntax inspired by Python and JavaScript
- **Dynamic Typing** - Flexible type system with optional type hints
- **Exception Handling** - Comprehensive try-catch-finally with typed exception catching
- **Async/Await** - Built-in support for asynchronous programming
- **Object-Oriented** - Classes, inheritance, and interfaces
- **Functional Programming** - First-class functions and lambda expressions

### C++ API
- **Easy Embedding** - Simple one-line integration into C++ applications
- **Type Safety** - Automatic type conversion between C++ and Zephyr
- **Plugin System** - Dynamic loading of C++ shared libraries
- **Memory Safety** - Smart pointer architecture prevents leaks
- **Professional Quality** - Industry-standard namespace structure and design

## 📖 Documentation Sections

### Language Features

| Topic | Description | Document |
|-------|-------------|----------|
| **Basic Syntax** | Variables, functions, control flow | [Language Overview](language_overview.md) |
| **Type System** | Dynamic typing, type hints, conversions | [Language Overview](language_overview.md) |
| **Exception Handling** | Try-catch-finally, typed exceptions, error recovery | [Exception Handling](examples/25_exception_handling.md) |
| **Object-Oriented** | Classes, inheritance, polymorphism | [Language Overview](language_overview.md) |
| **Async Programming** | Promises, async/await, concurrency | [Language Overview](language_overview.md) |
| **Function Overloading** | Advanced function overloading system | [Function Overloading](FUNCTION_OVERLOADING.md) |

### C++ Integration

| Topic | Description | Document |
|-------|-------------|----------|
| **Engine API** | Core engine for script execution | [C++ API Reference](cpp_api_reference.md) |
| **Type Conversion** | Automatic C++ ↔ Zephyr type conversion | [Getting Started](getting_started_cpp.md) |
| **Function Registration** | Register C++ functions for Zephyr | [C++ API User Guide](cpp_api_guide.md) |
| **Error Handling** | Robust error propagation and handling | [C++ API Reference](cpp_api_reference.md) |
| **Performance** | Optimization tips and characteristics | [C++ API Overview](cpp_api_overview.md) |

### Plugin Development

| Topic | Description | Document |
|-------|-------------|----------|
| **Plugin Architecture** | Plugin system design and lifecycle | [Plugin Development](plugin_development.md) |
| **Module Creation** | Creating native modules with functions | [Plugin Development](plugin_development.md) |
| **Class Bindings** | Exposing C++ classes to Zephyr | [Plugin Development](plugin_development.md) |
| **Building Plugins** | CMake setup and compilation | [Plugin Development](plugin_development.md) |
| **Best Practices** | Guidelines for professional plugins | [Plugin Development](plugin_development.md) |

## 🔗 Quick Links

### Essential References
- [C++ API Overview](cpp_api_overview.md) - **Start here for C++ integration**
- [Getting Started with C++ API](getting_started_cpp.md) - **Step-by-step tutorial**
- [Plugin Development Guide](plugin_development.md) - **Create dynamic plugins**
- [C++ API Reference](cpp_api_reference.md) - **Complete API documentation**

### Code Examples
- **Basic Embedding**: See [Getting Started with C++ API](getting_started_cpp.md)
- **Advanced Features**: See [C++ API User Guide](cpp_api_guide.md)
- **Plugin Creation**: See [Plugin Development Guide](plugin_development.md)
- **Real-World Usage**: Check the `examples/` directory

### External Resources
- **GitHub Repository**: [https://github.com/zephyr/zephyr](https://github.com/zephyr/zephyr)
- **Issue Tracker**: Report bugs and request features
- **Community**: Join discussions and get help

## 💡 Tips for Using This Documentation

### For Beginners
1. **Start Small** - Begin with [Getting Started](getting_started.md) for the language or [Getting Started with C++ API](getting_started_cpp.md) for C++ integration
2. **Try Examples** - Run the provided examples to see concepts in action
3. **Ask Questions** - Use the FAQ and community resources

### For Experienced Developers
1. **Jump to Reference** - Use [C++ API Reference](cpp_api_reference.md) for quick lookups
2. **Check Implementation** - See [Plugin System Implementation](plugin_system_implementation.md) for technical details
3. **Contribute** - Help improve documentation and examples

### For Plugin Developers
1. **Understand Architecture** - Read the plugin system overview first
2. **Follow Examples** - Study working plugin examples
3. **Use Best Practices** - Follow the recommended patterns and guidelines

## 📝 Documentation Standards

This documentation follows these standards:
- **Clear Examples** - Every concept includes working code examples
- **Complete Coverage** - All public APIs are documented
- **Cross-References** - Links between related topics
- **Regular Updates** - Documentation stays current with code changes

## 🤝 Contributing to Documentation

Help us improve this documentation:
- **Report Issues** - Found something unclear or incorrect?
- **Suggest Improvements** - Have ideas for better explanations?
- **Add Examples** - Share your real-world usage patterns
- **Fix Typos** - Every improvement helps

See [Contributing](contributing.md) for detailed guidelines.

---

**Ready to get started?** Choose your path:
- 🎯 [Learn Zephyr Language](language_overview.md)
- 🚀 [Embed in C++ Application](getting_started_cpp.md)  
- 🔌 [Create Dynamic Plugins](plugin_development.md)
- 📚 [Browse Complete API Reference](cpp_api_reference.md)

**Questions?** Check the [FAQ](faq.md) or browse [all documentation](#-documentation-sections) above.