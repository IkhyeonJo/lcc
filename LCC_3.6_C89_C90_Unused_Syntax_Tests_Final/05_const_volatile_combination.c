/* C89/C90: repeated type-qualifier-list */
int main(void)
{
    const volatile int x = 10;
    return x != 10;
}
