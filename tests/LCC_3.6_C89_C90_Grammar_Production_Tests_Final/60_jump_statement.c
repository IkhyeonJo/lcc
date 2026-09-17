/*
 * C89/C90 Annex B.2 production: jump-statement
 *
 * Production:
 *   goto identifier ; | continue ; | break ; | return expressionopt ;
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int i;
again:
    for (i=0; i<3; i++) {
        if (i==1) continue;
        if (i==2) break;
    }
    if (i==3) goto done;
    goto again;
done:
    return;
}
