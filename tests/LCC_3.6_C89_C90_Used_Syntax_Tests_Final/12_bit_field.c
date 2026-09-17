#include <stdio.h>

struct Flags {
    unsigned a : 1;
    unsigned b : 3;
    unsigned c : 4;
};

int main(void) {
    struct Flags f;
    f.a = 1;
    f.b = 5;
    f.c = 9;
    printf("%u %u %u\n", f.a, f.b, f.c);
    return 0;
}
