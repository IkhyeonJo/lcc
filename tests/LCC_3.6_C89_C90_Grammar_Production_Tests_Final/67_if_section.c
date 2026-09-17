/*
 * C89/C90 Annex B.2 production: if-section
 *
 * Production:
 *   if-group elif-groupsopt else-groupopt endif-line
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
#if 0
int bad(void) { return 0; }
#elif 1
int main(void) { return 1; }
#else
int main(void) { return 2; }
#endif
