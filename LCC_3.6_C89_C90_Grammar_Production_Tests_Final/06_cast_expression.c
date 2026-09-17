/*
 * C89/C90 Annex B.2 production: cast-expression
 *
 * Production:
 *   unary-expression | ( type-name ) cast-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 3;
    double d = (double)x;
    return (int)d;
}
