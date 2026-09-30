# Small-C code

This repo contains "Small-C" source code. The code was originally
published in the "Small-C Handbook" by Jim Hendrix in the 80s. This
copy is BYTE Small-C 1.0, the 8088/8086 MS-DOS port by R. E. Grehan.

The objective of this repo is to keep the code, and to have fun with it.

All of it is C99 and follows `STYLE.md`, enforced by `.clang-format`.
No K&R function definitions remain.

## Layout

The repo holds two bodies of code with different fates.

**The compiler** (`cc.h`, `ccdef.h`, `notice.h`, `cc1.c`, `cc11.c`,
`cc12.c`, `cc13.c`, `cc21.c`, `cc22.c`, `cc31.c`, `cc32.c`, `cc33.c`,
`cc41.c`, `cc42.c`) builds and runs on a modern host, and it emits MASM
assembly for MS-DOS. So it is now a cross-compiler.

**The runtime library** (`clib.h` plus every other `.c` file) is target
code for MS-DOS. It has been converted to C99 as well, which makes it
readable and checkable, but it no longer builds with either toolchain:
`cc86` cannot parse C99 prototypes, and a host compiler cannot assemble
the 8088 `#asm` blocks in `csyslib.c`, `ctell.c`, `exit.c`, `fgetc.c`,
`fputc.c`, `free.c`, `rename.c` and `unlink.c`. The library is therefore
reference source, and `clib.lib` is usable only as the prebuilt binary
committed here.

Assembly inside `#asm` blocks is fenced with `clang-format off`, so the
formatter leaves it byte-for-byte intact.

## Building the compiler

    make

This produces `cc86`. It builds clean under `-std=c99 -Wall -Wextra
-pedantic`, and also under `-Wshadow -Wconversion -Wstrict-prototypes
-Wmissing-prototypes`.

## Examples

`examples/` holds eight annotated Small-C programs, from hello world to
inline assembly, with a Makefile that compiles them all to `.mac`. See
`examples/README.md`, which also summarises what the Small-C subset
allows.

## Checking style

    make checkstyle

This checks every `.c` file and every C header against `.clang-format`,
and separately checks the 80 column cap of `STYLE.md` rule 9. The cap
needs its own check because `.clang-format` sets `ColumnLimit` to 0, so
the formatter never wraps a line and cannot enforce it. The target exits
non-zero on a violation, so it is usable from a hook or from CI.

    make format

This applies `.clang-format` in place. It does not wrap long lines, so
`make checkstyle` remains the authority on rule 9.

`prolog.h` and `epilog.h` are excluded from both targets. They carry a
`.h` extension but hold MASM assembly, not C.

## Compiling a program

    ./cc86 hello.c        # emits hello.mac, or writes to stdout
    masm hello.mac;;      # on DOS
    link hello            # link against clib.lib

`prolog.h` and `epilog.h` must be present when MASM runs, since the
emitted assembly includes them. Programs must be written in the Small-C
subset, which is what `cc86` parses: no structs, no unsigned, and
16-bit ints. Function definitions may be written in either K&R or C99
form, and prototypes are accepted; see `examples/c99.c`.

Options: `-m` monitor function headers, `-a` audible alarm on errors,
`-p` pause on errors, `-l1`/`-l2` list source to stdout or stderr, and
`-o` which is accepted but does nothing, because the peephole optimizer
is a stub that passes the staging buffer through unmodified.

## Known gaps

`call.mac` and `cend.mac` are referenced by `libmake.txt` but are
absent from this repo.

`libmake.txt` describes rebuilding `clib.lib` by running `cc86` over
the library sources. That recipe no longer applies, for the reason
given under Layout above. It is kept as a record of how the library was
originally composed.

`readme.doc` is the original BYTE documentation and is left unedited.
It refers to `CC86.EXE`, a prebuilt MS-DOS build of the compiler that
is no longer in the working tree. Building `cc86` from source replaces
it. To recover the historical binaries:

    git checkout d5f9b2b -- CC1.EXE CC86.EXE

## Resources

- [Retroarchive.org](http://www.retroarchive.org/cpm/archive/unofficial/small_c.html)
