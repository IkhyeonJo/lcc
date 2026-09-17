/*
 * C89/C90 Annex B.2 production: abstract-declarator
 *
 * Production:
 *   pointer | pointeropt direct-abstract-declarator
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(int *);
int main(void) {
    int x = 1;
    return f(&x);
}
int f(int *p) { return *p; }
