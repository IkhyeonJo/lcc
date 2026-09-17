/*
 * C89/C90 Annex B.2 production: direct-declarator
 *
 * Production:
 *   identifier | ( declarator ) | direct-declarator [ constant-expressionopt ] | direct-declarator ( parameter-type-list ) | direct-declarator ( identifier-listopt )
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int a[3];
int f(int x) { return x; }
int g(a) int a; { return a; }
int main(void) { return f(a[0]) + g(1); }
