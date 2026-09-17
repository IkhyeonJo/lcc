/*
 * C89/C90 Annex B.2 production: type-name
 *
 * Production:
 *   specifier-qualifier-list abstract-declaratoropt
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1;
    unsigned long n = sizeof(const int *);
    return (int)(n + x);
}
