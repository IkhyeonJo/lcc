/*
 * C89/C90 Annex B.2 production: exclusive-OR-expression
 *
 * Production:
 *   AND-expression | exclusive-OR-expression ^ AND-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    return 3 ^ 5;
}
