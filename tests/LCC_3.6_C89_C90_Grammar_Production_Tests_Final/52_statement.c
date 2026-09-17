/*
 * C89/C90 Annex B.2 production: statement
 *
 * Production:
 *   labeled-statement | compound-statement | expression-statement | selection-statement | iteration-statement | jump-statement
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x=0;
    if (x) { x++; } else x=1;
    while (x<2) x++;
    switch (x) { case 2: break; default: x=0; }
    return x;
}
