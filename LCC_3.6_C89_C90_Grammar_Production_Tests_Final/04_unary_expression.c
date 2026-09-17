/*
 * C89/C90 Annex B.2 production: unary-expression
 *
 * Production:
 *   postfix-expression | ++ unary-expression | -- unary-expression | unary-operator cast-expression | sizeof unary-expression | sizeof ( type-name )
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 2;
    int *p = &x;
    int a = *p;
    int b = ++a;
    int c = --b;
    unsigned long n = sizeof x;
    unsigned long m = sizeof(int *);
    return (int)(n + m + c);
}
