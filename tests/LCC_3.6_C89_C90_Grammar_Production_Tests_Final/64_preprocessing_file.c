/*
 * C89/C90 Annex B.2 production: preprocessing-file
 *
 * Production:
 *   groupopt
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
#define X 3
#if X == 3
int main(void) { return X; }
#else
int main(void) { return 1; }
#endif
