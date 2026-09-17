# LCC 3.6 — C89/C90 Used Syntax Tests

Purpose:
- Re-validate, using independent input programs, the C89/C90 syntax confirmed to be actually used in the LCC 3.6 source code (`src/`, `cpp/`, `lburg`).
- Use this together with `LCC_3.6_C89_C90_Unused_Syntax_Tests_Final.zip`.
- This set is not a formal completeness proof of "all syntax implemented by LCC"; it independently tests representative syntax that was confirmed to be actually used during this source audit.

Recommended verification:
1. Run preprocessing/compilation with LCC
2. Generate assembly
3. assembler/linker
4. Verify the execution results

Notes:
- If a test fails, identify whether the failure occurs in the parser, semantic analysis, type system, backend, or runtime/library.
- Results such as `sizeof` and integer representation may vary depending on the target/ABI.
- `#line` is included because its actual use was confirmed during the source audit.
- `auto`, standalone `signed`, `volatile`, `const volatile`, abstract array type-name, tag-only struct declaration,
  `#elif`, `#error`, `#pragma`, null preprocessing directive, etc. are covered in the separate Unused test set.
