#include <string.h>
static char rcsid[] = "$Id$";
extern char *concat(char *, char *);
#ifndef LCCDIR
#define LCCDIR "/usr/local/lib/lcc/"
#endif
#ifndef AS
#define AS "/usr/bin/as"
#endif
#ifndef LD
#define LD "/usr/bin/ld"
#endif
#ifndef START
#define START LCCDIR "start.o"
#endif
char *cpp[] = { LCCDIR "cpp", "-D__STDC__=1", "$1", "$2", "$3", 0 };
char *include[] = { "-I" LCCDIR "include", 0 };
char *com[] = { LCCDIR "rcc", "-target=x86/linux-gas", "$1", "$2", "$3", 0 };
char *as[] = { AS, "--32", "-o", "$3", "$2", "$1", 0 };
char *ld[] = { LD, "-m", "elf_i386", "-o", "$3", START, "$1", "$2", 0 };
int option(char *arg) {
    if (strncmp(arg, "-lccdir=", 8) == 0) {
        cpp[0] = concat(&arg[8], "/cpp");
        include[0] = concat("-I", concat(&arg[8], "/include"));
        com[0] = concat(&arg[8], "/rcc");
        ld[5] = concat(&arg[8], "/start.o");
        return 1;
    }
    if (strcmp(arg, "-g") == 0 || strcmp(arg, "-pg") == 0)
        return 1;
    return 0;
}
