#include <stdio.h>

int add(int a, int b);
void show(int value);

int add(int a, int b) {
    return a + b;
}

void show(int value) {
    printf("%d\n", value);
}

int main(void) {
    show(add(2, 3));
    return 0;
}
