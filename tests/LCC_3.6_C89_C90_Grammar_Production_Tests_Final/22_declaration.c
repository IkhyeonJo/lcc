/*
 * C89/C90 Annex B.2 production: declaration
 *
 * Production:
 *   declaration-specifiers init-declarator-listopt ;
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
typedef int T;
extern int e;
static int s;
int a = 1, b = 2;
int main(void) { T x = a + b; return x + s + e; }
