#include <stdio.h>

int main(void) {
    int x = 0;

start:
    x++;

    if (x < 3)
        goto start;

    printf("%d\n", x);
    return 0;
}
