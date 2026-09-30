# Examples

Small-C programs that `cc86` compiles to 8088 MASM assembly.

## Building

Build the compiler first, then the examples:

    make -C ..
    make

Each `.c` file produces a `.mac` file of MASM assembly.

To compile one by hand, run the compiler **from the parent
directory**:

    cd .. && ./cc86 examples/hello.c

That matters because Small-C resolves `#include` against the current
working directory and has no search path, so `stdio.h` is only found
from the repository root. Running `../cc86 hello.c` from inside this
directory fails for that reason, and the Makefile above works around it
by changing directory first.

To get a running program you still need MS-DOS:

    masm hello.mac;;
    link hello        # give the linker clib.lib when it asks

`prolog.h` and `epilog.h` from the parent directory must be present
when MASM runs, because the emitted assembly includes them.

## The examples

| File | Shows |
| --- | --- |
| `hello.c` | Global strings, `while`, `putchar()`, `printf()` |
| `fib.c` | Recursion and `for` loops |
| `sort.c` | Global array initialisers, nested loops, subscripting |
| `strings.c` | Char pointers and the `str*` library |
| `switch.c` | `switch`/`case`/`default`, compiled via `CCSWITCH` |
| `pointers.c` | Pointer arithmetic, and how int pointers are scaled |
| `asm.c` | `#asm` blocks and the calling convention |
| `fileio.c` | `fopen()`, `fgets()`, `fprintf()`, `fclose()` |
| `c99.c` | C99 function definitions and prototypes |

## Style

These files follow `STYLE.md` like the rest of the repo, and
`make checkstyle` in the parent directory covers them.

The eight original examples keep K&R definitions on purpose, to show
the language as Small-C first defined it. That is the one deviation
from `STYLE.md`, and the parameter types sit between the argument list
and the opening brace:

    show(label) char *label;
    {
            ...
    }

`clang-format` handles that shape correctly, so the brace still lands
on its own line and the function name still starts at column zero.
`c99.c` needs no exception at all, and follows `STYLE.md` in full.

In `asm.c` the `#asm` blocks are fenced with `clang-format off`, so the
8088 assembly inside them is left exactly as written. Those fences are
C comments, so Small-C strips them and they never reach the output.

## What the language allows

Small-C is a subset of C, not C itself. These are the rules that catch
people out, and the list is complete for what these examples rely on.

Functions may use K&R definitions, which is all the original Small-C
accepted:

    add(a, b) int a, b; { return a + b; }

This compiler also accepts C99 definitions and prototypes, so the two
forms can be mixed in one file. See `c99.c`. Parameter types are
recorded but calls are not checked against them.

There are no structs, unions, enums or typedefs. There is no
`unsigned`, no `long`, no `float` and no `double`. An `int` is 16 bits
and a `char` is 8.

There are no pointer arrays, so `char *argv[]` is rejected. `argv` is
declared `int argv[]` instead, because a pointer is just a 16-bit
address.

Arrays have a single dimension.

Local variables must be declared at the top of a block, and they cannot
be initialised where they are declared. Globals can be:

    int data[8] = {42, 7, 19, 3, 25, 11, 1, 30};

Comments are `/* ... */` only. The preprocessor handles `#define`,
`#include`, `#ifdef`, `#ifndef`, `#else` and `#endif`, and macros take
no arguments.

Escapes in literals are `\n`, `\t`, `\b`, `\f` and octal.
