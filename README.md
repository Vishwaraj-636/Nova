# Nova

Nova is a compact C++17 interpreter for a small language with variables, arithmetic, strings, booleans, `print`, `if` / `else`, and `while` blocks.

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

## How to check correctness

1. The build command succeeds with no compiler errors.
2. Running `.\nova` prints the demo output in the terminal instead of opening the source file.
3. The `if` branch should print the message for `x < y`.
4. The `while` loop should run until `x` reaches 15.
5. The final output should include the total and `true` for `x == 15`.

If you want to test error handling, try removing a semicolon or using an undefined variable; the interpreter should print a line-numbered error and return a nonzero exit code.
