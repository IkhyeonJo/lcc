/*
 * C89/C90 Annex B.2 production: init-declarator-list
 *
 * Production:
 *   init-declarator | init-declarator-list , init-declarator
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int a = 1, b = 2, c;
int main(void) { c = a + b; return c; }
