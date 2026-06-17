# C Compiler

A lightweight C compiler that translates a subset of C into Intel x86-64 Assembly. It is designed as a learning tool for understanding compiler construction.

## Features

- **Lexical Analysis**: Custom lexer converts raw source code into logical tokens.
- **Syntax Analysis**: Recursive descent parser generates an Abstract Syntax Tree (AST).
- **Code Generation**: Emits direct x86-64 assembly code for standard POSIX/Windows environments.

### TODO

- **Error Handling**: Provides descriptive line-by-line syntax error reporting.

## Prerequisites (Currently built for Windows)

- Visual Studio
- *OR*
- GCC (to compile the compiler)
- make (build automation)

## Building the Compiler

Clone the repository and build the executable using `make`:

## Usage

Run the compiler on a C source file to generate assembly output:

```bash
./mycc.exe path/to/source.c
```

Or run the compiler by itself to read from the local input file: "input.c".

## Supported Language Subset

Currently, the compiler supports a subset of the C99 standard, including:

- Basic types: `int`
- Control flow: `if`, `else`, including ternary expressions (e1 `?` e2 `:` e3)
- Arithmetic and relational operators `+`, `-`, `*`, `/`, `==`, `<`, `>`, etc.

### Work-In-Progress

Features that are currently being, or will soon be, worked on:

- Basic types: `bool`, `char`, and pointers.
- Control flow: `while` and `for` loops.
- Function definitions and calls (including parameter passing).

## Known Limitations

- Floating-point numbers (`float`, `double`) are not supported.
- Multi-file compilation is not supported; all code must exist in a single .c file.
- Does not support `struct` or `union` types.