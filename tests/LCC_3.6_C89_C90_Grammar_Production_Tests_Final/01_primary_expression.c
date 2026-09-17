/*
 * C89/C90 Annex B.2 production: primary-expression
 *
 * Production:
 *   identifier | constant | string-literal | ( expression )
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1;
    const char *s = "ok";
    x = (x);
    return x + s[0] - 'o';
}
