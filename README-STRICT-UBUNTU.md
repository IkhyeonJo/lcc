# LCC 3.6 Strict ANSI C90 — Ubuntu ELF32

This tree is an LCC 3.6 based strict C90 build for Ubuntu/Linux.

## Toolchain

C source -> LCC cpp -> LCC rcc -> GNU as (`as --32`) -> GNU ld (`ld -m elf_i386`) -> Linux ELF32.

The original DOS/MASM backend is not used by this Ubuntu build. `src/x86gas.md` is the Ubuntu backend and emits GNU assembler Intel syntax.

## Strict input policy

The build rejects the audited LCC extensions:

- `//` comments
- `#error`
- `#eval`
- `#pragma` (deliberately stricter than the minimum C90 requirement)
- enum trailing commas
- initializer trailing commas
- LCC/platform predefined extensions removed from the default driver environment

## Build

Run:

    ./build-ubuntu-gas.sh

The script uses the generated `lburg/gram.c`, so yacc/bison is not required for the bootstrap build.

## Verification

Run:

    ./verify/strict-c90-test.sh

The verified pipeline includes compilation, GNU assembly, ELF32 linking, and negative tests for the strict extensions.

The produced ELF32 test binary may not execute on an environment whose kernel lacks i386 execution support; successful `file`/`readelf` inspection and successful `ld` are still verified here.
