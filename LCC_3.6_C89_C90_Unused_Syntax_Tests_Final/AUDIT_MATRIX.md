# LCC 3.6 C89/C90 Syntax Coverage Audit

## Scope

This audit compares the C89/C90 phrase-structure grammar in the supplied C89/C90 standard document with **actual C source constructs in the LCC 3.6 compiler distribution**. The primary self-hosting scope is compiler source that is built as part of LCC itself: `src/`, `cpp/`, and `lburg/`. Mere appearances in comments, strings, keyword tables, generated assembly strings, or internal enum names are not counted as source-language use.

The `tst/` directory is treated as a separate test suite: it is evidence that a construct is supported by LCC, but it does **not** count as self-hosting coverage because those test programs are not what builds the compiler itself.

## Original 18-test suite

| Original test | C89/C90 construct | LCC 3.6 compiler-source use? | Action |
|---|---|---:|---|
| 01 unary plus | unary `+` | No actual source-language use found | KEEP |
| 02 auto | `auto` storage class | No actual declaration found | KEEP |
| 03 signed type specifier | standalone `signed` | No actual standalone use found | KEEP |
| 04 const volatile | repeated type qualifiers | No `const volatile` / `volatile const` declaration found | KEEP |
| 05 abstract array | original file used a named array parameter, not an abstract declarator | Not a valid unused-syntax test as written | REPLACE |
| 06 abstract pointer | original file used a named pointer, not an abstract declarator | Abstract pointer type-name is already used, e.g. `sizeof (Type *)` | REMOVE |
| 07 empty parameter list | `f()` | Yes: many actual definitions such as `static void emitYYnull() {}` | REMOVE |
| 08 old-style definition | `f(a,b) int a; int b;` | Yes | REMOVE |
| 09 ellipsis | `...` parameter list | Yes: `error`, `warning`, `print`, `fprint`, `stringf`, etc. | REMOVE |
| 10 function pointer | function-pointer declarator | Yes | REMOVE |
| 11 nested declarator | nested declarator/function pointer | Yes | REMOVE |
| 12 struct/union/enum | tags and definitions | Yes | REMOVE |
| 13 bit-field | `: constant-expression` | Yes: actual bit-fields in `src/config.h` | REMOVE |
| 14 initializer forms | initializer/initializer-list | Yes | REMOVE |
| 15 label/goto | label and `goto` | Yes | REMOVE |
| 16 switch/case/default | selection statement | Yes | REMOVE |
| 17 do-while | iteration statement | Yes: `src/stmt.c` has `dostmt` and actual `do` loops occur in compiler source | REMOVE |
| 18 comma/conditional | comma and `?:` expressions | Yes | REMOVE |

## Additional gaps found during the complete pass

The original 18 tests missed several grammar alternatives that are not exercised by compiler source:

| Test | C89/C90 construct | Result |
|---|---|---|
| 04 volatile | standalone `volatile` qualifier | No actual source-language declaration found |
| 06 abstract array type-name | `sizeof(int [3])` | No actual compiler-source use found |
| 07 tag-only struct declaration | `struct Forward;` | No actual compiler-source use found |
| 08 `#elif` | preprocessing directive | No actual directive found in compiler source |
| 09 `#error` | preprocessing directive | No actual directive found in compiler source |
| 10 `#pragma` | preprocessing directive | No actual directive found in compiler source |
| 11 null directive | `#` followed by newline | No actual directive found in compiler source |

`#line` is **not** included here because actual `# line` directives occur in `lburg/gram.c`. Likewise `#if`, `#ifdef`, `#ifndef`, `#else`, `#endif`, `#include`, `#define`, and `#undef` occur in compiler source.

## Important distinction

A parser implementation such as `case '+':` proves that LCC implements a grammar alternative, but it does not prove that compiling LCC itself exercised that alternative. Conversely, a real C construct in LCC source does prove that the compiler path was exercised when LCC was built with LCC.

This audit therefore uses three concepts:

1. **Implemented** — the LCC parser/preprocessor has code for the grammar feature.
2. **Self-hosting exercised** — an actual C source construct in compiler source uses the feature.
3. **Independent test required** — no self-hosting source use was found, so a dedicated C/preprocessor test is retained.

The final test directory contains only category (3). The mapping above preserves what happened to all 18 original tests.
