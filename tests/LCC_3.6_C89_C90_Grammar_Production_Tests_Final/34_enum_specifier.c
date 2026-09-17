/*
 * C89/C90 Annex B.2 production: enum-specifier
 *
 * Production:
 *   enum identifieropt { enumerator-list } | enum identifier
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
enum E { A, B = 5, C };
enum E value = C;
int main(void) { return value; }
