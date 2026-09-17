# C89/C90 Annex B.2 — Production-by-Production Test Matrix

Source basis: the supplied ISO/IEC 9899:1990 (E) Annex B.2 phrase-structure grammar.

This suite has **one test file per named grammar nonterminal** in B.2.1–B.3.
Each `.c` file repeats the production in its header and contains a valid C89/C90
example intended to exercise that production.

Important:
- This is a **production-level** one-to-one mapping, not one file per individual alternative.
- Terminals such as `identifier`, `constant`, `;`, `{`, `++`, etc. are exercised through the productions that contain them.
- Recursive productions are represented by examples that actually take the recursive form.
- Preprocessing tests may contain directives that are intentionally disabled (for example `#error`) so the whole file remains compilable.
- This suite is about syntax/grammar coverage, not exhaustive semantic/undefined-behavior coverage.
