/*
 * C89/C90 Annex B.2 production: logical-OR-expression
 *
 * Production:
 *   logical-AND-expression | logical-OR-expression || logical-AND-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 0, y = 2;
    return x || y;
}
