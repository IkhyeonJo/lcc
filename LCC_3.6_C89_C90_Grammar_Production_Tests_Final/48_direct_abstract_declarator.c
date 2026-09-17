/*
 * C89/C90 Annex B.2 production: direct-abstract-declarator
 *
 * Production:
 *   ( abstract-declarator ) | [ constant-expressionopt ] | direct-abstract-declarator [ constant-expressionopt ] | ( parameter-type-listopt ) | direct-abstract-declarator ( parameter-type-listopt )
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int f(int *);
int main(void) {
    unsigned long a = sizeof(int [3]);
    unsigned long b = sizeof(int (*)(int));
    return f((int *)0) + (int)(a + b);
}
int f(int *p) { return p == 0; }
