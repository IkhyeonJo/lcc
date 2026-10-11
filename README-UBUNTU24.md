# LCC 3.6 on Ubuntu 24.04 — NASM/ELF port (v5)

This kit bootstraps the historical LCC 3.6 source on Ubuntu 24.04 using the
historical NASM x86 backend.

The old Linux ELF driver hard-coded 1990s startup-object paths such as
`/usr/lib/crt1.o` and `/usr/lib/crtbegin.o`. On Ubuntu 24.04 those objects
are supplied through the multilib GCC toolchain instead. This version changes
the final link to `/usr/bin/gcc -m32`, allowing GCC to select the correct
multiarch startup objects and libraries.

## Back ends: AT&T (default) and NASM

Two x86 back ends share the same instruction selection, register allocation
and calling convention; they differ only in assembly syntax.

| rcc target                      | syntax | assembler        | lcc driver flag        |
|---------------------------------|--------|------------------|------------------------|
| `x86/linux` (alias `x86/att`)   | AT&T   | GNU as (`as --32`) | default / `-asm=att` |
| `x86/nasm`                      | Intel  | NASM (`-felf`)   | `-asm=nasm`            |

```bash
lcc -S hello.c               # hello.s in AT&T syntax
lcc -o hello hello.c         # cpp -> rcc (x86/linux) -> as --32 -> gcc -m32
lcc -asm=nasm -o hello hello.c   # previous NASM pipeline
```

The AT&T back end lives in `src/x86linux.md`. Note the x87 operand quirk of
AT&T syntax: Intel `fsubp st1` / `fdivp st1` are written `fsubrp`/`fdivrp
%st,%st(1)` for GNU as so that the computed result is the same.

It also patches the LCC 3.6 lburg grammar for modern Bison and uses the NASM
executable found by `command -v nasm`.

## VS Code workspace

Open this directory itself as the VS Code workspace:

```bash
cd ~/lcc
code .
```

The `.vscode/launch.json` is at the workspace root, so F5 exposes the LCC debug configurations automatically.
