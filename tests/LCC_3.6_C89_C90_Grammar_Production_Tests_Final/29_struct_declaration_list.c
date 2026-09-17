/*
 * C89/C90 Annex B.2 production: struct-declaration-list
 *
 * Production:
 *   struct-declaration | struct-declaration-list struct-declaration
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S {
    int x;
    char c;
};
int main(void) { struct S s; s.x = 1; s.c = 'a'; return s.x; }
