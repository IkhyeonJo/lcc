/* C89/C90: #elif preprocessing directive */
#define FLAG 0
#if FLAG
#error wrong branch
#elif 1
int main(void) { return 0; }
#else
#error wrong branch
#endif
