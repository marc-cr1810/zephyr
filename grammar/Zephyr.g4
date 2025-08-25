grammar Zephyr;

/*
    Zephyr Grammar Definition

    This grammar defines the syntax for Zephyr, a modern scripting language
    with comprehensive features including:

    - Mixed member/index chaining: obj.member[index].property[key]
    - Optional chaining: obj?.member, obj?.method(), obj?[index] for safe access
    - Direct member access for dictionaries: dict.key is equivalent to dict["key"]
    - String multiplication: "text" * count and count * "text"
    - Negative number arithmetic: -5, 10 + -3, (-4) * 2
    - Method calls on indexed objects: obj.list[index].method()
    - Assignment with any chaining pattern: obj.member[index] = value

    Explicit Typing System (Production Ready):
    - Variable type annotations: x : string = "hello"
    - Empty typed declarations: y : int (defaults to none)
    - Typed constants: const pi : float = 3.14159
    - Function parameter typing: func add(a : int, b : int)
    - Function return types: func calculate() : float
    - Lambda parameter typing: (a : int, b : int) -> a + b
    - Class member variable typing: name : string = "Unknown"
    - For-each loop variable typing: for item : string in list
    - Const loop variables: for const item : int in numbers
    - Mixed typing support: Explicit and dynamic types coexist
    - Custom class types: person : Person = Person()
    - Async integration: Full typing in async/await patterns
    - Scope inheritance: Type constraints across all boundaries

    The grammar is designed for recursive-descent parsing and is not left-recursive.
    Updated: December 2024 - Reflects current implementation with comprehensive explicit typing.
*/

program: statement+ EOF;

statement
    : constDeclaration
    | typedDeclaration
    | typedConstDeclaration
    | emptyDeclaration
    | emptyTypedDeclaration
    | expression (ASSIGN expression)?  // Handles both expressions and assignments
    | listDestructuringAssignment
    | functionDefinition
    | asyncFunctionDefinition
    | classDefinition
    | interfaceDefinition
    | returnStatement
    | ifStatement
    | whileStatement
    | forStatement
    | forEachStatement
    | doWhileStatement
    | doUntilStatement
    | loopStatement
    | switchStatement
    | breakStatement
    | continueStatement
    | tryCatchStatement
    ;

tryCatchStatement
    : TRY block CATCH (LPAREN)? NAME (RPAREN)? block
    ;

breakStatement
    : BREAK
    ;

continueStatement
    : CONTINUE
    ;

listDestructuringAssignment
    : LBRACKET (NAME (COMMA NAME)*)? RBRACKET ASSIGN expression
    ;

constDeclaration
    : CONST NAME ASSIGN expression
    ;

typedDeclaration
    : NAME COLON NAME ASSIGN expression
    ;

typedConstDeclaration
    : CONST NAME COLON NAME ASSIGN expression
    ;

emptyDeclaration
    : NAME
    ;

emptyTypedDeclaration
    : NAME COLON NAME
    ;

// memberAssignment is handled within the statement rule, not as a separate rule

doWhileStatement
    : DO block WHILE expression
    ;

doUntilStatement
    : DO block UNTIL expression
    ;

loopStatement
    : LOOP block
    ;

functionDefinition
    : FUNC NAME LPAREN ( typedParameter (COMMA typedParameter)*)? RPAREN (COLON NAME)? block
    ;

typedParameter
    : (CONST)? NAME (COLON NAME)?
    ;

asyncFunctionDefinition
    : ASYNC FUNC NAME LPAREN ( typedParameter (COMMA typedParameter)*)? RPAREN (COLON NAME)? block
    ;

classDefinition
    : CLASS NAME (COLON NAME (COMMA NAME)*)? LBRACE (memberVariableDeclaration | functionDefinition | asyncFunctionDefinition)* RBRACE
    ;

interfaceDefinition
    : INTERFACE NAME LBRACE functionSignature* RBRACE
    ;

functionSignature
    : FUNC NAME LPAREN ( typedParameter (COMMA typedParameter)*)? RPAREN (COLON NAME)?
    ;

memberVariableDeclaration
    : NAME (COLON NAME)? (ASSIGN expression)?
    ;

returnStatement
    : RETURN expression?
    ;

ifStatement
    : IF expression block (ELSE (block | ifStatement))?
    ;

whileStatement
    : WHILE expression block
    ;

forStatement
    : FOR (LPAREN)? (expression)? SEMICOLON expression? SEMICOLON (expression)? (RPAREN)? block
    ;

