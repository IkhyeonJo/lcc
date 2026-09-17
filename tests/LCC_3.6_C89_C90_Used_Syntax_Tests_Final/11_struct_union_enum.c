#include <stdio.h>

enum Color { RED, GREEN, BLUE };

struct Point {
    int x;
    int y;
};

union Value {
    int i;
    char c;
};

int main(void) {
    struct Point p = {1, 2};
    union Value v;
    enum Color color = GREEN;

    v.i = 42;
    printf("%d %d %d %d\n", p.x, p.y, v.i, color);
    return 0;
}
