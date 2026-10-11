/* x86 running linux, ELF.
 *
 * Default back end: x86/linux -- AT&T syntax assembled by GNU as.
 * Alternative:      x86/nasm  -- Intel syntax assembled by NASM
 *                   (select with  lcc -asm=nasm ...).
 */

#include <string.h>

#ifndef LCCDIR
#define LCCDIR "/usr/local/lib/lcc/"
#endif

#define NASMPATH "/usr/bin/nasm"
#define GASPATH "/usr/bin/as"

char *cpp[] = { LCCDIR "cpp", "-D__STDC__=1",
    "-D__ELF__", "-Di386", "-D__i386", "-D__i386__",
    "-Dlinux", "-D__linux", "-D__linux__",
    "$1", "$2", "$3", 0
};
char *include[] = { "-I" LCCDIR "include", "-I/usr/local/include",
    "-I/usr/include", 0
};
char *com[] = { LCCDIR "rcc", "-target=x86/linux",
    "$1", "$2", "$3", 0
};
char *as[] = { GASPATH, "--32", "-o", "$3", "$1", "$2", 0, 0 };
static char *gas_as[]  = { GASPATH, "--32", "-o", "$3", "$1", "$2", 0, 0 };
static char *nasm_as[] = { NASMPATH, "-a", "-felf", "-o", "$3", "$1", "$2", 0 };
char *ld[] = { "/usr/bin/gcc", "-m32", "-o", "$3", "$2", "$1", 0 };
static char *bbexit = LCCDIR "bbexit.o";

extern char *concat(char *, char *);
extern int access(const char *, int);

static void setas(char **v)
{
    int i;
    for (i = 0; (as[i] = v[i]) != 0; i++)
        ;
}

int option(char *arg)
{
    if (strcmp(arg, "-asm=att") == 0 || strcmp(arg, "-asm=gas") == 0) {
        com[1] = "-target=x86/linux";
        setas(gas_as);
    } else if (strcmp(arg, "-asm=nasm") == 0) {
        com[1] = "-target=x86/nasm";
        setas(nasm_as);
    } else if (strncmp(arg, "-lccdir=", 8) == 0) {
        cpp[0] = concat(&arg[8], "/cpp");
        include[0] = concat("-I", concat(&arg[8], "/include"));
        com[0] = concat(&arg[8], "/rcc");
        bbexit = concat(&arg[8], "/bbexit.o");
    } else if (strcmp(arg, "-g") == 0) ;
    else if (strcmp(arg, "-b") == 0 && access(bbexit, 4) == 0)
        ld[13] = bbexit;
    else
        return 0;
    return 1;
}
