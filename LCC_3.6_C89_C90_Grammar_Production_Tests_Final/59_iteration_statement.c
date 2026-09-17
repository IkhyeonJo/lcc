/*
 * C89/C90 Annex B.2 production: iteration-statement
 *
 * Production:
 *   while ( expression ) statement | do statement while ( expression ) ; | for ( expressionopt ; expressionopt ; expressionopt ) statement
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int i=0;
    while (i<1) i++;
    do { i--; } while (i>0);
    for (i=0; i<2; i++) { }
    return i;
}
