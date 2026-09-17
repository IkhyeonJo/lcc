/*
 * C89/C90 Annex B.2 production: assignment-expression
 *
 * Production:
 *   conditional-expression | unary-expression assignment-operator assignment-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1;
    x = 2;
    x += 3;
    return x;
}
