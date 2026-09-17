#include <stdio.h>

#define VALUE 42
#define ADD(a,b) ((a) + (b))
#undef UNUSED_MACRO

#ifdef VALUE
#define HAS_VALUE 1
#else
#define HAS_VALUE 0
#endif

#ifndef NEVER_DEFINED
#define HAS_NOT_DEFINED 1
#endif

#if VALUE == 42
#define BRANCH 1
#else
#define BRANCH 0
#endif

#line 100 "used_syntax_test.c"

int main(void) {
    printf("%d %d %d %d\n", VALUE, ADD(10, 20), HAS_VALUE, BRANCH);
    return 0;
}
