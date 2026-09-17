/*
 * C89/C90 Annex B.2 production: inclusive-OR-expression
 *
 * Production:
 *   exclusive-OR-expression | inclusive-OR-expression | exclusive-OR-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    return 3 | 5;
}
