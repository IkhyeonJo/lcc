#include <stdio.h>

int main(void) {
    int a = 1;
    int b = 2;
    int result;
    int *p = &a;

    result = (a++, b++, a < b ? a : b);
    printf("%d %lu %lu\n", result, (unsigned long)sizeof(a),
           (unsigned long)sizeof(p));
    return 0;
}
