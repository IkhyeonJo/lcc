/*
 * C89/C90 Annex B.2 production: selection-statement
 *
 * Production:
 *   if ( expression ) statement | if ( expression ) statement else statement | switch ( expression ) statement
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x=2;
    if (x==1) x=3; else x=4;
    switch (x) { case 4: return 0; default: return 1; }
}
