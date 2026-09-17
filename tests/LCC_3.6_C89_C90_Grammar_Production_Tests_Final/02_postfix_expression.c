/*
 * C89/C90 Annex B.2 production: postfix-expression
 *
 * Production:
 *   primary-expression | postfix-expression [ expression ] | postfix-expression ( argument-expression-listopt ) | postfix-expression . identifier | postfix-expression -> identifier | postfix-expression ++ | postfix-expression --
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
struct S { int x; };
int f(int x) { return x; }

int main(void) {
    struct S s;
    struct S *p = &s;
    int a[2] = {1, 2};
    int x = a[0];
    x = f(x);
    s.x = x;
    p->x++;
    x--;
    return x;
}
