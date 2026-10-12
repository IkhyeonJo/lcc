# include/x86/linux — glibc-compatible ANSI C headers for LCC on Linux (i386)

Copy of `include/x86/dos` with the four places that do not match the glibc
i386 ABI fixed. Everything else is identical.

| header     | DOS version                         | Linux version                                        |
|------------|-------------------------------------|------------------------------------------------------|
| `stdio.h`  | `stdin/stdout/stderr` = `&_iob[n]` (no `_iob` in glibc → link error); `feof/ferror/clearerr` read DOS `FILE` fields | `&_IO_2_1_stdin_` etc. (still address constants, so `FILE *f = stdin;` stays a valid static initializer); `FILE` is opaque, `feof/ferror/clearerr` are the libc functions |
| `setjmp.h` | `typedef int jmp_buf[13];` (52 bytes) | `jmp_buf[39]` (156 bytes = glibc `struct __jmp_buf_tag`) |
| `assert.h` | calls `_assert()` (not in glibc)    | calls `__assert_fail()`                              |
| `errno.h`  | `extern int errno;`                 | `(*__errno_location())` (glibc errno is thread-local) |

Installed by `BUILD-UBUNTU24.sh` into `build-ubuntu24/lcc-build/include` (the
`lcc` driver searches it first) and used by `tests/att-vs-nasm/run.sh`.
