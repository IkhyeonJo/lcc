# LCC 3.6 C89/C90 Grammar Production Tests

This is the exhaustive **named-production** companion to the Used/Unused syntax suites.

The supplied C89/C90 standard's Annex B.2 phrase-structure grammar is represented
as one independent test file per named nonterminal (74 files).

Recommended order:
1. Compile the LCC source.
2. Run the Used Syntax suite.
3. Run the Unused Syntax suite.
4. Run this production-by-production suite.
5. If a production test fails, inspect the exact production printed in that file's header.

This does not claim that every semantic constraint or every implementation-defined
behavior of the C standard is exhaustively tested.
