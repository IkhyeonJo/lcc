/*
 * C89/C90 Annex B.2 production: translation-unit
 *
 * Production:
 *   external-declaration | translation-unit external-declaration
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int x;
static int f(void) { return x; }
int main(void) { return f(); }
