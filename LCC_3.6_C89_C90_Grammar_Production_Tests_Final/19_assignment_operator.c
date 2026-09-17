/*
 * C89/C90 Annex B.2 production: assignment-operator
 *
 * Production:
 *   = *= /= %= += -= <<= >>= &= ^= |=
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
int main(void) {
    int x = 100;
    x = 1; x *= 2; x /= 2; x %= 3; x += 4; x -= 1;
    x <<= 1; x >>= 1; x &= 7; x ^= 3; x |= 8;
    return x;
}
