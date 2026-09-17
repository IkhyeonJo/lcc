#include <stdio.h>

typedef unsigned long ULong;
typedef short Short;
typedef long Long;
typedef unsigned char UChar;

int main(void) {
    char c = 'A';
    short s = 2;
    int i = 3;
    long l = 4L;
    unsigned u = 5U;
    unsigned long ul = 6UL;
    float f = 1.5f;
    double d = 2.5;
    ULong t = ul;
    Short ss = s;
    Long ll = l;
    UChar uc = (UChar)c;
    printf("%c %d %d %ld %u %lu %f %f %lu %d %ld %u\n",
           c, s, i, l, u, ul, f, d, t, ss, ll, uc);
    return 0;
}
