/*
 * C89/C90 Annex B.2 production: enumerator
 *
 * Production:
 *   enumeration-constant | enumeration-constant = constant-expression
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
enum E { A, B = 10 };
int main(void) { return B; }