forEachStatement
    : FOR (LPAREN forEachBody RPAREN | forEachBody) block
    ;

forEachBody
    : forEachHead IN expression (WHERE expression)?
    ;

forEachHead
    : typedForEachVariable
    | LPAREN typedForEachVariable (COMMA typedForEachVariable)* RPAREN
    ;

typedForEachVariable
    : (CONST)? NAME (COLON NAME)?
    ;

block
    : LBRACE statement* RBRACE
    ;



// Expression parsing with operator precedence
// Precedence (highest to lowest):
// 1. Primary expressions (literals, identifiers, parentheses)
// 2. Postfix expressions (member access, indexing, method calls)
// 3. Unary expressions (-, +, !, not)
// 4. Multiplicative (*, /) - supports string multiplication
// 5. Additive (+, -)
// 6. Comparison (<, >, <=, >=)
// 7. Equality (==, !=)
// 8. Logical AND (&&, and)
// 9. Logical OR (||, or)
// 10. Ternary (?:)
expression
    : pipeExpression
    ;

pipeExpression
    : ternaryExpression (PIPE ternaryExpression)*
    ;

ternaryExpression
    : nullishCoalescingExpression (QUESTION expression COLON expression)?
    ;

nullishCoalescingExpression
    : logicalOrExpression (NULLISH_COALESCING logicalOrExpression)*
    ;

logicalOrExpression
    : logicalAndExpression ( (OR | OR_OP) logicalAndExpression )*
    ;

logicalAndExpression
    : bitwiseOrExpression ( (AND | AND_OP) bitwiseOrExpression )*
    ;

bitwiseOrExpression
    : bitwiseXorExpression (BIT_OR bitwiseXorExpression)*
    ;

bitwiseXorExpression
    : bitwiseAndExpression (BIT_XOR bitwiseAndExpression)*
    ;

bitwiseAndExpression
    : equalityExpression (BIT_AND equalityExpression)*
    ;

equalityExpression
    : comparison ( (EQ | NE) comparison )*
    ;

comparison
    : shiftExpression ( (LT | LE | GT | GE) shiftExpression )*
    ;

shiftExpression
    : additiveExpression ( (LSHIFT | RSHIFT) additiveExpression )*
    ;

additiveExpression
    : term ( (PLUS | MINUS) term )*
    ;

// Multiplicative expressions - handles string multiplication
term
    : unary ( (MUL | DIV) unary )*
    ;

// Unary expressions - handles negative number literals and expressions
unary
    : (NOT | NOT_OP | MINUS | PLUS | AWAIT | SPAWN | TILDE) unary
    | factor
    ;

factor
    : NUMBER
    | FLOAT
    | STRING
    | FSTRING
    | TRUE
    | FALSE
    | THIS
    | NAME LPAREN (expression (COMMA expression)*)? RPAREN  // Function Call
    | NAME
    | listLiteral
    | dictionaryLiteral
    | lambdaExpression        // Moved up
    | asyncLambdaExpression   // Moved up
    | LPAREN expression RPAREN // Moved down
    ;

// Note: Advanced chaining (member access and indexing) is implemented
// in the parser through postfix operator loops. The grammar shows the
// basic elements, but chaining like obj.member[index].property is
// handled dynamically in the parser implementation.

dictionaryLiteral
    : LBRACE (expression COLON expression (COMMA expression COLON expression)*)? RBRACE
    ;

lambdaParameters
    : typedParameter                                    // Single parameter: x or const x : type
    | LPAREN RPAREN                                     // No parameters: ()
    | LPAREN ( typedParameter (COMMA typedParameter)*)? RPAREN // Multiple parameters: (x, y)
    ;

lambdaExpression
    : lambdaParameters (COLON NAME)? ARROW (expression | block)
    ;

asyncLambdaExpression
    : ASYNC lambdaParameters (COLON NAME)? ARROW (expression | block)

listLiteral
    : LBRACKET (expression (COMMA expression)*)? RBRACKET
    ;

/*
    Assignment Patterns Supported:

    The language supports sophisticated assignment patterns through the
    statement rule's expression (ASSIGN expression)? structure:

    - Simple assignment: name = expression
    - Member assignment: obj.member = expression
    - Indexed assignment: obj[index] = expression
    - Chained assignment: obj.member[index] = expression
    - Mixed chaining: obj.list[index].property[key] = value
    - Method-based chaining: obj.list[index].method().property = value

    All chaining patterns work seamlessly with assignment operations.
*/

