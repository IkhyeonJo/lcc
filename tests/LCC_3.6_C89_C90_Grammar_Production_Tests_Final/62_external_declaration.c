/*
 * C89/C90 Annex B.2 production: external-declaration
 *
 * Production:
 *   function-definition | declaration
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int x;
int f(void) { return x; }
int main(void) { return f(); }
