# LCC 3.6 — Strict ANSI C90

This tree is LCC 3.6 with user-program extensions disabled as far as identified in the source audit.

Disabled/rejected:
- C++ `//` comments
- `#error`
- LCC-specific `#eval`
- `#pragma` (this is intentionally stricter than ordinary C90 implementation-defined pragma handling)
- trailing comma in enum lists
- trailing comma in initializers
- LCC driver predefined `__LCC__` and `unix` macros

This is a strict language/extension policy; it does not attempt to eliminate all implementation-defined behavior or ISO C90 translation limits.
