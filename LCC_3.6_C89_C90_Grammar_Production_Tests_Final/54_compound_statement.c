/*
 * C89/C90 Annex B.2 production: compound-statement
 *
 * Production:
 *   { declaration-listopt statement-listopt }
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x=1;
    { int y=2; x+=y; }
    return x;
}
