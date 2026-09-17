/*
 * C89/C90 Annex B.2 production: init-declarator
 *
 * Production:
 *   declarator | declarator = initializer
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int x;
int y = 3;
int main(void) { return x + y; }
