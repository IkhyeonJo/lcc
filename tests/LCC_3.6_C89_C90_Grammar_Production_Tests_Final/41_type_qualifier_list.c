/*
 * C89/C90 Annex B.2 production: type-qualifier-list
 *
 * Production:
 *   type-qualifier | type-qualifier-list type-qualifier
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
const volatile int x = 1;
int main(void) { return x; }
