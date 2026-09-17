/*
 * C89/C90 Annex B.2 production: struct-or-union-specifier
 *
 * Production:
 *   struct-or-union identifieropt { struct-declaration-list } | struct-or-union identifier
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S { int x; };
union U { int i; char c; };
struct S a;
union U b;
int main(void) { a.x = 1; b.i = 2; return a.x + b.i; }
