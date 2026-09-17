#include <stdio.h>

int add(a, b)
int a;
int b;
{
    return a + b;
}

int main(void) {
    printf("%d\n", add(2, 3));
    return 0;
}
