/*
 * C89/C90 Annex B.2 production: specifier-qualifier-list
 *
 * Production:
 *   type-specifier specifier-qualifier-listopt | type-qualifier specifier-qualifier-listopt
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S {
    const unsigned int x;
    volatile int y;
};
int main(void) { struct S s = { 1, 2 }; return s.x + s.y; }
