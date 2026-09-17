/*
 * C89/C90 Annex B.2 production: storage-class-specifier
 *
 * Production:
 *   typedef | extern | static | auto | register
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
typedef int T;
extern int e;
static int s;
int f(void) {
    auto int a = 1;
    register int r = 2;
    return a + r + s + e;
}
int e = 3;
