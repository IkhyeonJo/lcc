#include <stdio.h>

int values[3] = {1, 2, 3,};

struct Pair {
    int x;
    int y;
};

struct Pair pair = {4, 5,};

int main(void) {
    printf("%d %d %d %d\n", values[0], values[2], pair.x, pair.y);
    return 0;
}
