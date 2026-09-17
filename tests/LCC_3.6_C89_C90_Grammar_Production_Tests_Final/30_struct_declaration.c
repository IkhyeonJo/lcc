/*
 * C89/C90 Annex B.2 production: struct-declaration
 *
 * Production:
 *   specifier-qualifier-list struct-declarator-list ;
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S {
    const int x, y;
    unsigned int z;
};
int main(void) { struct S s = {1, 2, 3}; return s.x + s.y + s.z; }
