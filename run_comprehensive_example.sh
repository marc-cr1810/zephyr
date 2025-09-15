#!/bin/bash

# Comprehensive Embedded Zephyr Example Runner
# This script builds and runs the comprehensive C++ example

set -e  # Exit on any error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== Zephyr Comprehensive Example Runner ==="
echo "Building and running comprehensive embedded C++ example"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if build directory exists
if [ ! -d "build" ]; then
    print_status "Creating build directory..."
    mkdir -p build
fi

# Configure CMake with embedding examples enabled
print_status "Configuring CMake build system..."
cmake -S . -B build -DBUILD_EMBEDDING_EXAMPLE=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=17

if [ $? -eq 0 ]; then
    print_success "CMake configuration completed"
else
    print_error "CMake configuration failed"
    exit 1
fi

# Build the comprehensive example
print_status "Building comprehensive embedded example..."
cmake --build build --target comprehensive_embedded_example -j$(nproc 2>/dev/null || echo 4)

if [ $? -eq 0 ]; then
    print_success "Build completed successfully"
else
    print_error "Build failed"
    exit 1
fi

# Check if the executable was created
if [ ! -f "bin/comprehensive_embedded_example" ]; then
    print_error "Executable not found at bin/comprehensive_embedded_example"
    exit 1
fi

# Also build main zephyr executable for script testing
print_status "Building main Zephyr executable..."
cmake --build build --target zephyr -j$(nproc 2>/dev/null || echo 4)

echo ""
echo "=== Available Options ==="
echo "1. Run C++ embedded example"
echo "2. Run Zephyr script demo"
echo "3. Run both examples"
echo "4. Exit"
echo ""

# Interactive menu
read -p "Please select an option (1-4): " choice

case $choice in
    1)
        echo ""
        print_status "Running C++ embedded example..."
        echo "=========================================="
        ./bin/comprehensive_embedded_example
        echo "=========================================="
        print_success "C++ example completed"
        ;;
    2)
        echo ""
        print_status "Running Zephyr script demo..."
        echo "=========================================="
        ./bin/zephyr examples/comprehensive_embedded_demo.zephyr
        echo "=========================================="
        print_success "Zephyr script demo completed"
        ;;
    3)
        echo ""
        print_status "Running C++ embedded example first..."
        echo "=========================================="
        ./bin/comprehensive_embedded_example
        echo "=========================================="
        print_success "C++ example completed"

        echo ""
        print_status "Now running Zephyr script demo..."
        echo "=========================================="
        ./bin/zephyr examples/comprehensive_embedded_demo.zephyr
        echo "=========================================="
        print_success "Both examples completed"
        ;;
    4)
        print_status "Exiting..."
        exit 0
        ;;
    *)
        print_warning "Invalid option. Running C++ example by default..."
        echo ""
        print_status "Running C++ embedded example..."
        echo "=========================================="
        ./bin/comprehensive_embedded_example
        echo "=========================================="
        print_success "C++ example completed"
        ;;
esac

echo ""
echo "=== Example Information ==="
echo "• C++ Source: examples/comprehensive_embedded_example.cpp"
echo "• Zephyr Script: examples/comprehensive_embedded_demo.zephyr"
echo "• Documentation: examples/COMPREHENSIVE_EXAMPLE_README.md"
echo "• Executable: bin/comprehensive_embedded_example"
echo ""
echo "=== Manual Build Commands ==="
echo "mkdir build && cd build"
echo "cmake .. -DBUILD_EMBEDDING_EXAMPLE=ON"
echo "make comprehensive_embedded_example"
echo ""
echo "=== Manual Run Commands ==="
echo "./bin/comprehensive_embedded_example          # C++ embedded version"
echo "./bin/zephyr examples/comprehensive_embedded_demo.zephyr  # Script version"
echo ""

print_success "Comprehensive example session completed!"
