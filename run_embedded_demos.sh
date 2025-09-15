#!/bin/bash

# Embedded Zephyr C++ Examples Demo Script
# This script builds and runs all working embedded examples

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print colored output
print_header() {
    echo -e "${CYAN}================================================================${NC}"
    echo -e "${CYAN} $1${NC}"
    echo -e "${CYAN}================================================================${NC}"
    echo
}

print_section() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Main demo function
main() {
    print_header "Zephyr Embedded C++ Examples Demo"

    echo -e "This demo showcases comprehensive C++ integration with Zephyr:"
    echo -e "• Object-oriented programming with classes and inheritance"
    echo -e "• Template functions and generic programming"
    echo -e "• STL containers and algorithms"
    echo -e "• Native module system with function exports"
    echo -e "• Mathematical computations and constants"
    echo -e "• Real-time system simulation"
    echo -e "• Memory management with smart pointers"
    echo -e "• Error handling and type safety"
    echo

    # Check if build directory exists
    if [ ! -d "build" ]; then
        print_info "Creating build directory..."
        mkdir -p build
    fi

    # Configure and build examples
    print_section "Building Examples"
    print_info "Configuring CMake with embedding examples enabled..."
    cmake -S . -B build -DBUILD_EMBEDDING_EXAMPLE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17

    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        exit 1
    fi

    print_info "Building examples..."
    cmake --build build --target minimal_embedded_example comprehensive_embedded_example -j$(nproc 2>/dev/null || echo 4)

    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi

    echo

    # Run minimal example
    print_section "Running Minimal Embedded Example"
    echo -e "This example demonstrates basic C++ class integration:"
    echo -e "• Calculator class with mathematical operations"
    echo -e "• Native module registration and exports"
    echo -e "• Function calls from Zephyr scripts"
    echo -e "• Mathematical constants"
    echo

    if [ -f "bin/minimal_embedded_example" ]; then
        print_info "Executing minimal_embedded_example..."
        echo -e "${PURPLE}▼ Output:${NC}"
        ./bin/minimal_embedded_example
        echo -e "${PURPLE}▲ End Output${NC}"
        echo
        print_success "Minimal example completed successfully"
    else
        print_error "minimal_embedded_example executable not found"
        exit 1
    fi

    echo

    # Information about comprehensive example
    print_section "Comprehensive Example Information"
    echo -e "The comprehensive example demonstrates advanced features:"
    echo -e "• Game entity system with inheritance"
    echo -e "• Template-based factory methods"
    echo -e "• STL containers (vectors, maps) and algorithms"
    echo -e "• Real-time game loop simulation"
    echo -e "• Statistical analysis and mathematical utilities"
    echo -e "• Complex native module interactions"
    echo
    echo -e "${YELLOW}Note: The comprehensive example has module import caching issues${NC}"
    echo -e "${YELLOW}that prevent multiple test runs in the same session. This is a${NC}"
    echo -e "${YELLOW}known limitation that doesn't affect the core functionality.${NC}"
    echo

    # Show available executables
    print_section "Available Examples"
    echo -e "The following examples have been built and are ready to run:"
    echo

    if [ -f "bin/minimal_embedded_example" ]; then
        echo -e "${GREEN}✅ bin/minimal_embedded_example${NC} - Basic C++ integration (working)"
        echo -e "   Demonstrates: Classes, functions, native modules, type conversion"
    fi

    if [ -f "bin/comprehensive_embedded_example" ]; then
        echo -e "${YELLOW}⚠️  bin/comprehensive_embedded_example${NC} - Advanced features (partial)"
        echo -e "   Demonstrates: Game systems, templates, STL, complex algorithms"
    fi

    if [ -f "bin/zephyr" ]; then
        echo -e "${GREEN}✅ bin/zephyr${NC} - Main interpreter"
        echo -e "   Can run Zephyr scripts: ./bin/zephyr script.zephyr"
    fi

    echo

    # Show source files
    print_section "Source Code"
    echo -e "Example source files for study and modification:"
    echo
    echo -e "${CYAN}📁 C++ Examples:${NC}"
    echo -e "• examples/minimal_embedded_example.cpp - Working basic integration"
    echo -e "• examples/comprehensive_embedded_example.cpp - Advanced features"
    echo
    echo -e "${CYAN}📁 Zephyr Scripts:${NC}"
    echo -e "• examples/comprehensive_embedded_demo.zephyr - Advanced script demo"
    echo
    echo -e "${CYAN}📁 Documentation:${NC}"
    echo -e "• examples/COMPREHENSIVE_EXAMPLE_README.md - Detailed documentation"
    echo -e "• README.md - Project overview"
    echo

    # Usage examples
    print_section "Usage Examples"
    echo -e "Manual execution commands:"
    echo
    echo -e "${CYAN}# Run minimal working example:${NC}"
    echo -e "./bin/minimal_embedded_example"
    echo
    echo -e "${CYAN}# Build specific target:${NC}"
    echo -e "cmake --build build --target minimal_embedded_example"
    echo
    echo -e "${CYAN}# Run Zephyr script:${NC}"
    echo -e "./bin/zephyr examples/comprehensive_embedded_demo.zephyr"
    echo
    echo -e "${CYAN}# Clean and rebuild:${NC}"
    echo -e "rm -rf build && make examples"
    echo

    # Learning objectives
    print_section "Key Learning Points"
    echo -e "After studying these examples, you'll understand:"
    echo
    echo -e "${GREEN}🎯 C++ Integration:${NC}"
    echo -e "   • How to embed Zephyr in C++ applications"
    echo -e "   • Native module creation and registration"
    echo -e "   • Automatic type conversion between C++ and Zephyr"
    echo
    echo -e "${GREEN}🎯 Object-Oriented Design:${NC}"
    echo -e "   • Class hierarchies and inheritance patterns"
    echo -e "   • Polymorphism and virtual functions"
    echo -e "   • Template programming techniques"
    echo
    echo -e "${GREEN}🎯 System Architecture:${NC}"
    echo -e "   • Module system design patterns"
    echo -e "   • Memory management with RAII"
    echo -e "   • Error handling strategies"
    echo
    echo -e "${GREEN}🎯 Practical Applications:${NC}"
    echo -e "   • Game engine development"
    echo -e "   • Mathematical computing libraries"
    echo -e "   • Real-time system simulation"
    echo

    print_header "Demo Complete!"
    echo -e "The minimal embedded example demonstrates a fully working"
    echo -e "integration of C++ classes, functions, and variables with Zephyr."
    echo
    echo -e "The comprehensive example showcases advanced features and"
    echo -e "design patterns, though it has some module import limitations."
    echo
    echo -e "Both examples serve as excellent starting points for building"
    echo -e "your own embedded Zephyr applications with C++!"
    echo
    print_success "All examples are ready for exploration and modification"
}

# Execute main function
main "$@"
