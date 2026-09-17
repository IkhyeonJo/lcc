#include <stdio.h>

int main(void) {
    int x = 10;
    const int cx = 20;
    int *p = &x;
    const int *pc = &cx;
    int * const cp = &x;

    *p = 11;
    *cp = 12;

    printf("%d %d %d\n", x, cx, *pc);
    return 0;
}
