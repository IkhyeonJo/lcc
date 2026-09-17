/*
 * C89/C90 Annex B.2 production: argument-expression-list
 *
 * Production:
 *   assignment-expression | argument-expression-list , assignment-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(int a, int b) { return a + b; }
int main(void) { return f(1, 2); }
