#!/bin/bash

# Test script to verify all Zephyr examples run successfully
# This script runs all example files and reports any failures

echo "=========================================="
echo "  Zephyr Examples Test Suite"
echo "=========================================="
echo ""

# Change to the zephyr_lang directory
cd "$(dirname "$0")"

# Make sure the project is built
echo "Building Zephyr..."
make > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi
echo "✅ Build successful"
echo ""

# Array of example files to test
examples=(
    "01_basic_features.zephyr"
    "02_data_structures.zephyr"
    "03_functions_and_lambdas.zephyr"
    "04_classes_and_objects.zephyr"
    "05_advanced_indexing.zephyr"
    "06_advanced_classes.zephyr"
    "07_debugging_and_edge_cases.zephyr"
    "08_comprehensive_examples.zephyr"
    "09_string_operations.zephyr"
    "10_compound_assignment.zephyr"
    "11_compound_assignment_comprehensive.zephyr"
    "12_lambda_dictionaries_nested.zephyr"
    "13_async_await_basic.zephyr"
    "14_async_parallel.zephyr"
    "15_async_promise_all.zephyr"
    "16_async_comprehensive_test.zephyr"
    "17_enhanced_loops.zephyr"
    "18_optional_chaining.zephyr"
    "19_async_class_methods.zephyr"
    "20_explicit_typing.zephyr"
    "21_exit_function.zephyr"
    "22_interfaces.zephyr"
    "23_built_in_functions.zephyr"
    "24_explicit_typing_and_init_restrictions.zephyr"
    "25_bitwise_operations.zephyr"
)

# Counters
total_tests=${#examples[@]}
passed_tests=0
failed_tests=0

echo "Running $total_tests example tests..."
echo ""

# Test each example
for example in "${examples[@]}"; do
    example_path="examples/$example"
    echo -n "Testing $example... "

    # Run the example and capture output
    if ./bin/zephyr "$example_path" > /dev/null 2>&1; then
        echo "✅ PASSED"
        ((passed_tests++))
    else
        echo "❌ FAILED"
        echo "   Error running: ./bin/zephyr $example_path"
        ((failed_tests++))
    fi
done

echo ""
echo "=========================================="
echo "  Test Results Summary"
echo "=========================================="
echo "Total tests:  $total_tests"
echo "Passed:       $passed_tests"
echo "Failed:       $failed_tests"
echo ""

if [ $failed_tests -eq 0 ]; then
    echo "🎉 All tests passed! Zephyr examples are working correctly."
    exit 0
else
    echo "⚠️  Some tests failed. Please check the failing examples."
    exit 1
fi
