/*
 * C89/C90 Annex B.2 production: AND-expression
 *
 * Production:
 *   equality-expression | AND-expression & equality-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    return (12 == 12) & (7 != 8);
}
