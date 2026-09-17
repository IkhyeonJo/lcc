/*
 * C89/C90 Annex B.2 production: initializer-list
 *
 * Production:
 *   initializer | initializer-list , initializer
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int a[3] = {1, 2, 3};
int main(void) { return a[0]+a[1]+a[2]; }
