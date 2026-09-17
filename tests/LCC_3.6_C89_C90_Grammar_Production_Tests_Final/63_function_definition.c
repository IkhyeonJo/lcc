/*
 * C89/C90 Annex B.2 production: function-definition
 *
 * Production:
 *   declarator declaration-listopt compound-statement
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int add(a,b)
int a;
int b;
{
    return a+b;
}
int main(void) { return add(1,2); }
