/*
 * C89/C90 Annex B.2 production: expression
 *
 * Production:
 *   assignment-expression | expression , assignment-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int a = 1, b = 2;
    return (a++, b++);
}
