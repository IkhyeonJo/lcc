#include <stdio.h>

int main(void) {
    int a[3] = {10, 20, 30};
    int *p = a;

    printf("%d %d %d\n", a[0], *(p + 1), p[2]);
    return 0;
}
