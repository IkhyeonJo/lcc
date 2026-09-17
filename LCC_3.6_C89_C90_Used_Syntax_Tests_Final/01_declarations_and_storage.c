#include <stdio.h>

extern int external_value;
static int static_value = 10;
register int register_value = 20;

int external_value = 30;

int main(void) {
    int x = 1;
    const int c = 2;
    printf("%d %d %d %d\n", x, c, static_value, register_value);
    return 0;
}
