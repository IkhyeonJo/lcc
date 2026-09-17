#include <stdio.h>

int main(void) {
    int x = 5;
    int y = 2;
    int r;

    r = -x + +y;
    r = ~r;
    r = !r;
    r = x * y + x / y - x % y;
    r = r << 1;
    r = r >> 1;
    r = (x < y) ? x : y;
    r = (x == y) || (x != y && x >= y);
    r = (x & y) | (x ^ y);

    printf("%d\n", r);
    return 0;
}
