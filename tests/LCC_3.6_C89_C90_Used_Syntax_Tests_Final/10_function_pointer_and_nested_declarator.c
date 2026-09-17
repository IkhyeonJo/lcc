#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main(void) {
    int (*fp)(int, int) = add;
    int (*(*chooser)(void))(int, int);

    chooser = 0;
    printf("%d\n", (*fp)(2, 3));
    printf("%d\n", chooser == 0);
    return 0;
}
