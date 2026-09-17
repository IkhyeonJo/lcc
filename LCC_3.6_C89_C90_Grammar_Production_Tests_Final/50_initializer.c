/*
 * C89/C90 Annex B.2 production: initializer
 *
 * Production:
 *   assignment-expression | { initializer-list } | { initializer-list , }
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int a = 1;
int b[2] = {2, 3};
int c[2] = {4, 5,};
int main(void) { return a+b[0]+c[1]; }
