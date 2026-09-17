/*
 * C89/C90 Annex B.2 production: identifier-list
 *
 * Production:
 *   identifier | identifier-list , identifier
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(a,b) int a; int b; { return a+b; }
int main(void) { return f(1,2); }
