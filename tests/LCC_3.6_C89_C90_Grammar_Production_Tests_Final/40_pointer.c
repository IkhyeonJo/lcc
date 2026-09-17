/*
 * C89/C90 Annex B.2 production: pointer
 *
 * Production:
 *   * type-qualifier-listopt | * type-qualifier-listopt pointer
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 1;
    int *p = &x;
    int **pp = &p;
    const int *cp = &x;
    return **pp + *cp;
}
