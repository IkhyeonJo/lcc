#include <stdio.h>

int main(void) {
    int x = 100;

    x *= 2;
    x /= 4;
    x %= 7;
    x += 3;
    x -= 1;
    x <<= 1;
    x >>= 1;
    x &= 15;
    x ^= 3;
    x |= 8;

    printf("%d\n", x);
    return 0;
}
