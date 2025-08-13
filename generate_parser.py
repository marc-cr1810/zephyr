import re
import os

def to_snake_case(name):
    """Converts camelCase to snake_case."""
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()

def parse_grammar(grammar_content):
    """Parses the grammar file to extract tokens and rules."""
    # Remove comments
    grammar_content = re.sub(r'//.*', '', grammar_content)
    grammar_content = re.sub(r'/\*.*?\*/', '', grammar_content, flags=re.DOTALL)

    tokens = set()
    rules = set()

    # The pattern for a rule is a name starting with a lowercase letter at the beginning of a line, followed by a colon.
    rule_pattern = re.compile(r'^\s*([a-z][a-zA-Z_]*)\s*:', re.MULTILINE)
    for match in rule_pattern.finditer(grammar_content):
        rules.add(to_snake_case(match.group(1)))

    # The pattern for a token is a name starting with an uppercase letter at the beginning of a line, followed by a colon.
    token_pattern = re.compile(r'^\s*([A-Z_]+)\s*:', re.MULTILINE)
    for match in token_pattern.finditer(grammar_content):
        tokens.add(match.group(1))

    return tokens, rules

def analyze_cpp_files(header_path, source_path):
    """Analyzes C++ files to find existing parser methods."""
    declared_methods = set()
    implemented_methods = set()

    # Find declared methods in Parser.h
    with open(header_path, "r") as f:
        header_content = f.read()
        # Regex to find snake_case method names
        declared_matches = re.findall(r"std::unique_ptr<\w+>\s+([a-z_]+)\(.*\)", header_content)
        declared_methods.update(declared_matches)

    # Find implemented methods in Parser.cpp
    with open(source_path, "r") as f:
        source_content = f.read()
        # Regex to find implemented snake_case method names
        implemented_matches = re.findall(r"Parser::([a-z_]+)\(.*\)", source_content)
        implemented_methods.update(implemented_matches)

    return declared_methods, implemented_methods

def main():
    """Main function to drive the analysis and code generation."""
    # Get the directory of the script to build absolute paths
    script_dir = os.path.dirname(os.path.abspath(__file__))

    grammar_path = os.path.join(script_dir, "grammar/Zephyr.g4")
    header_path = os.path.join(script_dir, "include/Parser.h")
    source_path = os.path.join(script_dir, "src/Parser.cpp")

    try:
        with open(grammar_path, "r") as f:
            grammar_content = f.read()
    except FileNotFoundError:
        print(f"Error: {grammar_path} not found.")
        return

    grammar_tokens, grammar_rules = parse_grammar(grammar_content)
    declared_methods, implemented_methods = analyze_cpp_files(header_path, source_path)

    # Find the difference
    missing_declarations = grammar_rules - declared_methods
    missing_implementations = grammar_rules - implemented_methods

    if not missing_declarations and not missing_implementations:
        print("The parser appears to be in sync with the grammar. No missing items found.")
        return

    print("---" + "Analysis Complete" + "---" + "\n")
    print("The following are suggestions for what might be missing from your parser.")
    print("Please review them carefully before adding to your code." + "\n")

    if missing_declarations:
        print("---" + "Missing Method Declarations (for include/Parser.h)" + "---")
        for rule in sorted(list(missing_declarations)):
            # Convert snake_case back to PascalCase for the Node type
            node_type = ''.join(word.capitalize() for word in rule.split('_'))
            print(f"    std::unique_ptr<{node_type}> {rule}();")
        print()

    if missing_implementations:
        print("---" + "Missing Method Implementations (for src/Parser.cpp)" + "---")
        for rule in sorted(list(missing_implementations)):
            node_type = ''.join(word.capitalize() for word in rule.split('_'))
            # The return type in the implementation should match the declaration.
            # We can make a reasonable guess, but it might need to be adjusted (e.g., to Statement, Expression, etc.)
            print(f"std::unique_ptr<{node_type}> Parser::{rule}() {{")
            print(f"    // TODO: Implement parsing logic for {rule}")
            print("    return nullptr;")
            print("}\n")

if __name__ == "__main__":
    main()
