/*
 * C89/C90 Annex B.2 production: unary-operator
 *
 * Production:
 *   & | * | + | - | ~ | !
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1;
    int *p = &x;
    int a = *p;
    int b = +a;
    int c = -b;
    int d = ~c;
    return !d;
}
