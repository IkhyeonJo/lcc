/*
 * C89/C90 Annex B.2 production: control-line
 *
 * Production:
 *   # include | # define | # undef | # line | # error | # pragma | null directive
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
#include <stddef.h>
#define X 1
#undef X
#define X 2
#line 100 "grammar_test.c"
#if 0
#error deliberately-disabled-error
#endif
#pragma once
int main(void) { return X; }
