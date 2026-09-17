/*
 * C89/C90 Annex B.2 production: labeled-statement
 *
 * Production:
 *   identifier : statement | case constant-expression : statement | default : statement
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x=1;
    goto done;
done:
    switch (x) { case 1: x++; break; default: x=0; }
    return x;
}
