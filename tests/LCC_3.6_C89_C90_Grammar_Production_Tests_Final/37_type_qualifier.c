/*
 * C89/C90 Annex B.2 production: type-qualifier
 *
 * Production:
 *   const | volatile
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
const int a = 1;
volatile int b = 2;
int main(void) { return a + b; }
