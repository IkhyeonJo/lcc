/*
 * C89/C90 Annex B.2 production: elif-group
 *
 * Production:
 *   # elif constant-expression new-line groupopt
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
#if 0
int main(void) { return 0; }
#elif 1
int main(void) { return 1; }
#endif
