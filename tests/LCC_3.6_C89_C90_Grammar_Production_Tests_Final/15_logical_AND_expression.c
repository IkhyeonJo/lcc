/*
 * C89/C90 Annex B.2 production: logical-AND-expression
 *
 * Production:
 *   inclusive-OR-expression | logical-AND-expression && inclusive-OR-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1, y = 2;
    return x && y;
}
