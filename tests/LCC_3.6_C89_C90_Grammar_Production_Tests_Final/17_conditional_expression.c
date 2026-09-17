/*
 * C89/C90 Annex B.2 production: conditional-expression
 *
 * Production:
 *   logical-OR-expression | logical-OR-expression ? expression : conditional-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1, y = 2;
    return x ? y : 0;
}
