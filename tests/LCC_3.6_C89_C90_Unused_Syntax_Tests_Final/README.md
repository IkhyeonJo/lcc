# LCC 3.6 C89/C90 Unused Syntax Tests — Final Audit

These tests target C89/C90 grammar alternatives that were not found as actual source-language constructs in the LCC 3.6 compiler source used for self-hosting coverage.

The audit is based on the supplied C89/C90 standard document and the supplied LCC 3.6 source tree. The original 18-test suite was rechecked rather than blindly retained.

The final suite contains 11 tests. Several original tests were removed because LCC itself already uses those constructs; the do-while test is one such example.

See `AUDIT_MATRIX.md` for the complete mapping and methodology.

## Expected handling

Most files should compile successfully. `09_preprocessor_error.c` intentionally contains `#error`; its purpose is to verify that the directive is recognized by LCC's preprocessor, so its expected diagnostic behavior is documented separately rather than treating a diagnostic as a syntax failure.
