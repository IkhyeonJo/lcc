/* C89/C90: array abstract-declarator in a type-name */
int main(void)
{
    int n = sizeof(int [3]);
    return n == 3 * sizeof(int) ? 0 : 1;
}
