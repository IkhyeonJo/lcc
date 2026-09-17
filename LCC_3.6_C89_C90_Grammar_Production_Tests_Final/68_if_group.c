/*
 * C89/C90 Annex B.2 production: if-group
 *
 * Production:
 *   # if constant-expression new-line groupopt | # ifdef identifier new-line groupopt | # ifndef identifier new-line groupopt
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
#define X 1
#if X
int main(void) { return 1; }
#endif
