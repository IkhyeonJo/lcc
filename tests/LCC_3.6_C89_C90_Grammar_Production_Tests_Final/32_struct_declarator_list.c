/*
 * C89/C90 Annex B.2 production: struct-declarator-list
 *
 * Production:
 *   struct-declarator | struct-declarator-list , struct-declarator
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S { int a, b, c; };
int main(void) { struct S s = {1,2,3}; return s.a+s.b+s.c; }
