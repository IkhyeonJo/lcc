/*
 * C89/C90 Annex B.2 production: parameter-declaration
 *
 * Production:
 *   declaration-specifiers declarator | declaration-specifiers abstract-declaratoropt
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(int a, char *s, int *p) { return a + s[0] + *p; }
int main(void) { int x=1; return f(2,"a",&x); }
