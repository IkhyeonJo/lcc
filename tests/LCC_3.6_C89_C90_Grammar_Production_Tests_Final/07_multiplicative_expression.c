/*
 * C89/C90 Annex B.2 production: multiplicative-expression
 *
 * Production:
 *   cast-expression | multiplicative-expression * cast-expression | multiplicative-expression / cast-expression | multiplicative-expression % cast-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 20;
    return (x * 2) / 4 % 3;
}
