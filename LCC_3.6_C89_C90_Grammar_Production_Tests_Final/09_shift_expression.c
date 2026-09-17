/*
 * C89/C90 Annex B.2 production: shift-expression
 *
 * Production:
 *   additive-expression | shift-expression << additive-expression | shift-expression >> additive-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1;
    x = x << 3;
    return x >> 1;
}
