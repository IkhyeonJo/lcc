/* C89/C90: tag-only struct declaration */
struct Forward;

int main(void)
{
    struct Forward *p = 0;
    return p != 0;
}
