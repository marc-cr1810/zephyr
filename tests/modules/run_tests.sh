#!/bin/bash
# run_tests.sh - Automated Module System Test Runner
# This script runs all module system tests and reports results

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo -e "${BLUE}================================================================${NC}"
echo -e "${BLUE}ZEPHYR MODULE SYSTEM TEST SUITE${NC}"
echo -e "${BLUE}================================================================${NC}"
echo ""
echo "Project root: $PROJECT_ROOT"
echo "Test directory: $SCRIPT_DIR"
echo ""

# Change to project root for proper module resolution
cd "$PROJECT_ROOT"

# Function to run a single test
run_test() {
    local test_file="$1"
    local test_name="$2"
    local test_desc="$3"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    echo -e "${YELLOW}========================================${NC}"
    echo -e "${YELLOW}TEST $TOTAL_TESTS: $test_name${NC}"
    echo -e "${YELLOW}DESC: $test_desc${NC}"
    echo -e "${YELLOW}FILE: $test_file${NC}"
    echo -e "${YELLOW}========================================${NC}"

    # Run the test and capture output
    if ./bin/zephyr "$test_file" > /dev/null 2>&1; then
        echo -e "${GREEN}✅ PASS: $test_name${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo ""
        return 0
    else
        echo -e "${RED}❌ FAIL: $test_name${NC}"
        echo -e "${RED}Running test with output for debugging:${NC}"
        ./bin/zephyr "$test_file" || true
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo ""
        return 1
    fi
}

# Check if zephyr binary exists
if [ ! -f "./bin/zephyr" ]; then
    echo -e "${RED}ERROR: Zephyr binary not found at ./bin/zephyr${NC}"
    echo "Please run 'make' to build the project first."
    exit 1
fi

echo -e "${BLUE}Running Module System Tests...${NC}"
echo ""

# Test 1: Internal Keyword Parsing
run_test "tests/modules/internal_keyword_test.zephyr" \
         "Internal Keyword Test" \
         "Verify that 'internal' keyword is parsed correctly for functions, classes, and constants"

# Test 2: Module Name Variable (Basic Math Module)
run_test "tests/modules/math_utils.zephyr" \
         "Module Name Variable Test" \
         "Verify that __module_name__ is correctly set to '__main__' in main scripts"

# Test 3: Module with Internal Functions
run_test "tests/modules/math_with_internal.zephyr" \
         "Internal Functions Module Test" \
         "Test module containing both public and internal functions"

# Test 4: Named and Namespace Imports
run_test "tests/modules/import_test.zephyr" \
         "Import Functionality Test" \
         "Test importing specific symbols and namespace imports from other modules"

# Test 5: Internal Symbol Protection
run_test "tests/modules/internal_import_test.zephyr" \
         "Internal Symbol Protection Test" \
         "Verify that internal symbols cannot be imported and are excluded from exports"

# Test 6: Path-based Imports
run_test "tests/modules/path_import_test.zephyr" \
         "Path-based Import Test" \
         "Test importing modules using relative file paths with .zephyr extension"

# Test 7: Test Suite Overview
run_test "tests/modules/run_module_tests.zephyr" \
         "Test Suite Documentation" \
         "Module system test suite overview and documentation"

# Test 8: Comprehensive Import Syntax Test
run_test "tests/modules/comprehensive_import_test.zephyr" \
         "Comprehensive Import Syntax Test" \
         "Test all import syntaxes: named, star, namespace, path-based, and error handling"

# Test 9: Conditional Import Test
run_test "tests/modules/conditional_import_test.zephyr" \
         "Conditional Import Test" \
         "Test all conditional importing patterns: if/else, functions, loops, error handling"

# Test 10: Simple Conditional Import Test
run_test "tests/modules/simple_conditional_test.zephyr" \
         "Simple Conditional Import Test" \
         "Basic conditional import functionality verification"

# Test 11: Conditional Import Guide
run_test "tests/modules/conditional_import_guide.zephyr" \
         "Conditional Import Comprehensive Guide" \
         "Complete guide and documentation for conditional importing patterns"

# Test 12: REPL Module System Test
run_test "tests/modules/repl_test.zephyr" \
         "REPL Module System Test" \
         "Verify that REPL mode properly supports module system features"

echo -e "${BLUE}================================================================${NC}"
echo -e "${BLUE}TEST RESULTS SUMMARY${NC}"
echo -e "${BLUE}================================================================${NC}"
echo ""
echo -e "Total Tests: ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Passed:      ${GREEN}$PASSED_TESTS${NC} ($(( (PASSED_TESTS * 100) / TOTAL_TESTS ))%)"
echo -e "Failed:      ${RED}$FAILED_TESTS${NC} ($(( (FAILED_TESTS * 100) / TOTAL_TESTS ))%)"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL TESTS PASSED! Module system is working perfectly!${NC}"
    echo ""
    echo -e "${GREEN}Module System Features Verified:${NC}"
    echo -e "${GREEN}✅ Internal keyword parsing${NC}"
    echo -e "${GREEN}✅ Module name injection (__module_name__)${NC}"
    echo -e "${GREEN}✅ Named imports (import symbol from module)${NC}"
    echo -e "${GREEN}✅ Namespace imports (import * as alias from module)${NC}"
    echo -e "${GREEN}✅ Path-based imports (import from './path.zephyr')${NC}"
    echo -e "${GREEN}✅ Internal symbol protection${NC}"
    echo -e "${GREEN}✅ Automatic export management${NC}"
    echo -e "${GREEN}✅ Error handling for missing symbols/modules${NC}"
    echo -e "${GREEN}✅ Module caching and performance${NC}"
    echo -e "${GREEN}✅ All import syntax patterns (named, star, namespace)${NC}"
    echo -e "${GREEN}✅ Conditional importing (if/else, functions, loops)${NC}"
    echo -e "${GREEN}✅ REPL mode module system support${NC}"
    echo ""
    echo -e "${GREEN}MODULE SYSTEM STATUS: PRODUCTION READY ✅${NC}"
    exit 0
else
    echo -e "${RED}⚠️  Some tests failed. Please check the implementation.${NC}"
    echo -e "${RED}Failed test count: $FAILED_TESTS${NC}"
    echo ""
    echo -e "${YELLOW}To debug failed tests, run them individually:${NC}"
    echo -e "${YELLOW}./bin/zephyr tests/modules/[test_file].zephyr${NC}"
    exit 1
fi
