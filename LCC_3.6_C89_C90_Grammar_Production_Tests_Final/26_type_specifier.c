/*
 * C89/C90 Annex B.2 production: type-specifier
 *
 * Production:
 *   void | char | short | int | long | float | double | signed | unsigned | struct-or-union-specifier | enum-specifier | typedef-name
 *
 * This file is one-to-one with a named grammar nonterminal.
 * The program is intended to be compiled as an independent translation unit.
 */
typedef unsigned long U;
struct S { int x; };
enum E { A, B };
void f(void) {}
int main(void) {
    char c = 'a'; short s = 1; int i = 2; long l = 3L;
    float f = 1.0f; double d = 2.0; signed si = -1;
    unsigned u = 4U; U ul = 5UL;
    struct S st; enum E e = B;
    st.x = i;
    return (int)(c+s+l+f+d+si+u+ul+st.x+e);
}
