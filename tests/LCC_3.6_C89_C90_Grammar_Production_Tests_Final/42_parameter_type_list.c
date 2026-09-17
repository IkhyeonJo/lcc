/*
 * C89/C90 Annex B.2 production: parameter-type-list
 *
 * Production:
 *   parameter-list | parameter-list , ...
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(int a, char *s) { return a + s[0]; }
int v(int a, ...) { return a; }
int main(void) { return f(1, "x") + v(2, 3); }
