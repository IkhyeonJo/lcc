#include <stdio.h>

typedef struct Node Node;

struct Node {
    int value;
    Node *next;
};

typedef enum Status {
    STATUS_OK,
    STATUS_ERROR
} Status;

int main(void) {
    Node n;
    Status s = STATUS_OK;
    n.value = 10;
    n.next = 0;
    printf("%d %d\n", n.value, s);
    return 0;
}
