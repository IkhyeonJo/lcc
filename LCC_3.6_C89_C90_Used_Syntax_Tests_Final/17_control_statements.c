#include <stdio.h>

int main(void) {
    int i;
    int x = 0;

    if (x == 0)
        x = 1;
    else
        x = 2;

    while (x < 3) {
        x++;
        if (x == 2)
            continue;
    }

    do {
        x--;
    } while (x > 1);

    for (i = 0; i < 2; i++)
        x += i;

    switch (x) {
    case 1:
        x++;
        break;
    case 2:
        x += 2;
        break;
    default:
        x = 0;
        break;
    }

    goto done;

done:
    return x >= 0 ? 0 : 1;
}
