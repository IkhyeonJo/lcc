/*
 * C89/C90 Annex B.2 production: pp-tokens
 *
 * Production:
 *   preprocessing-token | pp-tokens preprocessing-token
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
#define ADD(a,b) ((a)+(b))
int main(void) { return ADD(1,2); }
