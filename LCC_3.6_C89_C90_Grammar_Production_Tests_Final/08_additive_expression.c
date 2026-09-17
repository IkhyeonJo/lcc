/*
 * C89/C90 Annex B.2 production: additive-expression
 *
 * Production:
 *   multiplicative-expression | additive-expression + multiplicative-expression | additive-expression - multiplicative-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int a[3] = {1, 2, 3};
    int *p = a;
    return p[0] + 2 - p[1];
}
