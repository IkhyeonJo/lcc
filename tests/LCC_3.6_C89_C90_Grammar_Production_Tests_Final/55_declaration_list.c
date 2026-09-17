/*
 * C89/C90 Annex B.2 production: declaration-list
 *
 * Production:
 *   declaration | declaration-list declaration
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int a=1;
    int b=2;
    return a+b;
}
