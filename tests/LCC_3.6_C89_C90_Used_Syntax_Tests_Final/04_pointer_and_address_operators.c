#include <stdio.h>

int main(void) {
    int x = 42;
    int *p = &x;
    int **pp = &p;

    printf("%d %d %d\n", x, *p, **pp);
    return 0;
}
