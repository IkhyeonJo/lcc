/*
 * C89/C90 Annex B.2 production: constant-expression
 *
 * Production:
 *   conditional-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
enum { N = 2 + 3 * 4 };
int a[N];
int main(void) { return N; }
