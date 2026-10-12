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

## Testing the AT&T back end against the NASM back end

```bash
./BUILD-UBUNTU24.sh            # once
tests/att-vs-nasm/run.sh       # exit 0 = no unexpected difference
```

Every C file in `tests/*/` and `tst/` is built three ways — `gcc -m32 -std=c89`
(reference), LCC AT&T (`rcc -target=x86/linux` → `as --32`) and LCC NASM
(`rcc -target=x86/nasm` → `nasm -f elf32`) — and checked on three levels:

1. **runtime** — stdout and exit code of AT&T and NASM must match;
   `tst/` output is also compared with LCC's expected `x86/dos/tst/*.1bk`.
2. **machine code** — `cmp_obj.py` disassembles both objects and requires
   them to be instruction-for-instruction equivalent (branch targets,
   relocations and data directives normalized).
3. **grammar** — `check_grammar.py` requires `src/x86linux.md` to mirror
   `src/x86nasm.md` rule-for-rule (same rules, order and costs; only the
   templates differ). The AT&T file adds two mirror nonterminals, `ibase`
   (= `base`, rendered as an open `disp(basereg` prefix so that
   `addr: ADDx(index,ibase)` prints `disp(basereg,index,scale)`) and `iindex`
   (= `index`, so a bare index register prints `(%reg)` instead of
   `(,%reg,1)`).

Expected build failures (tests that are meant to fail, or use an undefined
`extern`) are listed at the top of `run.sh`; there are no known code
differences. Results go to `build-ubuntu24/test-att-vs-nasm/`.
VS Code: task *LCC: Test AT&T vs NASM*.

## Headers

`BUILD-UBUNTU24.sh` installs `include/x86/linux` (glibc-compatible: `stdin/
stdout/stderr`, `jmp_buf`, `assert`, `errno`) into
`build-ubuntu24/lcc-build/include`; see `include/x86/linux/README.md`.
