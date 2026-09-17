# LCC 3.6 — Strict ANSI C90 mode

This tree is a strict-input variant of LCC 3.6 intended to reject the non-standard source-language/preprocessor extensions identified in the LCC 3.6 source audit.

## Rejected extensions

- C++-style `//` comments
- `#error`
- LCC-specific `#eval`
- `#pragma` (this variant deliberately rejects implementation-specific pragmas for a stricter no-extension policy)
- trailing comma in an enumerator list
- trailing comma in an initializer
- driver-injected `__LCC__` and `unix` predefined macros
- the LCC preprocessor `+` option that enabled C++ comments

## Retained

Standard C90 constructs and standard predefined macros such as `__STDC__`, `__LINE__`, `__FILE__`, `__DATE__`, and `__TIME__` remain available as appropriate.

This is a strict *language/extension policy* for LCC 3.6, not a claim that every implementation-defined property of a C90 implementation has been made identical across platforms. C90 itself permits implementation-defined behavior and has implementation limits.

## Build note

The included source is the modified LCC 3.6 tree. On the build environment used for verification, the C compiler sources compiled successfully, but the complete build could not finish because `yacc` was not installed.
