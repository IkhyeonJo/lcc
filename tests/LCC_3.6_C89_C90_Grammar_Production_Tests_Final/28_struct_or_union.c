/*
 * C89/C90 Annex B.2 production: struct-or-union
 *
 * Production:
 *   struct | union
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S { int x; };
union U { int x; };
int main(void) { struct S s; union U u; s.x = 1; u.x = 2; return s.x + u.x; }
