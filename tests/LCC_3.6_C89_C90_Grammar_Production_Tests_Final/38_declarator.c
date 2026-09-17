/*
 * C89/C90 Annex B.2 production: declarator
 *
 * Production:
 *   pointeropt direct-declarator
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int x;
int *p = &x;
int main(void) { return *p; }
