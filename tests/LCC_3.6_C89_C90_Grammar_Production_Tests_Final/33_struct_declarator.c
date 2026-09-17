/*
 * C89/C90 Annex B.2 production: struct-declarator
 *
 * Production:
 *   declarator | declaratoropt : constant-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S { unsigned a : 1; unsigned b : 3; int c; };
int main(void) { struct S s; s.a=1; s.b=3; s.c=4; return s.c; }