caseStatement
    : CASE expression COLON (block | statement*)
    ;

switchStatement
    : SWITCH expression LBRACE caseStatement* (DEFAULT COLON (block | statement*))? RBRACE

// Tokens
LPAREN: '(';
RPAREN: ')';
LBRACE: '{';
RBRACE: '}';
LBRACKET: '[';
RBRACKET: ']';
COMMA: ',';
SEMICOLON: ';';
ASSIGN: '=';
DOT: '.';
QUESTION_DOT: '?.'; // New: Optional Chaining Operator
PLUS: '+';
MINUS: '-';
MUL: '*';
DIV: '/';
QUESTION: '?';
COLON: ':';
ARROW: '->';

// Bitwise operators
BIT_AND: '&';
BIT_OR: '|';
BIT_XOR: '^';
TILDE: '~';
LSHIFT: '<<';
RSHIFT: '>>';

// Nullish Coalescing Operator
NULLISH_COALESCING: '??';
PIPE: '|>';

// Comparison operators
EQ: '==';
NE: '!=';
LT: '<';
LE: '<=';
GT: '>';
GE: '>=';

// Logical operators
AND: 'and';
OR: 'or';
NOT: 'not';
AND_OP: '&&';
OR_OP: '||';
NOT_OP: '!';

// Keywords
IF: 'if';
ELSE: 'else';
WHILE: 'while';
FOR: 'for';
DO: 'do';
UNTIL: 'until';
LOOP: 'loop';
WHERE: 'where';
IN: 'in';
SWITCH: 'switch';
CASE: 'case';
DEFAULT: 'default';
FUNC: 'func';
RETURN: 'return';
CLASS: 'class';
INTERFACE: 'interface';
THIS: 'this';
TRY: 'try';
CATCH: 'catch';
BREAK: 'break';
CONTINUE: 'continue';
ASYNC: 'async';
AWAIT: 'await';
SPAWN: 'spawn';
TRUE: 'true';
FALSE: 'false';
CONST: 'const';
NONE: 'none';

/*
    Literal Tokens:
    - NAME: Identifiers for variables, functions, classes
    - NUMBER: Integer literals (negative handled by unary operators)
    - FLOAT: Floating-point literals (negative handled by unary operators)
    - STRING: String literals (support multiplication with * operator)
    - FSTRING: F-string literals with embedded expressions

    Async/Await Tokens:
    - ASYNC: Marks functions and lambdas as asynchronous
    - AWAIT: Pauses execution until a Promise resolves
    - SPAWN: Creates concurrent tasks returning Promises
*/
NAME: [a-zA-Z_] [a-zA-Z0-9_]*;
NUMBER: [0-9]+;
FLOAT: [0-9]+ '.' [0-9]+;
STRING: '"' ( '\\"' | ~["\r\n] )* '"';
FSTRING: 'f"' ( '\\{' | '\\}' | '{' ~[}]* '}' | ~["\r\n] )* '"';

/*
    Lexical Rules:
    - WS: Whitespace (spaces, tabs, newlines) - ignored
    - COMMENT: Single-line comments starting with # - ignored

    String Multiplication Support:
    The * operator supports both:
    - "string" * number (handled by StringType::multiply)
    - number * "string" (handled by IntType::multiply)

    Advanced Chaining Support:
    Member access (.) and indexing ([]) can be mixed in any order:
    - obj.member[index].property
    - obj[key].member[index]
    - obj.list[index].method()
    - obj.dict[key][index].submember[key2]
    - Optional chaining (?. and ?[]): Safely access properties, methods, or elements
      that might be 'none'. The chain short-circuits if an intermediate value is 'none'.
      - obj?.member, obj?.method(), obj?[index]
    - Direct member access for dictionaries: dict.key is equivalent to dict["key"].
      This allows for more concise access to dictionary values when the key is a string literal.

    Async/Await Support:
    Asynchronous programming with modern syntax:
    - async func name() { ... } - Async function definitions
    - await expression - Pause execution until Promise resolves
    - spawn function_call() - Create concurrent tasks
    - Promise objects with PENDING/FULFILLED/REJECTED states
    - async x -> expression - Async lambda functions
    - all([promises]) - Promise.all built-in function
*/
WS: [ \t\r\n]+ -> skip;
COMMENT: '#' ~[\r\n]* -> skip;
