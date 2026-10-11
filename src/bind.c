#include "c.h"
extern Interface nullIR, symbolicIR;
extern Interface mipsebIR, mipselIR;
extern Interface sparcIR, solarisIR;
extern Interface x86IR, x86nasmIR, x86linuxIR;
Binding bindings[] = {
    "symbolic", &symbolicIR,
    "mips-irix", &mipsebIR,
    "mips-ultrix", &mipselIR,
    "sparc-sun", &sparcIR,
    "sparc-solaris", &solarisIR,
    "x86-dos", &x86IR,
    "x86-nasm", &x86nasmIR,
    "x86-linux", &x86linuxIR,
    "symbolic/irix", &symbolicIR,       /* omit */
    "mips/irix", &mipsebIR,     /* omit */
    "mips/ultrix", &mipselIR,   /* omit */
    "sparc/sun", &sparcIR,      /* omit */
    "sparc/solaris", &solarisIR,        /* omit */
    "x86/dos", &x86IR,          /* omit */
    "x86/nasm", &x86nasmIR,     /* omit */
    "x86/linux", &x86linuxIR,   /* omit */
    "x86/att", &x86linuxIR,     /* omit */
    "null", &nullIR,
    NULL, NULL
};
