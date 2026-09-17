/*
 * C89/C90 Annex B.2 production: parameter-list
 *
 * Production:
 *   parameter-declaration | parameter-list , parameter-declaration
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(int a, char *s) { return a + s[0]; }
int main(void) { return f(1, "x"); }
