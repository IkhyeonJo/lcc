/*
 * C89/C90 Annex B.2 production: relational-expression
 *
 * Production:
 *   shift-expression | relational-expression < shift-expression | relational-expression > shift-expression | relational-expression <= shift-expression | relational-expression >= shift-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 2, y = 3;
    return (x < y) + (x > y) + (x <= y) + (x >= y);
}
