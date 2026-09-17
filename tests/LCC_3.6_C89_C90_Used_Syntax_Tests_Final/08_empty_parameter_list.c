#include <stdio.h>

static int value() {
    return 42;
}

int main(void) {
    printf("%d\n", value());
    return 0;
}
