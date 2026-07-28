# Nova

Nova is a compact C++17 interpreter for a small language with variables, arithmetic, strings, booleans, `print`, `if` / `else`, and `while` blocks.

This is a strong interview project because it demonstrates a full language pipeline: the source text is lexed into tokens, parsed into executable statements, and then interpreted at runtime.

## What the project does

Nova reads a `.nova` source file, processes it line by line, and runs the statements directly. The current language supports:

1. Variable declaration with `let`.
2. Assignment and expression evaluation.
3. Integer and decimal arithmetic.
4. String literals and string concatenation.
5. Boolean literals and comparisons.
6. `print(...)` for runtime output.
7. `if` / `else` blocks.
8. `while` loops.
9. Basic error reporting with line numbers.

## Project structure

1. [include/token.h](include/token.h) defines tokens and runtime values.
2. [src/lexer.cpp](src/lexer.cpp) converts source text into tokens.
3. [include/interpreter.h](include/interpreter.h) declares the runtime evaluator.
4. [src/interpreter.cpp](src/interpreter.cpp) executes statements and expressions.
5. [src/main.cpp](src/main.cpp) loads the file and starts the interpreter.

## Build

From the repository root:

```powershell
g++ -std=c++17 -I include src/main.cpp src/lexer.cpp src/interpreter.cpp -o nova
```

## Run

Run the bundled demo program:

```powershell
.\nova
```

Run your own Nova file:

```powershell
.\nova path\to\program.nova
```

## What happens when you run it

The interpreter reads the source file, tokenizes it, evaluates the statements, and prints runtime output directly in the terminal. The demo shows variable declarations, arithmetic, a conditional branch, and a loop.

Expected output is similar to this:

```text
Nova runtime demo
10
20
30
30
x is smaller than y
60
true
```

## How to check correctness

1. The build command succeeds with no compiler errors.
2. Running `.\nova` prints the demo output in the terminal instead of opening the source file.
3. The `if` branch should print the message for `x < y`.
4. The `while` loop should run until `x` reaches 15.
5. The final output should include the total and `true` for `x == 15`.

If you want to test error handling, try removing a semicolon or using an undefined variable; the interpreter should print a line-numbered error and return a nonzero exit code.

## How to explain it in an interview

Use this structure when asked about the project:

1. Start with the goal: you built a small interpreter in C++17 for a custom language.
2. Explain the pipeline: source text is tokenized, parsed, and then executed.
3. Mention the main features: variables, expressions, `print`, `if` / `else`, and `while`.
4. Point out the design choice: each component has a single job, which keeps the project maintainable.
5. Mention the current limitation honestly: this is an interpreter, not a full optimizing compiler.

## Good talking points

1. Lexing rules for keywords, identifiers, numbers, strings, operators, and comments.
2. Runtime evaluation using a simple environment for variables.
3. Error handling with line-numbered messages.
4. Extensibility toward functions, arrays, or a bytecode backend later.

## Suggested next improvements

1. Add a test suite for lexer and interpreter behavior.
2. Add function declarations and function calls.
3. Add a proper AST and separate parser stage if you want a stronger compiler story.
4. Add better error recovery and clearer runtime diagnostics.
