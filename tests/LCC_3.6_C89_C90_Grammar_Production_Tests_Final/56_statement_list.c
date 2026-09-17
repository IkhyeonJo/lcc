/*
 * C89/C90 Annex B.2 production: statement-list
 *
 * Production:
 *   statement | statement-list statement
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x=0;
    x++;
    x++;
    return x;
}
