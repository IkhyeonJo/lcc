#include <stdio.h>

typedef int Type;

int main(void) {
    Type value = 7;
    unsigned long size1 = sizeof(Type *);
    unsigned long size2 = sizeof(int (*)(int, int));

    printf("%d %lu %lu\n", value, size1, size2);
    return 0;
}
