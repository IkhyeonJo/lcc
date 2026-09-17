# LCC 3.6 — Strict ANSI C90 / Ubuntu 24.04

This repository is a modified LCC 3.6 source tree for studying compiler construction on Ubuntu 24.04.

It provides one entry point:

```bash
chmod +x BUILD-UBUNTU24.sh
./BUILD-UBUNTU24.sh
```

The goal is to make the complete workflow reproducible from a single script:

1. install required Ubuntu build packages
2. obtain/build the historical x86 NASM backend used by this port
3. build LCC 3.6
4. install the resulting `lcc` toolchain
5. run Strict ANSI C90 positive/negative tests
6. generate a real Linux i386 ELF executable
7. verify the resulting ELF file

No DOS/MASM output is used for the Ubuntu target. The target is Linux i386 ELF, with NASM producing `elf32` objects and GNU tools performing the final link.

---

## 1. What is included

The main LCC components are:

```text
lcc    - compiler driver
cpp    - C preprocessor
rcc    - C compiler
lburg  - code-generator generator
```

The basic pipeline is:

```text
hello.c
   |
   v
  lcc
   |
   +----> cpp
   |
   +----> rcc
   |        |
   |        v
   |      assembly
   |        |
   |        v
   |      NASM elf32
   |        |
   |        v
   |      ELF object
   |        |
   |        v
   |      GNU linker
   |
   v
Linux ELF executable
```

---

## 2. Strict ANSI C90 policy

The source was audited for LCC extensions accepted in user programs. The strict version disables the extensions identified in the audit:

- C++-style `//` comments are disabled.
- The command-line switch that re-enables C++ comments is removed.
- `#error` is diagnosed as not being part of ANSI C90.
- LCC-specific `#eval` is diagnosed as not being part of ANSI C90.
- A trailing comma in an enumerator list is diagnosed as not being part of ANSI C90.
- A trailing comma in an initializer list is diagnosed as not being part of ANSI C90.

`#pragma` is retained. ANSI C90 specifies implementation-defined handling of pragmas, so merely accepting and ignoring a pragma is not by itself a violation of the C90 language standard. If an even narrower project policy is desired, the pragma handler can be changed to an error as well.

This strict mode concerns the C90 language and preprocessing facilities. It does **not** pretend to eliminate implementation-defined behavior or implementation translation limits specified by the C90 standard. For example, properties such as the size of `int` remain target/implementation dependent.

The source tree also avoids modern C language constructs such as `inline`, `restrict`, `_Bool`, `_Complex`, `_Static_assert`, compound literals, designated initializers, and C99-style declaration placement in the compiler sources used by the Ubuntu build.

---

## 3. One-command Ubuntu 24.04 build

From the repository root:

```bash
chmod +x BUILD-UBUNTU24.sh
./BUILD-UBUNTU24.sh
```

The script is intended to be the **single build/test entry point**. It performs dependency setup, compiler construction, strict-conformance tests, and an actual ELF generation test.

The generated compiler is installed as:

```text
/usr/local/bin/lcc
```

The exact installation and backend details are handled by `BUILD-UBUNTU24.sh`; users should not need to reproduce the historical build steps manually.

---

## 4. What the automated tests check

### Strict C90 acceptance

The test suite includes valid C90 constructs such as:

- declarations and definitions
- old-style function definitions
- function prototypes
- arrays and pointers
- structures, unions, and enumerations
- typedefs
- expressions and casts
- control statements
- storage classes and type qualifiers
- C90 preprocessing facilities

### Extension rejection

The strict tests exercise non-C90 constructs/extensions including:

```text
// C++-style comments
#error
#eval
trailing comma in enum
trailing comma in initializer
C99 declaration forms
C99 designated initializers
C99 compound literals
inline
restrict
_Bool
_Complex
GNU-style extensions such as __attribute__, typeof, and asm
```

The test script distinguishes between constructs that are already rejected by the underlying compiler and extensions that required an explicit strict-mode change.

### Real ELF generation

The final integration test goes beyond syntax checking:

```text
C90 source
   -> LCC
   -> x86 assembly
   -> NASM -f elf32
   -> ELF32 object
   -> GNU linker
   -> Linux ELF executable
```

The generated file is inspected as an ELF executable so that the test validates the complete compiler/assembler/linker pipeline rather than only the parser.

---

## 5. Compiler-study workflow

LCC is particularly useful as a source-level compiler laboratory.

### A. CPP: C source -> preprocessed C

Source directory:

```text
cpp/
```

Important files:

```text
cpp/cpp.c
cpp/lex.c
cpp/macro.c
cpp/include.c
```

Study topics:

- preprocessing
- lexical processing
- `#include`
- macro expansion
- conditional compilation
- token handling
- preprocessing directives

A useful debugging pipeline is:

```text
hello.c
  |
  v
 cpp
  |
  v
hello.i
```

### B. RCC: preprocessed C -> assembly

Source directory:

```text
src/
```

Important files:

```text
src/main.c
src/lex.c
src/decl.c
src/expr.c
src/stmt.c
src/tree.c
src/dag.c
src/gen.c
```

Study topics:

- lexical analysis
- declarations and types
- expressions
- statements
- syntax processing
- tree/IR representation
- DAG construction
- optimization
- instruction selection
- assembly generation

For compiler-construction study, this is the main area to debug.

### C. Driver

The driver is under:

```text
etc/lcc.c
```

It coordinates preprocessing, compilation, assembly, and linking.

---

## 6. Recommended debugging order

Start from the outside and progressively move inward:

```text
1. cpp
   C source -> preprocessed C

2. rcc
   preprocessed C -> assembly

3. assembler/backend
   assembly -> object

4. linker
   object -> ELF executable
```

For understanding how a C compiler works internally, spend most of the source-level debugging time in `rcc`.

---

## 7. VS Code / GDB study

The repository can also be opened directly in VS Code:

```bash
code .
```

Install Microsoft's C/C++ extension if it is not already installed.

For debugging, useful starting points are:

### CPP

```text
cpp/cpp.c
cpp/lex.c
cpp/macro.c
cpp/include.c
```

### RCC

```text
src/main.c
src/lex.c
src/decl.c
src/expr.c
src/stmt.c
src/tree.c
src/gen.c
src/dag.c
```

### Driver

```text
etc/lcc.c
```

A practical RCC study flow is:

```text
hello.c
   |
   v
preprocess
   |
   v
hello.i
   |
   v
rcc
   |
   v
hello.s
```

Then set breakpoints in `src/main.c`, `src/lex.c`, `src/decl.c`, `src/expr.c`, `src/stmt.c`, `src/tree.c`, and `src/gen.c` and follow the source through parsing, semantic processing, IR construction, and code generation.

---

## 8. Relationship to ANSI C90

The important distinction is:

```text
LCC implementation source
        !=
set of language features accepted by LCC
```

LCC itself can be implemented using conforming C90-style source while still accepting additional language features for users. This project addresses the second issue by disabling the identified user-facing extensions.

Likewise, “strict ANSI C90” does not mean that every implementation detail has a single mandated value. C90 leaves some characteristics implementation-defined and permits implementation-defined translation limits.

The purpose here is therefore:

> Accept the ANSI C90 language/preprocessing model while rejecting the identified LCC-specific and later-standard language extensions.

---

## 9. Original LCC

This project is based on LCC 3.6, the retargetable C compiler described in:

*A Retargetable C Compiler: Design and Implementation*

by Christopher W. Fraser and David R. Hanson.

The original project is useful because the compiler is small enough to inspect as a complete implementation while still exposing the major stages of a real C compiler.

---

## 10. First compiler exercise

A useful first exercise is:

1. Run `./BUILD-UBUNTU24.sh`.
2. Compile a tiny C90 program.
3. Inspect the preprocessed output.
4. Inspect the generated assembly.
5. Inspect the generated ELF with `readelf`.
6. Set a breakpoint at `src/main.c`.
7. Follow tokenization through `gettok()`.
8. Continue into declaration, statement, and expression parsing.
9. Follow tree/IR construction.
10. Observe final code generation.

This turns LCC 3.6 into a practical ANSI C90 compiler-construction laboratory rather than treating it merely as a black-box compiler.


---

# Appendix: src/mips.md

%{
#define INTTMP 0x0100ff00
#define INTVAR 0x40ff0000
#define FLTTMP 0x000f0ff0
#define FLTVAR 0xfff00000

#define INTRET 0x00000004
#define FLTRET 0x00000003

#define readsreg(p) \
	(generic((p)->op)==INDIR && (p)->kids[0]->op==VREG+P)
#define setsrc(d) ((d) && (d)->x.regnode && \
	(d)->x.regnode->set == src->x.regnode->set && \
	(d)->x.regnode->mask&src->x.regnode->mask)

#define relink(a, b) ((b)->x.prev = (a), (a)->x.next = (b))

#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)
static void address     ARGS((Symbol, Symbol, int));
static void blkfetch    ARGS((int, int, int, int));
static void blkloop     ARGS((int, int, int, int, int, int[]));
static void blkstore    ARGS((int, int, int, int));
static void defaddress  ARGS((Symbol));
static void defconst    ARGS((int, Value));
static void defstring   ARGS((int, char *));
static void defsymbol   ARGS((Symbol));
static void doarg       ARGS((Node));
static void emit2       ARGS((Node));
static void export      ARGS((Symbol));
static void clobber     ARGS((Node));
static void function    ARGS((Symbol, Symbol [], Symbol [], int));
static void global      ARGS((Symbol));
static void import      ARGS((Symbol));
static void local       ARGS((Symbol));
static void progbeg     ARGS((int, char **));
static void progend     ARGS((void));
static void segment     ARGS((int));
static void space       ARGS((int));
static void target      ARGS((Node));
extern int      atoi            ARGS((const char *));
static int      bitcount        ARGS((unsigned));
static Symbol   argreg          ARGS((int, int, int, int));

static Symbol ireg[32], freg2[32], d6;
static int tmpregs[] = {3, 9, 10};
static Symbol blkreg;

static int gnum = 8;
static int pic;

static int cseg;
%}
%start stmt
%term ADDD=306 ADDF=305 ADDI=309 ADDP=311 ADDU=310
%term ADDRFP=279
%term ADDRGP=263
%term ADDRLP=295
%term ARGB=41 ARGD=34 ARGF=33 ARGI=37 ARGP=39
%term ASGNB=57 ASGNC=51 ASGND=50 ASGNF=49 ASGNI=53 ASGNP=55 ASGNS=52
%term BANDU=390
%term BCOMU=406
%term BORU=422
%term BXORU=438
%term CALLB=217 CALLD=210 CALLF=209 CALLI=213 CALLV=216
%term CNSTC=19 CNSTD=18 CNSTF=17 CNSTI=21 CNSTP=23 CNSTS=20 CNSTU=22
%term CVCI=85 CVCU=86
%term CVDF=97 CVDI=101
%term CVFD=114
%term CVIC=131 CVID=130 CVIS=132 CVIU=134
%term CVPU=150
%term CVSI=165 CVSU=166
%term CVUC=179 CVUI=181 CVUP=183 CVUS=180
%term DIVD=450 DIVF=449 DIVI=453 DIVU=454
%term EQD=482 EQF=481 EQI=485
%term GED=498 GEF=497 GEI=501 GEU=502
%term GTD=514 GTF=513 GTI=517 GTU=518
%term INDIRB=73 INDIRC=67 INDIRD=66 INDIRF=65 INDIRI=69 INDIRP=71 INDIRS=68
%term JUMPV=584
%term LABELV=600
%term LED=530 LEF=529 LEI=533 LEU=534
%term LOADB=233 LOADC=227 LOADD=226 LOADF=225 LOADI=229 LOADP=231 LOADS=228 LOADU=230
%term LSHI=341 LSHU=342
%term LTD=546 LTF=545 LTI=549 LTU=550
%term MODI=357 MODU=358
%term MULD=466 MULF=465 MULI=469 MULU=470
%term NED=562 NEF=561 NEI=565
%term NEGD=194 NEGF=193 NEGI=197
%term RETD=242 RETF=241 RETI=245
%term RSHI=373 RSHU=374
%term SUBD=322 SUBF=321 SUBI=325 SUBP=327 SUBU=326
%term VREGP=615
%%
reg:  INDIRC(VREGP)     "# read register\n"
reg:  INDIRD(VREGP)     "# read register\n"
reg:  INDIRF(VREGP)     "# read register\n"
reg:  INDIRI(VREGP)     "# read register\n"
reg:  INDIRP(VREGP)     "# read register\n"
reg:  INDIRS(VREGP)     "# read register\n"
stmt: ASGNC(VREGP,reg)  "# write register\n"
stmt: ASGND(VREGP,reg)  "# write register\n"
stmt: ASGNF(VREGP,reg)  "# write register\n"
stmt: ASGNI(VREGP,reg)  "# write register\n"
stmt: ASGNP(VREGP,reg)  "# write register\n"
stmt: ASGNS(VREGP,reg)  "# write register\n"
con: CNSTC  "%a"
con: CNSTI  "%a"
con: CNSTP  "%a"
con: CNSTS  "%a"
con: CNSTU  "%a"
stmt: reg  ""
reg: CVIU(reg)  "%0"  notarget(a)
reg: CVPU(reg)  "%0"  notarget(a)
reg: CVUI(reg)  "%0"  notarget(a)
reg: CVUP(reg)  "%0"  notarget(a)
acon: con     "%0"
acon: ADDRGP  "%a"
addr: ADDI(reg,acon)  "%1($%0)"
addr: ADDU(reg,acon)  "%1($%0)"
addr: ADDP(reg,acon)  "%1($%0)"
addr: acon  "%0"
addr: reg   "($%0)"
addr: ADDRFP  "%a+%F($sp)"
addr: ADDRLP  "%a+%F($sp)"
reg: addr  "la $%c,%0\n"  1
reg: CNSTC  "# reg\n"  range(a, 0, 0)
reg: CNSTS  "# reg\n"  range(a, 0, 0)
reg: CNSTI  "# reg\n"  range(a, 0, 0)
reg: CNSTU  "# reg\n"  range(a, 0, 0)
reg: CNSTP  "# reg\n"  range(a, 0, 0)
stmt: ASGNC(addr,reg)  "sb $%1,%0\n"  1
stmt: ASGNS(addr,reg)  "sh $%1,%0\n"  1
stmt: ASGNI(addr,reg)  "sw $%1,%0\n"  1
stmt: ASGNP(addr,reg)  "sw $%1,%0\n"  1
reg:  INDIRC(addr)     "lb $%c,%0\n"  1
reg:  INDIRS(addr)     "lh $%c,%0\n"  1
reg:  INDIRI(addr)     "lw $%c,%0\n"  1
reg:  INDIRP(addr)     "lw $%c,%0\n"  1
reg: CVCI(INDIRC(addr))  "lb $%c,%0\n"  1
reg: CVSI(INDIRS(addr))  "lh $%c,%0\n"  1
reg: CVCU(INDIRC(addr))  "lbu $%c,%0\n" 1
reg: CVSU(INDIRS(addr))  "lhu $%c,%0\n" 1
reg:  INDIRD(addr)     "l.d $f%c,%0\n"  1
reg:  INDIRF(addr)     "l.s $f%c,%0\n"  1
stmt: ASGND(addr,reg)  "s.d $f%1,%0\n"  1
stmt: ASGNF(addr,reg)  "s.s $f%1,%0\n"  1
reg: DIVI(reg,reg)  "div $%c,$%0,$%1\n"   1
reg: DIVU(reg,reg)  "divu $%c,$%0,$%1\n"  1
reg: MODI(reg,reg)  "rem $%c,$%0,$%1\n"   1
reg: MODU(reg,reg)  "remu $%c,$%0,$%1\n"  1
reg: MULI(reg,reg)  "mul $%c,$%0,$%1\n"   1
reg: MULU(reg,reg)  "mul $%c,$%0,$%1\n"   1
rc:  con            "%0"
rc:  reg            "$%0"

reg: ADDI(reg,rc)   "addu $%c,$%0,%1\n"  1
reg: ADDP(reg,rc)   "addu $%c,$%0,%1\n"  1
reg: ADDU(reg,rc)   "addu $%c,$%0,%1\n"  1
reg: BANDU(reg,rc)  "and $%c,$%0,%1\n"   1
reg: BORU(reg,rc)   "or $%c,$%0,%1\n"    1
reg: BXORU(reg,rc)  "xor $%c,$%0,%1\n"   1
reg: SUBI(reg,rc)   "subu $%c,$%0,%1\n"  1
reg: SUBP(reg,rc)   "subu $%c,$%0,%1\n"  1
reg: SUBU(reg,rc)   "subu $%c,$%0,%1\n"  1
rc5: CNSTI          "%a"                range(a,0,31)
rc5: reg            "$%0"

reg: LSHI(reg,rc5)  "sll $%c,$%0,%1\n"  1
reg: LSHU(reg,rc5)  "sll $%c,$%0,%1\n"  1
reg: RSHI(reg,rc5)  "sra $%c,$%0,%1\n"  1
reg: RSHU(reg,rc5)  "srl $%c,$%0,%1\n"  1
reg: BCOMU(reg)  "not $%c,$%0\n"   1
reg: NEGI(reg)   "negu $%c,$%0\n"  1
reg: LOADC(reg)  "move $%c,$%0\n"  move(a)
reg: LOADS(reg)  "move $%c,$%0\n"  move(a)
reg: LOADI(reg)  "move $%c,$%0\n"  move(a)
reg: LOADP(reg)  "move $%c,$%0\n"  move(a)
reg: LOADU(reg)  "move $%c,$%0\n"  move(a)
reg: ADDD(reg,reg)  "add.d $f%c,$f%0,$f%1\n"  1
reg: ADDF(reg,reg)  "add.s $f%c,$f%0,$f%1\n"  1
reg: DIVD(reg,reg)  "div.d $f%c,$f%0,$f%1\n"  1
reg: DIVF(reg,reg)  "div.s $f%c,$f%0,$f%1\n"  1
reg: MULD(reg,reg)  "mul.d $f%c,$f%0,$f%1\n"  1
reg: MULF(reg,reg)  "mul.s $f%c,$f%0,$f%1\n"  1
reg: SUBD(reg,reg)  "sub.d $f%c,$f%0,$f%1\n"  1
reg: SUBF(reg,reg)  "sub.s $f%c,$f%0,$f%1\n"  1
reg: LOADD(reg)     "mov.d $f%c,$f%0\n"       move(a)
reg: LOADF(reg)     "mov.s $f%c,$f%0\n"       move(a)
reg: NEGD(reg)      "neg.d $f%c,$f%0\n"       1
reg: NEGF(reg)      "neg.s $f%c,$f%0\n"       1
reg: CVCI(reg)  "sll $%c,$%0,24; sra $%c,$%c,24\n"  2
reg: CVSI(reg)  "sll $%c,$%0,16; sra $%c,$%c,16\n"  2
reg: CVCU(reg)  "and $%c,$%0,0xff\n"                1
reg: CVSU(reg)  "and $%c,$%0,0xffff\n"              1
reg: CVIC(reg)  "%0"  notarget(a)
reg: CVIS(reg)  "%0"  notarget(a)
reg: CVUC(reg)  "%0"  notarget(a)
reg: CVUS(reg)  "%0"  notarget(a)
reg: CVIC(reg)  "move $%c,$%0\n"  move(a)
reg: CVIS(reg)  "move $%c,$%0\n"  move(a)
reg: CVIU(reg)  "move $%c,$%0\n"  move(a)
reg: CVPU(reg)  "move $%c,$%0\n"  move(a)
reg: CVUC(reg)  "move $%c,$%0\n"  move(a)
reg: CVUI(reg)  "move $%c,$%0\n"  move(a)
reg: CVUP(reg)  "move $%c,$%0\n"  move(a)
reg: CVUS(reg)  "move $%c,$%0\n"  move(a)
reg: CVDF(reg)  "cvt.s.d $f%c,$f%0\n"  1
reg: CVFD(reg)  "cvt.d.s $f%c,$f%0\n"  1
reg: CVID(reg)  "mtc1 $%0,$f%c; cvt.d.w $f%c,$f%c\n"  2
reg: CVDI(reg)  "trunc.w.d $f2,$f%0,$%c; mfc1 $%c,$f2\n"  2
stmt: LABELV  "%a:\n"
stmt: JUMPV(acon)  "b %0\n"   1
stmt: JUMPV(reg)   ".cpadd $%0\nj $%0\n"  !pic
stmt: JUMPV(reg)   "j $%0\n"  pic
stmt: EQI(reg,reg)  "beq $%0,$%1,%a\n"   1
stmt: GEI(reg,reg)  "bge $%0,$%1,%a\n"   1
stmt: GEU(reg,reg)  "bgeu $%0,$%1,%a\n"  1
stmt: GTI(reg,reg)  "bgt $%0,$%1,%a\n"   1
stmt: GTU(reg,reg)  "bgtu $%0,$%1,%a\n"  1
stmt: LEI(reg,reg)  "ble $%0,$%1,%a\n"   1
stmt: LEU(reg,reg)  "bleu $%0,$%1,%a\n"  1
stmt: LTI(reg,reg)  "blt $%0,$%1,%a\n"   1
stmt: LTU(reg,reg)  "bltu $%0,$%1,%a\n"  1
stmt: NEI(reg,reg)  "bne $%0,$%1,%a\n"   1
stmt: EQD(reg,reg)  "c.eq.d $f%0,$f%1; bc1t %a\n"  2
stmt: EQF(reg,reg)  "c.eq.s $f%0,$f%1; bc1t %a\n"  2
stmt: LED(reg,reg)  "c.le.d $f%0,$f%1; bc1t %a\n"  2
stmt: LEF(reg,reg)  "c.le.s $f%0,$f%1; bc1t %a\n"  2
stmt: LTD(reg,reg)  "c.lt.d $f%0,$f%1; bc1t %a\n"  2
stmt: LTF(reg,reg)  "c.lt.s $f%0,$f%1; bc1t %a\n"  2
stmt: GED(reg,reg)  "c.lt.d $f%0,$f%1; bc1f %a\n"  2
stmt: GEF(reg,reg)  "c.lt.s $f%0,$f%1; bc1f %a\n"  2
stmt: GTD(reg,reg)  "c.le.d $f%0,$f%1; bc1f %a\n"  2
stmt: GTF(reg,reg)  "c.le.s $f%0,$f%1; bc1f %a\n"  2
stmt: NED(reg,reg)  "c.eq.d $f%0,$f%1; bc1f %a\n"  2
stmt: NEF(reg,reg)  "c.eq.s $f%0,$f%1; bc1f %a\n"  2
ar:   ADDRGP     "%a"

reg:  CALLD(ar)  "jal %0\n"  1
reg:  CALLF(ar)  "jal %0\n"  1
reg:  CALLI(ar)  "jal %0\n"  1
stmt: CALLV(ar)  "jal %0\n"  1
ar: reg    "$%0"
ar: CNSTP  "%a"   range(a, 0, 0x0fffffff)
stmt: RETD(reg)  "# ret\n"  1
stmt: RETF(reg)  "# ret\n"  1
stmt: RETI(reg)  "# ret\n"  1
stmt: ARGD(reg)  "# arg\n"  1
stmt: ARGF(reg)  "# arg\n"  1
stmt: ARGI(reg)  "# arg\n"  1
stmt: ARGP(reg)  "# arg\n"  1

stmt: ARGB(INDIRB(reg))       "# argb %0\n"      1
stmt: ASGNB(reg,INDIRB(reg))  "# asgnb %0 %1\n"  1
%%
static void progend(){}
static void progbeg(argc, argv) int argc; char *argv[]; {
	int i;

	{
		union {
			char c;
			int i;
		} u;
		u.i = 0;
		u.c = 1;
		swap = ((int)(u.i == 1)) != ((int)IR->little_endian);
	}
	print(".set reorder\n");
	pic = !IR->little_endian;
	parseflags(argc, argv);
	for (i = 0; i < argc; i++)
		if (strncmp(argv[i], "-G", 2) == 0)
			gnum = atoi(argv[i] + 2);
		else if (strcmp(argv[i], "-pic=1") == 0
		||       strcmp(argv[i], "-pic=0") == 0)
			pic = argv[i][5]-'0';
	for (i = 0; i < 31; i += 2)
		freg2[i] = mkreg("%d", i, 3, FREG);
	for (i = 0; i < 32; i++)
		ireg[i]  = mkreg("%d", i, 1, IREG);
	ireg[29]->x.name = "sp";
	d6 = mkreg("6", 6, 3, IREG);
	rmap[C] = rmap[S] = rmap[P] = rmap[B] = rmap[U] = rmap[I] =
		mkwildcard(ireg);
	rmap[F] = rmap[D] = mkwildcard(freg2);
	tmask[IREG] = INTTMP; tmask[FREG] = FLTTMP;
	vmask[IREG] = INTVAR; vmask[FREG] = FLTVAR;
	blkreg = mkreg("8", 8, 7, IREG);
}
static void target(p) Node p; {
	assert(p);
	switch (p->op) {
	case CNSTC: case CNSTI: case CNSTS: case CNSTU: case CNSTP:
		if (range(p, 0, 0) == 0) {
			setreg(p, ireg[0]);
			p->x.registered = 1;
		}
		break;
	case CALLV:             rtarget(p, 0, ireg[25]); break;
	case CALLD: case CALLF: rtarget(p, 0, ireg[25]);
	                        setreg(p, freg2[0]);     break;
	case CALLI:             rtarget(p, 0, ireg[25]);
	                        setreg(p, ireg[2]);      break;
	case RETD: case RETF:   rtarget(p, 0, freg2[0]); break;
	case RETI:              rtarget(p, 0, ireg[2]);  break;
	case ARGD: case ARGF: case ARGI: case ARGP: {
		static int ty0;
		int ty = optype(p->op);
		Symbol q;

		q = argreg(p->x.argno, p->syms[2]->u.c.v.i, ty, ty0);
		if (p->x.argno == 0)
			ty0 = ty;
		if (q &&
		!((ty == F || ty == D) && q->x.regnode->set == IREG))
			rtarget(p, 0, q);
		break;
		}
	case ASGNB: rtarget(p->kids[1], 0, blkreg); break;
	case ARGB:  rtarget(p->kids[0], 0, blkreg); break;
	}
}
static void clobber(p) Node p; {
	assert(p);
	switch (p->op) {
	case CALLD: case CALLF:
		spill(INTTMP | INTRET, IREG, p);
		spill(FLTTMP,          FREG, p);
		break;
	case CALLI:
		spill(INTTMP,          IREG, p);
		spill(FLTTMP | FLTRET, FREG, p);
		break;
	case CALLV:
		spill(INTTMP | INTRET, IREG, p);
		spill(FLTTMP | FLTRET, FREG, p);
		break;
	}
}
static void emit2(p) Node p; {
	int dst, n, src, ty;
	static int ty0;
	Symbol q;

	switch (p->op) {
	case ARGD: case ARGF: case ARGI: case ARGP:
		ty = optype(p->op);
		if (p->x.argno == 0)
			ty0 = ty;
		q = argreg(p->x.argno, p->syms[2]->u.c.v.i, ty, ty0);
		src = getregnum(p->x.kids[0]);
		if (q == NULL && ty == F)
			print("s.s $f%d,%d($sp)\n", src, p->syms[2]->u.c.v.i);
		else if (q == NULL && ty == D)
			print("s.d $f%d,%d($sp)\n", src, p->syms[2]->u.c.v.i);
		else if (q == NULL)
			print("sw $%d,%d($sp)\n", src, p->syms[2]->u.c.v.i);
		else if (ty == F && q->x.regnode->set == IREG)
			print("mfc1 $%d,$f%d\n", q->x.regnode->number, src);
		else if (ty == D && q->x.regnode->set == IREG)
			print("mfc1.d $%d,$f%d\n", q->x.regnode->number, src);
		break;
	case ASGNB:
		dalign = salign = p->syms[1]->u.c.v.i;
		blkcopy(getregnum(p->x.kids[0]), 0,
			getregnum(p->x.kids[1]), 0,
			p->syms[0]->u.c.v.i, tmpregs);
		break;
	case ARGB:
		dalign = 4;
		salign = p->syms[1]->u.c.v.i;
		blkcopy(29, p->syms[2]->u.c.v.i,
			getregnum(p->x.kids[0]), 0,
			p->syms[0]->u.c.v.i, tmpregs);
		n   = p->syms[2]->u.c.v.i + p->syms[0]->u.c.v.i;
		dst = p->syms[2]->u.c.v.i;
		for ( ; dst <= 12 && dst < n; dst += 4)
			print("lw $%d,%d($sp)\n", (dst/4)+4, dst);
		break;
	}
}
static Symbol argreg(argno, offset, ty, ty0)
int argno, offset, ty, ty0; {
	assert((offset&3) == 0);
	if (offset > 12)
		return NULL;
	else if (argno == 0 && (ty == F || ty == D))
		return freg2[12];
	else if (argno == 1 && (ty == F || ty == D)
	&& (ty0 == F || ty0 == D))
		return freg2[14];
	else if (argno == 1 && ty == D)
		return d6;  /* Pair! */
	else
		return ireg[(offset/4) + 4];
}
static void doarg(p) Node p; {
	static int argno;
	int size;

	if (argoffset == 0)
		argno = 0;
	p->x.argno = argno++;
	size = p->syms[1]->u.c.v.i < 4 ? 4 : p->syms[1]->u.c.v.i;
	p->syms[2] = intconst(mkactual(size,
		p->syms[0]->u.c.v.i));
}
static void local(p) Symbol p; {
	if (askregvar(p, rmap[ttob(p->type)]) == 0)
		mkauto(p);
}
static void function(f, caller, callee, ncalls)
Symbol f, callee[], caller[]; int ncalls; {
	int i, saved, sizefsave, sizeisave, varargs;
	Symbol r, argregs[4];

	usedmask[0] = usedmask[1] = 0;
	freemask[0] = freemask[1] = ~(unsigned)0;
	offset = maxoffset = maxargoffset = 0;
	for (i = 0; callee[i]; i++)
		;
	varargs = variadic(f->type)
		|| i > 0 && strcmp(callee[i-1]->name, "va_alist") == 0;
	for (i = 0; callee[i]; i++) {
		Symbol p = callee[i];
		Symbol q = caller[i];
		assert(q);
		offset = roundup(offset, q->type->align);
		p->x.offset = q->x.offset = offset;
		p->x.name = q->x.name = stringd(offset);
		r = argreg(i, offset, ttob(q->type), ttob(caller[0]->type));
		if (i < 4)
			argregs[i] = r;
		offset = roundup(offset + q->type->size, 4);
		if (varargs)
			p->sclass = AUTO;
		else if (r && ncalls == 0 &&
			 !isstruct(q->type) && !p->addressed &&
			 !(isfloat(q->type) && r->x.regnode->set == IREG)
) {
			p->sclass = q->sclass = REGISTER;
			askregvar(p, r);
			assert(p->x.regnode && p->x.regnode->vbl == p);
			q->x = p->x;
			q->type = p->type;
		}
		else if (askregvar(p, rmap[ttob(p->type)])
			 && r != NULL
			 && (isint(p->type) || p->type == q->type)) {
			assert(q->sclass != REGISTER);
			p->sclass = q->sclass = REGISTER;
			q->type = p->type;
		}
	}
	assert(!caller[i]);
	offset = 0;
	gencode(caller, callee);
	if (ncalls)
		usedmask[IREG] |= ((unsigned)1)<<31;
	usedmask[IREG] &= 0xc0ff0000;
	usedmask[FREG] &= 0xfff00000;
	if (pic && ncalls)
		usedmask[IREG] |= 1<<25;
	maxargoffset = roundup(maxargoffset, 4);
	if (ncalls && maxargoffset < 16)
		maxargoffset = 16;
	sizefsave = 4*bitcount(usedmask[FREG]);
	sizeisave = 4*bitcount(usedmask[IREG]);
	framesize = roundup(maxargoffset + sizefsave
		+ sizeisave + maxoffset, 8);
	segment(CODE);
	print(".align 2\n");
	print(".ent %s\n", f->x.name);
	print("%s:\n", f->x.name);
	i = maxargoffset + sizefsave - framesize;
	print(".frame $sp,%d,$31\n", framesize);
	if (pic)
		print(".set noreorder\n.cpload $25\n.set reorder\n");
	if (framesize > 0)
		print("addu $sp,$sp,%d\n", -framesize);
	if (usedmask[FREG])
		print(".fmask 0x%x,%d\n", usedmask[FREG], i - 8);
	if (usedmask[IREG])
		print(".mask 0x%x,%d\n",  usedmask[IREG],
			i + sizeisave - 4);
	saved = maxargoffset;
	for (i = 20; i <= 30; i += 2)
		if (usedmask[FREG]&(3<<i)) {
			print("s.d $f%d,%d($sp)\n", i, saved);
			saved += 8;
		}
	for (i = 16; i <= 31; i++)
		if (usedmask[IREG]&(1<<i)) {
			if (i == 25)
				print(".cprestore %d\n", saved);
			else
				print("sw $%d,%d($sp)\n", i, saved);
			saved += 4;
		}
	for (i = 0; i < 4 && callee[i]; i++) {
		r = argregs[i];
		if (r && r->x.regnode != callee[i]->x.regnode) {
			Symbol out = callee[i];
			Symbol in  = caller[i];
			int rn = r->x.regnode->number;
			int rs = r->x.regnode->set;
			int tyin = ttob(in->type);

			assert(out && in && r && r->x.regnode);
			assert(out->sclass != REGISTER || out->x.regnode);
			if (out->sclass == REGISTER
			&& (isint(out->type) || out->type == in->type)) {
				int outn = out->x.regnode->number;
				if (rs == FREG && tyin == D)
					print("mov.d $f%d,$f%d\n", outn, rn);
				else if (rs == FREG && tyin == F)
					print("mov.s $f%d,$f%d\n", outn, rn);
				else if (rs == IREG && tyin == D)
					print("mtc1.d $%d,$f%d\n", rn,   outn);
				else if (rs == IREG && tyin == F)
					print("mtc1 $%d,$f%d\n",   rn,   outn);
				else
					print("move $%d,$%d\n",    outn, rn);
			} else {
				int off = in->x.offset + framesize;
				if (rs == FREG && tyin == D)
					print("s.d $f%d,%d($sp)\n", rn, off);
				else if (rs == FREG && tyin == F)
					print("s.s $f%d,%d($sp)\n", rn, off);
				else {
					int i, n = (in->type->size + 3)/4;
					for (i = rn; i < rn+n && i <= 7; i++)
						print("sw $%d,%d($sp)\n", i, off + (i-rn)*4);
				}
			}
		}
	}
	if (varargs && callee[i-1]) {
		i = callee[i-1]->x.offset + callee[i-1]->type->size;
		for (i = roundup(i, 4)/4; i <= 3; i++)
			print("sw $%d,%d($sp)\n", i + 4, framesize + 4*i);
		}
	emitcode();
	saved = maxargoffset;
	for (i = 20; i <= 30; i += 2)
		if (usedmask[FREG]&(3<<i)) {
			print("l.d $f%d,%d($sp)\n", i, saved);
			saved += 8;
		}
	for (i = 16; i <= 31; i++)
		if (usedmask[IREG]&(1<<i)) {
			print("lw $%d,%d($sp)\n", i, saved);
			saved += 4;
		}
	if (framesize > 0)
		print("addu $sp,$sp,%d\n", framesize);
	print("j $31\n");
	print(".end %s\n", f->x.name);
}
static void defconst(ty, v) int ty; Value v; {
	switch (ty) {
	case C: print(".byte %d\n",   v.uc); return;
	case S: print(".half %d\n",   v.ss); return;
	case I: print(".word 0x%x\n", v.i);  return;
	case U: print(".word 0x%x\n", v.u);  return;
	case P: print(".word 0x%x\n", v.p); return;
	case F: print(".word 0x%x\n", *(unsigned *)&v.f); return;
	case D: {
		unsigned *p = (unsigned *)&v.d;
		print(".word 0x%x\n.word 0x%x\n", p[swap], p[!swap]);
		return;
		}
	}
	assert(0);
}
static void defaddress(p) Symbol p; {
	if (pic && p->scope == LABELS)
		print(".gpword %s\n", p->x.name);
	else
		print(".word %s\n", p->x.name);
}
static void defstring(n, str) int n; char *str; {
	char *s;

	for (s = str; s < str + n; s++)
		print(".byte %d\n", (*s)&0377);
}
static void export(p) Symbol p; {
	print(".globl %s\n", p->x.name);
}
static void import(p) Symbol p; {
	if (!isfunc(p->type))
		print(".extern %s %d\n", p->name, p->type->size);
}
static void defsymbol(p) Symbol p; {
	if (p->scope >= LOCAL && p->sclass == STATIC)
		p->x.name = stringf("L.%d", genlabel(1));
	else if (p->generated)
		p->x.name = stringf("L.%s", p->name);
	else
		assert(p->scope != CONSTANTS || isint(p->type) || isptr(p->type)),
		p->x.name = p->name;
}
static void address(q, p, n) Symbol q, p; int n; {
	q->x.offset = p->x.offset + n;
	if (p->scope == GLOBAL
	|| p->sclass == STATIC || p->sclass == EXTERN)
		q->x.name = stringf("%s%s%d", p->x.name,
			n >= 0 ? "+" : "", n);
	else
		q->x.name = stringd(q->x.offset);
}
static void global(p) Symbol p; {
	if (p->u.seg == BSS) {
		if (p->sclass == STATIC || Aflag >= 2)
			print(".lcomm %s,%d\n", p->x.name, p->type->size);
		else
			print( ".comm %s,%d\n", p->x.name, p->type->size);
	} else {
		if (p->u.seg == DATA
		&& (p->type->size == 0 || p->type->size > gnum))
			print(".data\n");
		else if (p->u.seg == DATA)
			print(".sdata\n");
		print(".align %c\n", ".01.2...3"[p->type->align]);
		print("%s:\n", p->x.name);
	}
}
static void segment(n) int n; {
	cseg = n;
	switch (n) {
	case CODE: print(".text\n");  break;
	case LIT:  print(".rdata\n"); break;
	}
}
static void space(n) int n; {
	if (cseg != BSS)
		print(".space %d\n", n);
}
static void blkloop(dreg, doff, sreg, soff, size, tmps)
int dreg, doff, sreg, soff, size, tmps[]; {
	int lab = genlabel(1);

	print("addu $%d,$%d,%d\n", sreg, sreg, size&~7);
	print("addu $%d,$%d,%d\n", tmps[2], dreg, size&~7);
	blkcopy(tmps[2], doff, sreg, soff, size&7, tmps);
	print("L.%d:\n", lab);
	print("addu $%d,$%d,%d\n", sreg, sreg, -8);
	print("addu $%d,$%d,%d\n", tmps[2], tmps[2], -8);
	blkcopy(tmps[2], doff, sreg, soff, 8, tmps);
	print("bltu $%d,$%d,L.%d\n", dreg, tmps[2], lab);
}
static void blkfetch(size, off, reg, tmp)
int size, off, reg, tmp; {
	assert(size == 1 || size == 2 || size == 4);
	if (size == 1)
		print("lbu $%d,%d($%d)\n",  tmp, off, reg);
	else if (salign >= size && size == 2)
		print("lhu $%d,%d($%d)\n",  tmp, off, reg);
	else if (salign >= size)
		print("lw $%d,%d($%d)\n",   tmp, off, reg);
	else if (size == 2)
		print("ulhu $%d,%d($%d)\n", tmp, off, reg);
	else
		print("ulw $%d,%d($%d)\n",  tmp, off, reg);
}
static void blkstore(size, off, reg, tmp)
int size, off, reg, tmp; {
	if (size == 1)
		print("sb $%d,%d($%d)\n",  tmp, off, reg);
	else if (dalign >= size && size == 2)
		print("sh $%d,%d($%d)\n",  tmp, off, reg);
	else if (dalign >= size)
		print("sw $%d,%d($%d)\n",  tmp, off, reg);
	else if (size == 2)
		print("ush $%d,%d($%d)\n", tmp, off, reg);
	else
		print("usw $%d,%d($%d)\n", tmp, off, reg);
}
static void stabinit ARGS((char *, int, char *[]));
static void stabline ARGS((Coordinate *));
static void stabsym ARGS((Symbol));

static char *currentfile;

static int bitcount(mask) unsigned mask; {
	unsigned i, n = 0;

	for (i = 1; i; i <<= 1)
		if (mask&i)
			n++;
	return n;
}

/* stabinit - initialize stab output */
static void stabinit(file, argc, argv) int argc; char *file, *argv[]; {
	if (file) {
		print(".file 2,\"%s\"\n", file);
		currentfile = file;
	}
}

/* stabline - emit stab entry for source coordinate *cp */
static void stabline(cp) Coordinate *cp; {
	if (cp->file && cp->file != currentfile) {
		print(".file 2,\"%s\"\n", cp->file);
		currentfile = cp->file;
	}
	print(".loc 2,%d\n", cp->y);
}

/* stabsym - output a stab entry for symbol p */
static void stabsym(p) Symbol p; {
	if (p == cfunc && IR->stabline)
		(*IR->stabline)(&p->src);
}
Interface mipsebIR = {
	1, 1, 0,  /* char */
	2, 2, 0,  /* short */
	4, 4, 0,  /* int */
	4, 4, 1,  /* float */
	8, 8, 1,  /* double */
	4, 4, 0,  /* T * */
	0, 1, 0,  /* struct */
	0,	/* little_endian */
	0,  /* mulops_calls */
	0,  /* wants_callb */
	1,  /* wants_argb */
	1,  /* left_to_right */
	0,  /* wants_dag */
address,
blockbeg,
blockend,
defaddress,
defconst,
defstring,
defsymbol,
emit,
export,
function,
gen,
global,
import,
local,
progbeg,
progend,
segment,
space,
	0, 0, 0, stabinit, stabline, stabsym, 0,
	{
		4,	/* max_unaligned_load */
		blkfetch, blkstore, blkloop,
		_label,
		_rule,
		_nts,
		_kids,
		_opname,
		_arity,
		_string,
		_templates,
		_isinstruction,
		_ntname,
		emit2,
		doarg,
		target,
		clobber,

	}
}, mipselIR = {
	1, 1, 0,  /* char */
	2, 2, 0,  /* short */
	4, 4, 0,  /* int */
	4, 4, 1,  /* float */
	8, 8, 1,  /* double */
	4, 4, 0,  /* T * */
	0, 1, 0,  /* struct */
	1,	/* little_endian */
	0,  /* mulops_calls */
	0,  /* wants_callb */
	1,  /* wants_argb */
	1,  /* left_to_right */
	0,  /* wants_dag */
address,
blockbeg,
blockend,
defaddress,
defconst,
defstring,
defsymbol,
emit,
export,
function,
gen,
global,
import,
local,
progbeg,
progend,
segment,
space,
	0, 0, 0, stabinit, stabline, stabsym, 0,
	{
		4,	/* max_unaligned_load */
		blkfetch, blkstore, blkloop,
		_label,
		_rule,
		_nts,
		_kids,
		_opname,
		_arity,
		_string,
		_templates,
		_isinstruction,
		_ntname,
		emit2,
		doarg,
		target,
		clobber,

	}
};


---

# Appendix: src/sparc.md

%{
#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)
static void address     ARGS((Symbol, Symbol, int));
static void blkfetch    ARGS((int, int, int, int));
static void blkloop     ARGS((int, int, int, int, int, int[]));
static void blkstore    ARGS((int, int, int, int));
static void defaddress  ARGS((Symbol));
static void defconst    ARGS((int, Value));
static void defstring   ARGS((int, char *));
static void defsymbol   ARGS((Symbol));
static void doarg       ARGS((Node));
static void emit2       ARGS((Node));
static void export      ARGS((Symbol));
static void clobber     ARGS((Node));
static void function    ARGS((Symbol, Symbol [], Symbol [], int));
static void global      ARGS((Symbol));
static void import      ARGS((Symbol));
static void local       ARGS((Symbol));
static void progbeg     ARGS((int, char **));
static void progend     ARGS((void));
static void segment     ARGS((int));
static void space       ARGS((int));
static void target      ARGS((Node));
static int imm ARGS((Node));
static void rename ARGS((void));
static void defsymbol2 ARGS((Symbol));
static void global2 ARGS((Symbol));
static void segment2 ARGS((int));

static Symbol greg[32];
static Symbol *oreg = &greg[8], *ireg = &greg[24];
static Symbol freg[32], freg2[32];

static int retstruct;

static int pflag = 0;

static int cseg;

%}
%start stmt
%term ADDD=306 ADDF=305 ADDI=309 ADDP=311 ADDU=310
%term ADDRFP=279
%term ADDRGP=263
%term ADDRLP=295
%term ARGB=41 ARGD=34 ARGF=33 ARGI=37 ARGP=39
%term ASGNB=57 ASGNC=51 ASGND=50 ASGNF=49 ASGNI=53 ASGNP=55 ASGNS=52
%term BANDU=390
%term BCOMU=406
%term BORU=422
%term BXORU=438
%term CALLB=217 CALLD=210 CALLF=209 CALLI=213 CALLV=216
%term CNSTC=19 CNSTD=18 CNSTF=17 CNSTI=21 CNSTP=23 CNSTS=20 CNSTU=22
%term CVCI=85 CVCU=86
%term CVDF=97 CVDI=101
%term CVFD=114
%term CVIC=131 CVID=130 CVIS=132 CVIU=134
%term CVPU=150
%term CVSI=165 CVSU=166
%term CVUC=179 CVUI=181 CVUP=183 CVUS=180
%term DIVD=450 DIVF=449 DIVI=453 DIVU=454
%term EQD=482 EQF=481 EQI=485
%term GED=498 GEF=497 GEI=501 GEU=502
%term GTD=514 GTF=513 GTI=517 GTU=518
%term INDIRB=73 INDIRC=67 INDIRD=66 INDIRF=65 INDIRI=69 INDIRP=71 INDIRS=68
%term JUMPV=584
%term LABELV=600
%term LED=530 LEF=529 LEI=533 LEU=534
%term LOADB=233 LOADC=227 LOADD=226 LOADF=225 LOADI=229 LOADP=231 LOADS=228 LOADU=230
%term LSHI=341 LSHU=342
%term LTD=546 LTF=545 LTI=549 LTU=550
%term MODI=357 MODU=358
%term MULD=466 MULF=465 MULI=469 MULU=470
%term NED=562 NEF=561 NEI=565
%term NEGD=194 NEGF=193 NEGI=197
%term RETD=242 RETF=241 RETI=245
%term RSHI=373 RSHU=374
%term SUBD=322 SUBF=321 SUBI=325 SUBP=327 SUBU=326
%term VREGP=615
%%
reg:  INDIRC(VREGP)     "# read register\n"
reg:  INDIRD(VREGP)     "# read register\n"
reg:  INDIRF(VREGP)     "# read register\n"
reg:  INDIRI(VREGP)     "# read register\n"
reg:  INDIRP(VREGP)     "# read register\n"
reg:  INDIRS(VREGP)     "# read register\n"
stmt: ASGNC(VREGP,reg)  "# write register\n"
stmt: ASGND(VREGP,reg)  "# write register\n"
stmt: ASGNF(VREGP,reg)  "# write register\n"
stmt: ASGNI(VREGP,reg)  "# write register\n"
stmt: ASGNP(VREGP,reg)  "# write register\n"
stmt: ASGNS(VREGP,reg)  "# write register\n"
con: CNSTC  "%a"
con: CNSTI  "%a"
con: CNSTP  "%a"
con: CNSTS  "%a"
con: CNSTU  "%a"
stmt: reg  ""
reg: CVIU(reg)  "%0"  notarget(a)
reg: CVPU(reg)  "%0"  notarget(a)
reg: CVUI(reg)  "%0"  notarget(a)
reg: CVUP(reg)  "%0"  notarget(a)
reg: ADDRGP  "set %a,%%%c\n"  1
stk13: ADDRFP  "%a"                  imm(a)
stk13: ADDRLP  "%a"                  imm(a)
reg:   stk13   "add %0,%%fp,%%%c\n"  1
stk: ADDRFP  "set %a,%%%c\n"                      2
stk: ADDRLP  "set %a,%%%c\n"                      2
reg: ADDRFP  "set %a,%%%c\nadd %%%c,%%fp,%%%c\n"  3
reg: ADDRLP  "set %a,%%%c\nadd %%%c,%%fp,%%%c\n"  3
con13: CNSTC  "%a"  imm(a)
con13: CNSTI  "%a"  imm(a)
con13: CNSTP  "%a"  imm(a)
con13: CNSTS  "%a"  imm(a)
con13: CNSTU  "%a"  imm(a)
base: ADDI(reg,con13)  "%%%0+%1"
base: ADDP(reg,con13)  "%%%0+%1"
base: ADDU(reg,con13)  "%%%0+%1"
base: reg    "%%%0"
base: con13  "%0"
base: stk13  "%%fp+%0"
addr: base           "%0"
addr: ADDI(reg,reg)  "%%%0+%%%1"
addr: ADDP(reg,reg)  "%%%0+%%%1"
addr: ADDU(reg,reg)  "%%%0+%%%1"
addr: stk            "%%fp+%%%0"
reg:  INDIRC(addr)     "ldsb [%0],%%%c\n"  1
reg:  INDIRS(addr)     "ldsh [%0],%%%c\n"  1
reg:  INDIRI(addr)     "ld [%0],%%%c\n"    1
reg:  INDIRP(addr)     "ld [%0],%%%c\n"    1
reg:  INDIRF(addr)     "ld [%0],%%f%c\n"   1
stmt: ASGNC(addr,reg)  "stb %%%1,[%0]\n"   1
stmt: ASGNS(addr,reg)  "sth %%%1,[%0]\n"   1
stmt: ASGNI(addr,reg)  "st %%%1,[%0]\n"    1
stmt: ASGNP(addr,reg)  "st %%%1,[%0]\n"    1
stmt: ASGNF(addr,reg)  "st %%f%1,[%0]\n"   1
addrl: ADDRLP            "%%%fp+%a"          imm(a)

reg:   INDIRD(addrl)     "ldd [%0],%%f%c\n"  1
stmt:  ASGND(addrl,reg)  "std %%f%1,[%0]\n"  1
reg:  INDIRD(base)     "ld2 [%0],%%f%c\n"  2
stmt: ASGND(base,reg)  "st2 %%f%1,[%0]\n"  2
spill:  ADDRLP          "%a" !imm(a)

stmt: ASGNC(spill,reg)  "set %0,%%g1\nstb %%%1,[%%fp+%%g1]\n"
stmt: ASGNS(spill,reg)  "set %0,%%g1\nsth %%%1,[%%fp+%%g1]\n"
stmt: ASGNI(spill,reg)  "set %0,%%g1\nst %%%1,[%%fp+%%g1]\n"
stmt: ASGNP(spill,reg)  "set %0,%%g1\nst %%%1,[%%fp+%%g1]\n"
stmt: ASGNF(spill,reg)  "set %0,%%g1\nst %%f%1,[%%fp+%%g1]\n"
stmt: ASGND(spill,reg)  "set %0,%%g1\nstd %%f%1,[%%fp+%%g1]\n"
reg: CVCI(INDIRC(addr))  "ldsb [%0],%%%c\n"  1
reg: CVSI(INDIRS(addr))  "ldsh [%0],%%%c\n"  1
reg: CVCU(INDIRC(addr))  "ldub [%0],%%%c\n"  1
reg: CVSU(INDIRS(addr))  "lduh [%0],%%%c\n"  1
reg: CVIC(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVIS(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVIU(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVPU(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVUC(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVUI(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVUP(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVUS(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CVIC(reg)  "%0"  notarget(a)
reg: CVIS(reg)  "%0"  notarget(a)
reg: CVUC(reg)  "%0"  notarget(a)
reg: CVUS(reg)  "%0"  notarget(a)
reg: LOADC(reg)  "mov %%%0,%%%c\n"  move(a)
reg: LOADI(reg)  "mov %%%0,%%%c\n"  move(a)
reg: LOADP(reg)  "mov %%%0,%%%c\n"  move(a)
reg: LOADS(reg)  "mov %%%0,%%%c\n"  move(a)
reg: LOADU(reg)  "mov %%%0,%%%c\n"  move(a)
reg: CNSTC  "# reg\n"  range(a, 0, 0)
reg: CNSTI  "# reg\n"  range(a, 0, 0)
reg: CNSTP  "# reg\n"  range(a, 0, 0)
reg: CNSTS  "# reg\n"  range(a, 0, 0)
reg: CNSTU  "# reg\n"  range(a, 0, 0)
reg: con  "set %0,%%%c\n"  1
rc: con13  "%0"
rc: reg    "%%%0"
reg: ADDI(reg,rc)   "add %%%0,%1,%%%c\n"  1
reg: ADDP(reg,rc)   "add %%%0,%1,%%%c\n"  1
reg: ADDU(reg,rc)   "add %%%0,%1,%%%c\n"  1
reg: BANDU(reg,rc)  "and %%%0,%1,%%%c\n"  1
reg: BORU(reg,rc)   "or %%%0,%1,%%%c\n"   1
reg: BXORU(reg,rc)  "xor %%%0,%1,%%%c\n"  1
reg: SUBI(reg,rc)   "sub %%%0,%1,%%%c\n"  1
reg: SUBP(reg,rc)   "sub %%%0,%1,%%%c\n"  1
reg: SUBU(reg,rc)   "sub %%%0,%1,%%%c\n"  1
rc5: CNSTI  "%a"    range(a, 0, 31)
rc5: reg    "%%%0"
reg: LSHI(reg,rc5)  "sll %%%0,%1,%%%c\n"  1
reg: LSHU(reg,rc5)  "sll %%%0,%1,%%%c\n"  1
reg: RSHI(reg,rc5)  "sra %%%0,%1,%%%c\n"  1
reg: RSHU(reg,rc5)  "srl %%%0,%1,%%%c\n"  1
reg: BANDU(reg,BCOMU(rc))  "andn %%%0,%1,%%%c\n"  1
reg: BORU(reg,BCOMU(rc))   "orn %%%0,%1,%%%c\n"   1
reg: BXORU(reg,BCOMU(rc))  "xnor %%%0,%1,%%%c\n"  1
reg: NEGI(reg)   "neg %%%0,%%%c\n"  1
reg: BCOMU(reg)  "not %%%0,%%%c\n"  1
reg: CVCI(reg)  "sll %%%0,24,%%%c; sra %%%c,24,%%%c\n"  2
reg: CVSI(reg)  "sll %%%0,16,%%%c; sra %%%c,16,%%%c\n"  2
reg: CVCU(reg)  "and %%%0,0xff,%%%c\n"                   1
reg: CVSU(reg)  "set 0xffff,%%g1; and %%%0,%%g1,%%%c\n"  2
addrg: ADDRGP        "%a"
stmt:  JUMPV(addrg)  "ba %0; nop\n"   2
stmt:  JUMPV(addr)   "jmp %0; nop\n"  2
stmt:  LABELV        "%a:\n"
stmt: EQI(reg,rc)  "cmp %%%0,%1; be %a; nop\n"    3
stmt: GEI(reg,rc)  "cmp %%%0,%1; bge %a; nop\n"   3
stmt: GEU(reg,rc)  "cmp %%%0,%1; bgeu %a; nop\n"  3
stmt: GTI(reg,rc)  "cmp %%%0,%1; bg %a; nop\n"    3
stmt: GTU(reg,rc)  "cmp %%%0,%1; bgu %a; nop\n"   3
stmt: LEI(reg,rc)  "cmp %%%0,%1; ble %a; nop\n"   3
stmt: LEU(reg,rc)  "cmp %%%0,%1; bleu %a; nop\n"  3
stmt: LTI(reg,rc)  "cmp %%%0,%1; bl %a; nop\n"    3
stmt: LTU(reg,rc)  "cmp %%%0,%1; blu %a; nop\n"   3
stmt: NEI(reg,rc)  "cmp %%%0,%1; bne %a; nop\n"   3
call: ADDRGP           "%a"
call: addr             "%0"
reg:  CALLD(call)      "call %0; nop\n"                2
reg:  CALLF(call)      "call %0; nop\n"                2
reg:  CALLI(call)      "call %0; nop\n"                2
stmt: CALLV(call)      "call %0; nop\n"                2
stmt: CALLB(call,reg)  "call %0; st %%%1,[%%sp+64]; unimp %b&0xfff\n"  3

stmt: RETD(reg)  "# ret\n"  1
stmt: RETF(reg)  "# ret\n"  1
stmt: RETI(reg)  "# ret\n"  1
stmt: ARGI(reg)  "st %%%0,[%%sp+4*%c+68]\n"  1
stmt: ARGP(reg)  "st %%%0,[%%sp+4*%c+68]\n"  1
stmt: ARGD(reg)  "# ARGD\n"  1
stmt: ARGF(reg)  "# ARGF\n"  1

reg: DIVI(reg,rc)   "sra %%%0,31,%%g1; wr %%g0,%%g1,%%y; nop; nop; nop; sdiv %%%0,%1,%%%c\n"       6

reg: DIVU(reg,rc)   "wr %%g0,%%g0,%%y; nop; nop; nop; udiv %%%0,%1,%%%c\n"       5

reg: MODI(reg,rc)   "sra %%%0,31,%%g1; wr %%g0,%%g1,%%y; nop; nop; nop; sdiv %%%0,%1,%%g1\n; smul %%g1,%1,%%g1; sub %%%0,%%g1,%%%c\n"  8


reg: MODU(reg,rc)   "wr %%g0,%%g0,%%y; nop; nop; nop; udiv %%%0,%1,%%g1\n; umul %%g1,%1,%%g1; sub %%%0,%%g1,%%%c\n"  7


reg: MULI(rc,reg)   "smul %%%1,%0,%%%c\n"  1
reg: MULU(rc,reg)   "umul %%%1,%0,%%%c\n"  1
reg: ADDD(reg,reg)  "faddd %%f%0,%%f%1,%%f%c\n"  1
reg: ADDF(reg,reg)  "fadds %%f%0,%%f%1,%%f%c\n"  1
reg: DIVD(reg,reg)  "fdivd %%f%0,%%f%1,%%f%c\n"  1
reg: DIVF(reg,reg)  "fdivs %%f%0,%%f%1,%%f%c\n"  1
reg: MULD(reg,reg)  "fmuld %%f%0,%%f%1,%%f%c\n"  1
reg: MULF(reg,reg)  "fmuls %%f%0,%%f%1,%%f%c\n"  1
reg: SUBD(reg,reg)  "fsubd %%f%0,%%f%1,%%f%c\n"  1
reg: SUBF(reg,reg)  "fsubs %%f%0,%%f%1,%%f%c\n"  1
reg: NEGF(reg)   "fnegs %%f%0,%%f%c\n"  1
reg: LOADF(reg)  "fmovs %%f%0,%%f%c\n"  1
reg: CVDF(reg)   "fdtos %%f%0,%%f%c\n"  1
reg: CVFD(reg)   "fstod %%f%0,%%f%c\n"  1
reg: CVDI(reg)  "fdtoi %%f%0,%%f0; st %%f0,[%%sp+64]; ld [%%sp+64],%%%c\n"  3

reg: CVID(reg)  "st %%%0,[%%sp+64]; ld [%%sp+64],%%f%c; fitod %%f%c,%%f%c\n"  3

rel: EQD(reg,reg)  "fcmped %%f%0,%%f%1; nop; fbue"
rel: EQF(reg,reg)  "fcmpes %%f%0,%%f%1; nop; fbue"
rel: GED(reg,reg)  "fcmped %%f%0,%%f%1; nop; fbuge"
rel: GEF(reg,reg)  "fcmpes %%f%0,%%f%1; nop; fbuge"
rel: GTD(reg,reg)  "fcmped %%f%0,%%f%1; nop; fbug"
rel: GTF(reg,reg)  "fcmpes %%f%0,%%f%1; nop; fbug"
rel: LED(reg,reg)  "fcmped %%f%0,%%f%1; nop; fbule"
rel: LEF(reg,reg)  "fcmpes %%f%0,%%f%1; nop; fbule"
rel: LTD(reg,reg)  "fcmped %%f%0,%%f%1; nop; fbul"
rel: LTF(reg,reg)  "fcmpes %%f%0,%%f%1; nop; fbul"
rel: NED(reg,reg)  "fcmped %%f%0,%%f%1; nop; fbne"
rel: NEF(reg,reg)  "fcmpes %%f%0,%%f%1; nop; fbne"

stmt: rel  "%0 %a; nop\n"  4
reg:  LOADD(reg)  "# LOADD\n"  2

reg:  NEGD(reg)  "# NEGD\n"  2

stmt:  ASGNB(reg,INDIRB(reg))  "# ASGNB\n"

%%
static void progend(){}
static void progbeg(argc, argv) int argc; char *argv[]; {
	int i;

	{
		union {
			char c;
			int i;
		} u;
		u.i = 0;
		u.c = 1;
		swap = ((int)(u.i == 1)) != ((int)IR->little_endian);
	}
	parseflags(argc, argv);
	for (i = 0; i < argc; i++)
		if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "-pg") == 0)
			pflag = 1;
	for (i = 0; i < 8; i++) {
		greg[i +  0] = mkreg(stringf("g%d", i), i +  0, 1, IREG);
		greg[i +  8] = mkreg(stringf("o%d", i), i +  8, 1, IREG);
		greg[i + 16] = mkreg(stringf("l%d", i), i + 16, 1, IREG);
		greg[i + 24] = mkreg(stringf("i%d", i), i + 24, 1, IREG);
	}
	for (i = 0; i < 32; i++)
		freg[i]  = mkreg("%d", i, 1, FREG);
	for (i = 0; i < 31; i += 2)
		freg2[i] = mkreg("%d", i, 3, FREG);
	rmap[C] = rmap[S] = rmap[P] = rmap[B] = rmap[U] = rmap[I] =
		mkwildcard(greg);
	rmap[F] = mkwildcard(freg);
	rmap[D] = mkwildcard(freg2);
	tmask[IREG] = 0x3fff3e00;
	vmask[IREG] = 0x3ff00000;
	tmask[FREG]  = ~(unsigned)0;
	vmask[FREG]  = 0;
}
static void target(p) Node p; {
	assert(p);
	switch (p->op) {
	case CNSTC: case CNSTI: case CNSTS: case CNSTU: case CNSTP:
		if (range(p, 0, 0) == 0) {
			setreg(p, greg[0]);
			p->x.registered = 1;
		}
		break;
	case CALLB:
		assert(p->syms[1] && p->syms[1]->type && isfunc(p->syms[1]->type));
		p->syms[1] = intconst(freturn(p->syms[1]->type)->size);
		break;
	case CALLD: setreg(p, freg2[0]);     break;
	case CALLF: setreg(p, freg[0]);      break;
	case CALLI:
	case CALLV: setreg(p, oreg[0]);      break;
	case RETD:  rtarget(p, 0, freg2[0]); break;
	case RETF:  rtarget(p, 0, freg[0]);  break;
	case RETI:
		rtarget(p, 0, ireg[0]);
		p->kids[0]->x.registered = 1;
		break;
	case ARGI: case ARGP:
		if (p->syms[RX]->u.c.v.i < 6) {
			rtarget(p, 0, oreg[p->syms[RX]->u.c.v.i]);
			p->op = LOAD+optype(p->op);
			setreg(p, oreg[p->syms[RX]->u.c.v.i]);
		}
		break;
	}
}
static void clobber(p) Node p; {
	assert(p);
	switch (p->op) {
	case CALLB: case CALLD: case CALLF: case CALLI:
		spill(~(unsigned)3, FREG, p);
		break;
	case CALLV:
		spill(oreg[0]->x.regnode->mask, IREG, p);
		spill(~(unsigned)3, FREG, p);
		break;
	case ARGF:
		if (p->syms[2]->u.c.v.i <= 6)
			spill((1<<(p->syms[2]->u.c.v.i + 8)), IREG, p);
		break;
	case ARGD:
		if (p->syms[2]->u.c.v.i <= 5)
			spill((3<<(p->syms[2]->u.c.v.i + 8))&0xff00, IREG, p);
		break;
	}
}
static int imm(p) Node p; {
	return range(p, -4096, 4095);
}
static void doarg(p) Node p; {
	assert(p && p->syms[0] && p->op != ARG+B);
	p->syms[RX] = intconst(mkactual(4,
		p->syms[0]->u.c.v.i)/4);
}
static void emit2(p) Node p; {
	switch (p->op) {
	case ARGF: {
		int n = p->syms[RX]->u.c.v.i;
		print("st %%f%d,[%%sp+4*%d+68]\n",
			getregnum(p->x.kids[0]), n);
		if (n <= 5)
			print("ld [%%sp+4*%d+68],%%o%d\n", n, n);
		break;
	}
	case ARGD: {
		int n = p->syms[RX]->u.c.v.i;
		int src = getregnum(p->x.kids[0]);
		print("st %%f%d,[%%sp+4*%d+68]\n", src, n);
		print("st %%f%d,[%%sp+4*%d+68]\n", src+1, n+1);
		if (n <= 5)
			print("ld [%%sp+4*%d+68],%%o%d\n", n, n);
		if (n <= 4)
			print("ld [%%sp+4*%d+68],%%o%d\n", n+1, n+1);
		break;
	}
	case LOADD: {
		int dst = getregnum(p);
		int src = getregnum(p->x.kids[0]);
		print("fmovs %%f%d,%%f%d; ", src,   dst);
		print("fmovs %%f%d,%%f%d\n", src+1, dst+1);
		break;
	}
	case NEGD: {
		int dst = getregnum(p);
		int src = getregnum(p->x.kids[0]);
		print("fnegs %%f%d,%%f%d; ", src,   dst);
		print("fmovs %%f%d,%%f%d\n", src+1, dst+1);
		break;
	}
	case ASGNB: {
		static int tmpregs[] = { 1, 2, 3 };
		dalign = salign = p->syms[1]->u.c.v.i;
		blkcopy(getregnum(p->x.kids[0]), 0,
		        getregnum(p->x.kids[1]), 0,
		        p->syms[0]->u.c.v.i, tmpregs);
		break;
	}
	}
}
static void local(p) Symbol p; {
	if (retstruct) {
		p->x.name = stringd(4*16);
		p->x.offset = 4*16;
		retstruct = 0;
		return;
	}
	if (isscalar(p->type) && !p->addressed && !isfloat(p->type))
		p->sclass = REGISTER;
	if (glevel && glevel != 3) p->sclass = AUTO;	/* glevel!=3 is for Norman's debugger */
	if (askregvar(p, rmap[ttob(p->type)]) == 0)
		mkauto(p);
}
static void function(f, caller, callee, ncalls)
Symbol f, callee[], caller[]; int ncalls; {
	int autos = 0, i, leaf, reg, varargs;

	for (i = 0; callee[i]; i++)
		;
	varargs = variadic(f->type)
		|| i > 0 && strcmp(callee[i-1]->name,
			"__builtin_va_alist") == 0;
usedmask[0] = usedmask[1] = 0;
freemask[0] = freemask[1] = ~(unsigned)0;
	for (i = 0; i < 8; i++)
		ireg[i]->x.regnode->vbl = NULL;
	offset = 68;
	maxargoffset = 24;
	reg = 0;
	for (i = 0; callee[i]; i++) {
		Symbol p = callee[i], q = caller[i];
		int size = roundup(q->type->size, 4);
		assert(q);
		if (isfloat(p->type) || reg >= 6) {
			p->x.offset = q->x.offset = offset;
			p->x.name = q->x.name = stringd(offset);
			p->sclass = q->sclass = AUTO;
			autos++;
		}
		else if (p->addressed || varargs)
			{
				p->x.offset = offset;
				p->x.name = stringd(p->x.offset);
				p->sclass = AUTO;
				q->sclass = REGISTER;
				askregvar(q, ireg[reg]);
				assert(q->x.regnode);
				autos++;
			}
		else if (glevel && glevel != 3) {
							p->x.offset = offset;
							p->x.name = stringd(p->x.offset);
							p->sclass = AUTO;
							q->sclass = REGISTER;
							askregvar(q, ireg[reg]);
							assert(q->x.regnode);
							autos++;
						}

		else {
			p->sclass = q->sclass = REGISTER;
			askregvar(p, ireg[reg]);
			assert(p->x.regnode);
			q->x.name = p->x.name;
		}
		offset += size;
		reg += isstruct(p->type) ? 1 : size/4;
	}
	assert(caller[i] == 0);
	offset = maxoffset = 0;
	retstruct = isstruct(freturn(f->type));
	gencode(caller, callee);
	maxargoffset = roundup(maxargoffset, 4);
	framesize = roundup(maxoffset + maxargoffset + 4*(16+1), 8);
	assert(!varargs || autos);
	leaf = (!ncalls
		&& !maxoffset && !autos
		&& !isstruct(freturn(f->type))
		&& !(usedmask[IREG]&0x00ffff01)
		&& !(usedmask[FREG]&~(unsigned)3)
		&& !pflag && !glevel);
	print(".align 4\n.proc 4\n%s:\n", f->x.name);
	if (leaf) {
		for (i = 0; caller[i] && callee[i]; i++) {
			Symbol p = caller[i], q = callee[i];
			if (p->sclass == REGISTER && q->sclass == REGISTER)
				assert(q->x.regnode),
				assert(q->x.regnode->set == IREG),
				assert(q->x.regnode->number >= 24),
				assert(q->x.regnode->number <= 31),
				p->x.name = greg[q->x.regnode->number - 16]->x.name;
			}
		rename();
	} else if (framesize <= 4095)
		print("save %%sp,%d,%%sp\n", -framesize);
	else
		print("set %d,%%g1; save %%sp,%%g1,%%sp\n", -framesize);
	if (varargs)
		for (; reg < 6; reg++)
			print("st %%i%d,[%%fp+%d]\n", reg, 4*reg + 68);
	else
		offset = 4*(16 + 1);
		reg = 0;
		for (i = 0; caller[i]; i++) {
			Symbol p = caller[i];
			if (isdouble(p->type) && reg <= 4) {
				print("st %%r%d,[%%fp+%d]\n",
					ireg[reg++]->x.regnode->number, offset);
				print("st %%r%d,[%%fp+%d]\n",
					ireg[reg++]->x.regnode->number, offset + 4);
			} else if (isfloat(p->type) && reg <= 5)
				print("st %%r%d,[%%fp+%d]\n",
					ireg[reg++]->x.regnode->number, offset);
			else
				reg++;
			offset += roundup(p->type->size, 4);
		}
if (pflag) {
	int lab = genlabel(1);
	print("set L%d,%%o0; call mcount; nop\n", lab);
	print(".seg \"data\"\n.align 4; L%d:.word 0\n.seg \"text\"\n", lab);
}
	emitcode();
	if (isstruct(freturn(f->type)))
		print("jmp %%i7+12; restore\n");
	else if (!leaf)
		print("ret; restore\n");
	else {
		rename();
		print("retl; nop\n");
	}
}
#define exch(x, y, t) (((t) = x), ((x) = (y)), ((y) = (t)))

static void rename() {
	int i;

	for (i = 0; i < 8; i++) {
		char *ptmp;
		int itmp;
		if (ireg[i]->x.regnode->vbl)
			ireg[i]->x.regnode->vbl->x.name = oreg[i]->x.name;
		exch(ireg[i]->x.name, oreg[i]->x.name, ptmp);
		exch(ireg[i]->x.regnode->number,
			oreg[i]->x.regnode->number, itmp);
	}
}
static void defconst(ty, v) int ty; Value v; {
	switch (ty) {
	case C: print(".byte %d\n",   v.uc); return;
	case S: print(".half %d\n",   v.ss); return;
	case I: print(".word %d\n",   v.i ); return;
	case U: print(".word 0x%x\n", v.u ); return;
	case P: print(".word 0x%x\n", v.p ); return;
	case F:
		print(".word 0x%x\n", *(unsigned *)&v.f);
		return;
case D: {
	unsigned *p = (unsigned *)&v.d;
	print(".word 0x%x\n.word 0x%x\n", p[swap], p[!swap]);
	return;
	}
	}
	assert(0);
}

static void defaddress(p) Symbol p; {
	print(".word %s\n", p->x.name);
}

static void defstring(n, str) int n; char *str; {
	char *s;

	for (s = str; s < str + n; s++)
		print(".byte %d\n", (*s)&0377);
}

static void address(q, p, n) Symbol q, p; int n; {
	q->x.offset = p->x.offset + n;
	if (p->scope == GLOBAL || p->sclass == STATIC || p->sclass == EXTERN)
		q->x.name = stringf("%s%s%d", p->x.name, n >= 0 ? "+" : "", n);
	else
		q->x.name = stringd(q->x.offset);
}
static void export(p) Symbol p; {
	print(".global %s\n", p->x.name);
}
static void import(p) Symbol p; {}
static void defsymbol(p) Symbol p; {
	if (p->scope >= LOCAL && p->sclass == STATIC)
		p->x.name = stringf("%d", genlabel(1));
	else
		assert(p->scope != CONSTANTS || isint(p->type) || isptr(p->type)),
		p->x.name = p->name;
	if (p->scope >= LABELS)
		p->x.name = stringf(p->generated ? "L%s" : "_%s",
			p->x.name);
}
static void segment(n) int n; {
	cseg = n;
	switch (n) {
	case CODE: print(".seg \"text\"\n"); break;
	case BSS:  print(".seg \"bss\"\n");  break;
	case DATA: print(".seg \"data\"\n"); break;
	case LIT:  print(".seg \"text\"\n"); break;
	}
}
static void space(n) int n; {
	if (cseg != BSS)
		print(".skip %d\n", n);
}
static void global(p) Symbol p; {
	print(".align %d\n", p->type->align);
	assert(p->u.seg);
	if (p->u.seg == BSS
	&& (p->sclass == STATIC || Aflag >= 2))
		print(".reserve %s,%d\n", p->x.name, p->type->size);
	else if (p->u.seg == BSS)
		print(".common %s,%d\n",  p->x.name, p->type->size);
	else
		print("%s:\n", p->x.name);
}
static void blkfetch(k, off, reg, tmp)
int k, off, reg, tmp; {
	assert(k == 1 || k == 2 || k == 4);
	assert(salign >= k);
	if (k == 1)
		print("ldub [%%r%d+%d],%%r%d\n", reg, off, tmp);
	else if (k == 2)
		print("lduh [%%r%d+%d],%%r%d\n", reg, off, tmp);
	else
		print("ld [%%r%d+%d],%%r%d\n",   reg, off, tmp);
}
static void blkstore(k, off, reg, tmp)
int k, off, reg, tmp; {
	assert(k == 1 || k == 2 || k == 4);
	assert(dalign >= k);
	if (k == 1)
		print("stb %%r%d,[%%r%d+%d]\n", tmp, reg, off);
	else if (k == 2)
		print("sth %%r%d,[%%r%d+%d]\n", tmp, reg, off);
	else
		print("st %%r%d,[%%r%d+%d]\n",  tmp, reg, off);
}
static void blkloop(dreg, doff, sreg, soff, size, tmps)
int dreg, doff, sreg, soff, size, tmps[]; {
	if ((size&~7) < 4096) {
		print("add %%r%d,%d,%%r%d\n", sreg, size&~7, sreg);
		print("add %%r%d,%d,%%r%d\n", dreg, size&~7, tmps[2]);
	} else {
		print("set %d,%%r%d\n", size&~7, tmps[2]);
		print("add %%r%d,%%r%d,%%r%d\n", sreg, tmps[2], sreg);
		print("add %%r%d,%%r%d,%%r%d\n", dreg, tmps[2], tmps[2]);
	}
	blkcopy(tmps[2], doff, sreg, soff, size&7, tmps);
	print("1: dec 8,%%r%d\n", tmps[2]);
	blkcopy(tmps[2], doff, sreg, soff - 8, 8, tmps);
	print("cmp %%r%d,%%r%d; ", tmps[2], dreg);
	print("bgt 1b; ");
	print("dec 8,%%r%d\n", sreg);
}
static void defsymbol2(p) Symbol p; {
	if (p->scope >= LOCAL && p->sclass == STATIC)
		p->x.name = stringf(".%d", genlabel(1));
	else
		assert(p->scope != CONSTANTS || isint(p->type) || isptr(p->type)),
		p->x.name = p->name;
	if (p->scope >= LABELS)
		p->x.name = stringf(p->generated ? ".L%s" : "%s",
			p->x.name);
}

static void global2(p) Symbol p; {
	assert(p->u.seg);
	if (p->sclass != STATIC && !p->generated)
		print(".type %s,#%s\n", p->x.name,
			isfunc(p->type) ? "function" : "object");
	if (p->u.seg == BSS && p->sclass == STATIC)
		print(".local %s\n.common %s,%d,%d\n", p->x.name, p->x.name,
			p->type->size, p->type->align);
	else if (p->u.seg == BSS && Aflag >= 2)
		print(".align %d\n%s:.skip %d\n", p->type->align, p->x.name,
			p->type->size);
	else if (p->u.seg == BSS)
		print(".common %s,%d,%d\n", p->x.name, p->type->size, p->type->align);
	else
		print(".align %d\n%s:\n", p->type->align, p->x.name);
}

static void segment2(n) int n; {
	cseg = n;
	switch (n) {
	case CODE: print(".section \".text\"\n");   break;
	case BSS:  print(".section \".bss\"\n");    break;
	case DATA: print(".section \".data\"\n");   break;
	case LIT:  print(".section \".rodata\"\n"); break;
	}
}
#ifdef sparc
#include <stab.h>
static char *currentfile;       /* current file name */
static int ntypes;
static int nextlab = 1;
extern Interface solarisIR;

static void stabblock ARGS((int, int, Symbol*));
static void stabfend ARGS((Symbol, int));
static void stabinit ARGS((char *, int, char *[]));
static void stabline ARGS((Coordinate *));
static void stabsym ARGS((Symbol));
static void stabtype ARGS((Symbol));

static void asgncode ARGS((Type, int));
static void dbxout ARGS((Type));
static int dbxtype ARGS((Type));
static int emittype ARGS((Type, int, int));

/* asgncode - assign type code to ty */
static void asgncode(ty, lev) Type ty; int lev; {
	if (ty->x.marked || ty->x.typeno)
		return;
	ty->x.marked = 1;
	switch (ty->op) {
	case VOLATILE: case CONST: case VOLATILE+CONST:
		asgncode(ty->type, lev);
		ty->x.typeno = ty->type->x.typeno;
		break;
	case POINTER: case FUNCTION: case ARRAY:
		asgncode(ty->type, lev + 1);
		/* fall thru */
	case VOID: case CHAR: case SHORT: case INT: case UNSIGNED:
	case FLOAT: case DOUBLE:
		break;
	case STRUCT: case UNION: {
		Field p;
		for (p = fieldlist(ty); p; p = p->link)
			asgncode(p->type, lev + 1);
		/* fall thru */
	case ENUM:
		if (ty->x.typeno == 0)
			ty->x.typeno = ++ntypes;
		if (lev > 0 && (*ty->u.sym->name < '0' || *ty->u.sym->name > '9'))
			dbxout(ty);
		break;
		}
	default:
		assert(0);
	}
}

/* dbxout - output .stabs entry for type ty */
static void dbxout(ty) Type ty; {
	ty = unqual(ty);
	if (!ty->x.printed) {
		int col = 0;
		print(".stabs \""), col += 8;
		if (ty->u.sym && !(isfunc(ty) || isarray(ty) || isptr(ty)))
			print("%s", ty->u.sym->name), col += strlen(ty->u.sym->name);
		print(":%c", isstruct(ty) || isenum(ty) ? 'T' : 't'), col += 2;
		emittype(ty, 0, col);
		print("\",%d,0,0,0\n", N_LSYM);
	}
}

/* dbxtype - emit a stabs entry for type ty, return type code */
static int dbxtype(ty) Type ty; {
	asgncode(ty, 0);
	dbxout(ty);
	return ty->x.typeno;
}

/*
 * emittype - emit ty's type number, emitting its definition if necessary.
 * Returns the output column number after emission; col is the approximate
 * output column before emission and is used to emit continuation lines for long
 * struct, union, and enum types. Continuations are not emitted for other types,
 * even if the definition is long. lev is the depth of calls to emittype.
 */
static int emittype(ty, lev, col) Type ty; int lev, col; {
	int tc = ty->x.typeno;

	if (isconst(ty) || isvolatile(ty)) {
		col = emittype(ty->type, lev, col);
		ty->x.typeno = ty->type->x.typeno;
		ty->x.printed = 1;
		return col;
	}
	if (tc == 0) {
		ty->x.typeno = tc = ++ntypes;
/*              fprint(2,"`%t'=%d\n", ty, tc); */
	}
	print("%d", tc), col += 3;
	if (ty->x.printed)
		return col;
	ty->x.printed = 1;
	switch (ty->op) {
	case VOID:	/* void is defined as itself */
		print("=%d", tc), col += 1+3;
		break;
	case CHAR:	/* unsigned char is a subrange of int */
		if (ty == unsignedchar)
			print("=r1;0;255;"), col += 10;
		else	/* following pcc, char is a subrange of itself */
			print("=r%d;-128;127;", tc), col += 2+3+10;
		break;
	case SHORT:	/* short is a subrange of int */
		if (ty == unsignedshort)
			print("=r1;0;65535;"), col += 12;
		else	/* signed */
			print("=r1;-32768;32767;"), col += 17;
		break;
	case INT:	/* int is a subrange of itself */
		print("=r1;%d;%d;", INT_MIN, INT_MAX), col += 4+11+1+10+1;
		break;
	case UNSIGNED:	/* unsigned is a subrange of int */
		print("=r1;0;-1;"), col += 9;
		break;
	case FLOAT: case DOUBLE:	/* float, double get sizes instead of ranges */
		print("=r1;%d;0;", ty->size), col += 4+1+3;
		break;
	case POINTER:
		print("=*"), col += 2;
		col = emittype(ty->type, lev + 1, col);
		break;
	case FUNCTION:
		print("=f"), col += 2;
		col = emittype(ty->type, lev + 1, col);
		break;
	case ARRAY:	/* array includes subscript as an int range */
		if (ty->size && ty->type->size)
			print("=ar1;0;%d;", ty->size/ty->type->size - 1), col += 7+3+1;
		else
			print("=ar1;0;-1;"), col += 10;
		col = emittype(ty->type, lev + 1, col);
		break;
	case STRUCT: case UNION: {
		Field p;
		if (!ty->u.sym->defined) {
			print("=x%c%s:", ty->op == STRUCT ? 's' : 'u', ty->u.sym->name);
			col += 2+1+strlen(ty->u.sym->name)+1;
			break;
		}
		if (lev > 0 && (*ty->u.sym->name < '0' || *ty->u.sym->name > '9')) {
			ty->x.printed = 0;
			break;
		}
		print("=%c%d", ty->op == STRUCT ? 's' : 'u', ty->size), col += 1+1+3;
		for (p = fieldlist(ty); p; p = p->link) {
			if (p->name)
				print("%s:", p->name), col += strlen(p->name)+1;
			else
				print(":"), col += 1;
			col = emittype(p->type, lev + 1, col);
			if (p->lsb)
				print(",%d,%d;", 8*p->offset +
					(IR->little_endian ? fieldright(p) : fieldleft(p)),
					fieldsize(p));
			else
				print(",%d,%d;", 8*p->offset, 8*p->type->size);
			col += 1+3+1+3+1;	/* accounts for ,%d,%d; */
			if (col >= 80 && p->link) {
				print("\\\\\",%d,0,0,0\n.stabs \"", N_LSYM);
				col = 8;
			}
		}
		print(";"), col += 1;
		break;
		}
	case ENUM: {
		Symbol *p;
		if (lev > 0 && (*ty->u.sym->name < '0' || *ty->u.sym->name > '9')) {
			ty->x.printed = 0;
			break;
		}
		print("=e"), col += 2;
		for (p = ty->u.sym->u.idlist; *p; p++) {
			print("%s:%d,", (*p)->name, (*p)->u.value), col += strlen((*p)->name)+3;
			if (col >= 80 && p[1]) {
				print("\\\\\",%d,0,0,0\n.stabs \"", N_LSYM);
				col = 8;
			}
		}
		print(";"), col += 1;
		break;
		}
	default:
		assert(0);
	}
	return col;
}

/* stabblock - output a stab entry for '{' or '}' at level lev */
static void stabblock(brace, lev, p) int brace, lev; Symbol *p; {
	if (brace == '{')
		while (*p)
			stabsym(*p++);
	if (IR == &solarisIR) {
		print(".stabn 0x%x,0,%d,.LL%d-%s\n", brace == '{' ? N_LBRAC : N_RBRAC, lev, nextlab, cfunc->x.name);
		print(".LL%d:\n", nextlab++);
	} else
		print(".stabd 0x%x,0,%d\n", brace == '{' ? N_LBRAC : N_RBRAC, lev);
}

/* stabfend - end of function p */
static void stabfend(p, line) Symbol p; int line; {
	if (IR == &solarisIR) {
		print(".type %s,#function\n", p->x.name);
		print(".size %s,.-%s\n", p->x.name, p->x.name);
	}
}

/* stabinit - initialize stab output */
static void stabinit(file, argc, argv) int argc; char *file, *argv[]; {
	typedef void (*Closure) ARGS((Symbol, void *));

	if (file && *file) {
		(*IR->segment)(CODE);
		print("Ltext:.stabs \"%s\",0x%x,0,0,Ltext\n", file, N_SO);
		currentfile = file;
	}
	dbxtype(inttype);
	dbxtype(chartype);
	dbxtype(doubletype);
	dbxtype(floattype);
	dbxtype(longdouble);
	dbxtype(longtype);
	dbxtype(shorttype);
	dbxtype(signedchar);
	dbxtype(unsignedchar);
	dbxtype(unsignedlong);
	dbxtype(unsignedshort);
	dbxtype(unsignedtype);
	dbxtype(voidtype);
	foreach(types, GLOBAL, (Closure)stabtype, NULL);
}

/* stabline - emit stab entry for source coordinate *cp */
static void stabline(cp) Coordinate *cp; {
	if (cp->file && cp->file != currentfile) {
		int lab = genlabel(1);
		print("L%d: .stabs \"%s\",0x%x,0,0,L%d\n", lab,
				cp->file, N_SOL, lab);
		currentfile = cp->file;
	}
	if (IR == &solarisIR) {
		print(".stabn 0x%x,0,%d,.LL%d-%s\n", N_SLINE, cp->y, nextlab, cfunc->x.name);
		print(".LL%d:\n", nextlab++);
	} else
		print(".stabd 0x%x,0,%d\n", N_SLINE, cp->y);
}

/* stabsym - output a stab entry for symbol p */
static void stabsym(p) Symbol p; {
	int code, tc, sz = p->type->size;

	if (p->generated || p->computed)
		return;
	if (isfunc(p->type)) {
		print(".stabs \"%s:%c%d\",%d,0,0,%s\n", p->name,
			p->sclass == STATIC ? 'f' : 'F', dbxtype(freturn(p->type)),
			N_FUN, p->x.name);
		return;
	}
	if (!IR->wants_argb && p->scope == PARAM && p->structarg) {
		assert(isptr(p->type) && isstruct(p->type->type));
		tc = dbxtype(p->type->type);
		sz = p->type->type->size;
	} else
		tc = dbxtype(p->type);
	if (p->sclass == AUTO && p->scope == GLOBAL || p->sclass == EXTERN) {
		print(".stabs \"%s:G", p->name);
		code = N_GSYM;
	} else if (p->sclass == STATIC) {
		print(".stabs \"%s:%c%d\",%d,0,0,%s\n", p->name, p->scope == GLOBAL ? 'S' : 'V',
			tc, p->u.seg == BSS ? N_LCSYM : N_STSYM, p->x.name);
		return;
	} else if (p->sclass == REGISTER) {
		if (p->scope > PARAM) {
			int r = p->x.regnode->number;
			if (p->x.regnode->set == FREG)
				r += 32;	/* floating point */
			print(".stabs \"%s:r%d\",%d,0,", p->name, tc, N_RSYM);
			print("%d,%d\n", sz, r);
		}
		return;
	} else if (p->scope == PARAM) {
		print(".stabs \"%s:p", p->name);
		code = N_PSYM;
	} else if (p->scope >= LOCAL) {
		print(".stabs \"%s:", p->name);
		code = N_LSYM;
	} else
		assert(0);
	print("%d\",%d,0,0,%s\n", tc, code,
		p->scope >= PARAM && p->sclass != EXTERN ? p->x.name : "0");
}

/* stabtype - output a stab entry for type *p */
static void stabtype(p) Symbol p; {
	if (p->type) {
		if (p->sclass == 0)
			dbxtype(p->type);
		else if (p->sclass == TYPEDEF)
			print(".stabs \"%s:t%d\",%d,0,0,0\n", p->name, dbxtype(p->type), N_LSYM);
	}
}
#else
#define stabblock 0
#define stabfend 0
#define stabinit 0
#define stabline 0
#define stabsym 0
#define stabtype 0
#endif
Interface sparcIR = {
	1, 1, 0,  /* char */
	2, 2, 0,  /* short */
	4, 4, 0,  /* int */
	4, 4, 1,  /* float */
	8, 8, 1,  /* double */
	4, 4, 0,  /* T * */
	0, 1, 0,  /* struct */
	0,  /* little_endian */
	0,  /* mulops_calls */
	1,  /* wants_callb */
	0,  /* wants_argb */
	1,  /* left_to_right */
	0,  /* wants_dag */
	address,
	blockbeg,
	blockend,
	defaddress,
	defconst,
	defstring,
	defsymbol,
	emit,
	export,
	function,
	gen,
	global,
	import,
	local,
	progbeg,
	progend,
	segment,
	space,
	stabblock, 0, 0, stabinit, stabline, stabsym, stabtype,
	{
		1,  /* max_unaligned_load */
		blkfetch, blkstore, blkloop,
		_label,
		_rule,
		_nts,
		_kids,
		_opname,
		_arity,
		_string,
		_templates,
		_isinstruction,
		_ntname,
		emit2,
		doarg,
		target,
		clobber,

	}
};

Interface solarisIR = {
	1, 1, 0,  /* char */
	2, 2, 0,  /* short */
	4, 4, 0,  /* int */
	4, 4, 1,  /* float */
	8, 8, 1,  /* double */
	4, 4, 0,  /* T * */
	0, 1, 0,  /* struct */
	0,	/* little_endian */
	0,	/* mulops_calls */
	1,	/* wants_callb */
	0,	/* wants_argb */
	1,	/* left_to_right */
	0,	/* wants_dag */
	address,
	blockbeg,
	blockend,
	defaddress,
	defconst,
	defstring,
	defsymbol2,
	emit,
	export,
	function,
	gen,
	global2,
	import,
	local,
	progbeg,
	progend,
	segment2,
	space,
	stabblock, 0, stabfend, stabinit, stabline, stabsym, stabtype,
	{
		1,	/* max_unaligned_load */
		blkfetch, blkstore, blkloop,
		_label,
		_rule,
		_nts,
		_kids,
		_opname,
		_arity,
		_string,
		_templates,
		_isinstruction,
		_ntname,
		emit2,
		doarg,
		target,
		clobber,

	}
};


---

# Appendix: src/x86.md

%{
enum { EAX=0, ECX=1, EDX=2, EBX=3, ESI=6, EDI=7 };
#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)
static void address     ARGS((Symbol, Symbol, int));
static void blkfetch    ARGS((int, int, int, int));
static void blkloop     ARGS((int, int, int, int, int, int[]));
static void blkstore    ARGS((int, int, int, int));
static void defaddress  ARGS((Symbol));
static void defconst    ARGS((int, Value));
static void defstring   ARGS((int, char *));
static void defsymbol   ARGS((Symbol));
static void doarg       ARGS((Node));
static void emit2       ARGS((Node));
static void export      ARGS((Symbol));
static void clobber     ARGS((Node));
static void function    ARGS((Symbol, Symbol [], Symbol [], int));
static void global      ARGS((Symbol));
static void import      ARGS((Symbol));
static void local       ARGS((Symbol));
static void progbeg     ARGS((int, char **));
static void progend     ARGS((void));
static void segment     ARGS((int));
static void space       ARGS((int));
static void target      ARGS((Node));
static int ckstack ARGS((Node, int));
static int memop ARGS((Node));
static int sametree ARGS((Node, Node));
static Symbol charreg[32], shortreg[32], intreg[32];
static Symbol fltreg[32];

static int cseg;

static Symbol quo, rem;

%}
%start stmt
%term ADDD=306 ADDF=305 ADDI=309 ADDP=311 ADDU=310
%term ADDRFP=279
%term ADDRGP=263
%term ADDRLP=295
%term ARGB=41 ARGD=34 ARGF=33 ARGI=37 ARGP=39
%term ASGNB=57 ASGNC=51 ASGND=50 ASGNF=49 ASGNI=53 ASGNP=55 ASGNS=52
%term BANDU=390
%term BCOMU=406
%term BORU=422
%term BXORU=438
%term CALLB=217 CALLD=210 CALLF=209 CALLI=213 CALLV=216
%term CNSTC=19 CNSTD=18 CNSTF=17 CNSTI=21 CNSTP=23 CNSTS=20 CNSTU=22
%term CVCI=85 CVCU=86
%term CVDF=97 CVDI=101
%term CVFD=114
%term CVIC=131 CVID=130 CVIS=132 CVIU=134
%term CVPU=150
%term CVSI=165 CVSU=166
%term CVUC=179 CVUI=181 CVUP=183 CVUS=180
%term DIVD=450 DIVF=449 DIVI=453 DIVU=454
%term EQD=482 EQF=481 EQI=485
%term GED=498 GEF=497 GEI=501 GEU=502
%term GTD=514 GTF=513 GTI=517 GTU=518
%term INDIRB=73 INDIRC=67 INDIRD=66 INDIRF=65 INDIRI=69 INDIRP=71 INDIRS=68
%term JUMPV=584
%term LABELV=600
%term LED=530 LEF=529 LEI=533 LEU=534
%term LOADB=233 LOADC=227 LOADD=226 LOADF=225 LOADI=229 LOADP=231 LOADS=228 LOADU=230
%term LSHI=341 LSHU=342
%term LTD=546 LTF=545 LTI=549 LTU=550
%term MODI=357 MODU=358
%term MULD=466 MULF=465 MULI=469 MULU=470
%term NED=562 NEF=561 NEI=565
%term NEGD=194 NEGF=193 NEGI=197
%term RETD=242 RETF=241 RETI=245
%term RSHI=373 RSHU=374
%term SUBD=322 SUBF=321 SUBI=325 SUBP=327 SUBU=326
%term VREGP=615
%%
reg:  INDIRC(VREGP)     "# read register\n"
reg:  INDIRD(VREGP)     "# read register\n"
reg:  INDIRF(VREGP)     "# read register\n"
reg:  INDIRI(VREGP)     "# read register\n"
reg:  INDIRP(VREGP)     "# read register\n"
reg:  INDIRS(VREGP)     "# read register\n"
stmt: ASGNC(VREGP,reg)  "# write register\n"
stmt: ASGND(VREGP,reg)  "# write register\n"
stmt: ASGNF(VREGP,reg)  "# write register\n"
stmt: ASGNI(VREGP,reg)  "# write register\n"
stmt: ASGNP(VREGP,reg)  "# write register\n"
stmt: ASGNS(VREGP,reg)  "# write register\n"
con: CNSTC  "%a"
con: CNSTI  "%a"
con: CNSTP  "%a"
con: CNSTS  "%a"
con: CNSTU  "%a"
stmt: reg  ""
reg: CVIU(reg)  "%0"  notarget(a)
reg: CVPU(reg)  "%0"  notarget(a)
reg: CVUI(reg)  "%0"  notarget(a)
reg: CVUP(reg)  "%0"  notarget(a)
acon: ADDRGP  "%a"
acon: con     "%0"
base: ADDRGP          "%a"
base: reg             "[%0]"
base: ADDI(reg,acon)  "%1[%0]"
base: ADDP(reg,acon)  "%1[%0]"
base: ADDU(reg,acon)  "%1[%0]"
base: ADDRFP  "%a[ebp]"
base: ADDRLP  "%a[ebp]"
index: reg "%0"
index: LSHI(reg,con1)  "%0*2"
index: LSHI(reg,con2)  "%0*4"
index: LSHI(reg,con3)  "%0*8"

con1:  CNSTI  "1"  range(a, 1, 1)
con1:  CNSTU  "1"  range(a, 1, 1)
con2:  CNSTI  "2"  range(a, 2, 2)
con2:  CNSTU  "2"  range(a, 2, 2)
con3:  CNSTI  "3"  range(a, 3, 3)
con3:  CNSTU  "3"  range(a, 3, 3)
index: LSHU(reg,con1)  "%0*2"
index: LSHU(reg,con2)  "%0*4"
index: LSHU(reg,con3)  "%0*8"
addr: base              "%0"
addr: ADDI(index,base)  "%1[%0]"
addr: ADDP(index,base)  "%1[%0]"
addr: ADDU(index,base)  "%1[%0]"
addr: index  "[%0]"
mem: INDIRC(addr)  "byte ptr %0"
mem: INDIRI(addr)  "dword ptr %0"
mem: INDIRP(addr)  "dword ptr %0"
mem: INDIRS(addr)  "word ptr %0"
rc:   reg  "%0"
rc:   con  "%0"

mr:   reg  "%0"
mr:   mem  "%0"

mrc0: mem  "%0"
mrc0: rc   "%0"
mrc1: mem  "%0"  1
mrc1: rc   "%0"

mrc3: mem  "%0"  3
mrc3: rc   "%0"
reg: addr        "lea %c,%0\n"  1
reg: mrc0        "mov %c,%0\n"  1
reg: LOADC(reg)  "mov %c,%0\n"  move(a)
reg: LOADI(reg)  "mov %c,%0\n"  move(a)
reg: LOADP(reg)  "mov %c,%0\n"  move(a)
reg: LOADS(reg)  "mov %c,%0\n"  move(a)
reg: LOADU(reg)  "mov %c,%0\n"  move(a)
reg: ADDI(reg,mrc1)  "?mov %c,%0\nadd %c,%1\n"  1
reg: ADDP(reg,mrc1)  "?mov %c,%0\nadd %c,%1\n"  1
reg: ADDU(reg,mrc1)  "?mov %c,%0\nadd %c,%1\n"  1
reg: SUBI(reg,mrc1)  "?mov %c,%0\nsub %c,%1\n"  1
reg: SUBP(reg,mrc1)  "?mov %c,%0\nsub %c,%1\n"  1
reg: SUBU(reg,mrc1)  "?mov %c,%0\nsub %c,%1\n"  1
reg: BANDU(reg,mrc1)  "?mov %c,%0\nand %c,%1\n"  1
reg: BORU(reg,mrc1)   "?mov %c,%0\nor %c,%1\n"   1
reg: BXORU(reg,mrc1)  "?mov %c,%0\nxor %c,%1\n"  1
stmt: ASGNI(addr,ADDI(mem,con1))  "inc %1\n"  memop(a)
stmt: ASGNI(addr,ADDU(mem,con1))  "inc %1\n"  memop(a)
stmt: ASGNP(addr,ADDP(mem,con1))  "inc %1\n"  memop(a)
stmt: ASGNI(addr,SUBI(mem,con1))  "dec %1\n"  memop(a)
stmt: ASGNI(addr,SUBU(mem,con1))  "dec %1\n"  memop(a)
stmt: ASGNP(addr,SUBP(mem,con1))  "dec %1\n"  memop(a)
stmt: ASGNI(addr,ADDI(mem,rc))   "add %1,%2\n"  memop(a)
stmt: ASGNI(addr,ADDU(mem,rc))   "add %1,%2\n"  memop(a)
stmt: ASGNI(addr,SUBI(mem,rc))   "sub %1,%2\n"  memop(a)
stmt: ASGNI(addr,SUBU(mem,rc))   "sub %1,%2\n"  memop(a)

stmt: ASGNI(addr,BANDU(mem,rc))  "and %1,%2\n"  memop(a)
stmt: ASGNI(addr,BORU(mem,rc))   "or %1,%2\n"   memop(a)
stmt: ASGNI(addr,BXORU(mem,rc))  "xor %1,%2\n"  memop(a)
reg: BCOMU(reg)  "?mov %c,%0\nnot %c\n"  2
reg: NEGI(reg)   "?mov %c,%0\nneg %c\n"  2

stmt: ASGNI(addr,BCOMU(mem))  "not %1\n"  memop(a)
stmt: ASGNI(addr,NEGI(mem))   "neg %1\n"  memop(a)
reg: LSHI(reg,rc5)  "?mov %c,%0\nsal %c,%1\n"  2
reg: LSHU(reg,rc5)  "?mov %c,%0\nshl %c,%1\n"  2
reg: RSHI(reg,rc5)  "?mov %c,%0\nsar %c,%1\n"  2
reg: RSHU(reg,rc5)  "?mov %c,%0\nshr %c,%1\n"  2

stmt: ASGNI(addr,LSHI(mem,rc5))  "sal %1,%2\n"  memop(a)
stmt: ASGNI(addr,LSHU(mem,rc5))  "shl %1,%2\n"  memop(a)
stmt: ASGNI(addr,RSHI(mem,rc5))  "sar %1,%2\n"  memop(a)
stmt: ASGNI(addr,RSHU(mem,rc5))  "shr %1,%2\n"  memop(a)

rc5: CNSTI  "%a"  range(a, 0, 31)
rc5: reg    "cl"
reg: MULI(reg,mrc3)  "?mov %c,%0\nimul %c,%1\n"  14
reg: MULI(con,mr)    "imul %c,%1,%0\n"  13
reg: MULU(reg,mr)  "mul %1\n"  13
reg: DIVU(reg,reg)  "xor edx,edx\ndiv %1\n"
reg: MODU(reg,reg)  "xor edx,edx\ndiv %1\n"
reg: DIVI(reg,reg)  "cdq\nidiv %1\n"
reg: MODI(reg,reg)  "cdq\nidiv %1\n"
reg: CVIU(reg)  "mov %c,%0\n"  move(a)
reg: CVPU(reg)  "mov %c,%0\n"  move(a)
reg: CVUI(reg)  "mov %c,%0\n"  move(a)
reg: CVUP(reg)  "mov %c,%0\n"  move(a)
reg: CVCI(INDIRC(addr))  "movsx %c,byte ptr %0\n"  3
reg: CVCU(INDIRC(addr))  "movzx %c,byte ptr %0\n"  3
reg: CVSI(INDIRS(addr))  "movsx %c,word ptr %0\n"  3
reg: CVSU(INDIRS(addr))  "movzx %c,word ptr %0\n"  3
reg: CVCI(reg)  "# extend\n"  3
reg: CVCU(reg)  "# extend\n"  3
reg: CVSI(reg)  "# extend\n"  3
reg: CVSU(reg)  "# extend\n"  3

reg: CVIC(reg)  "# truncate\n"  1
reg: CVIS(reg)  "# truncate\n"  1
reg: CVUC(reg)  "# truncate\n"  1
reg: CVUS(reg)  "# truncate\n"  1
stmt: ASGNC(addr,rc)  "mov byte ptr %0,%1\n"   1
stmt: ASGNI(addr,rc)  "mov dword ptr %0,%1\n"  1
stmt: ASGNP(addr,rc)  "mov dword ptr %0,%1\n"  1
stmt: ASGNS(addr,rc)  "mov word ptr %0,%1\n"   1
stmt: ARGI(mrc3)  "push %0\n"  1
stmt: ARGP(mrc3)  "push %0\n"  1
stmt: ASGNB(reg,INDIRB(reg))  "mov ecx,%a\nrep movsb\n"
stmt: ARGB(INDIRB(reg))  "sub esp,%a\nmov edi,esp\nmov ecx,%a\nrep movsb\n"

memf: INDIRD(addr)        "qword ptr %0"
memf: INDIRF(addr)        "dword ptr %0"
memf: CVFD(INDIRF(addr))  "dword ptr %0"
reg: memf  "fld %0\n"  3
stmt: ASGND(addr,reg)        "fstp qword ptr %0\n"  7
stmt: ASGNF(addr,reg)        "fstp dword ptr %0\n"  7
stmt: ASGNF(addr,CVDF(reg))  "fstp dword ptr %0\n"  7
stmt: ARGD(reg)  "sub esp,8\nfstp qword ptr [esp]\n"
stmt: ARGF(reg)  "sub esp,4\nfstp dword ptr [esp]\n"
reg: NEGD(reg)  "fchs\n"
reg: NEGF(reg)  "fchs\n"
flt: memf  " %0"
flt: reg   "p st(1),st"
reg: ADDD(reg,flt)  "fadd%1\n"
reg: ADDF(reg,flt)  "fadd%1\n"
reg: DIVD(reg,flt)  "fdiv%1\n"
reg: DIVF(reg,flt)  "fdiv%1\n"
reg: MULD(reg,flt)  "fmul%1\n"
reg: MULF(reg,flt)  "fmul%1\n"
reg: SUBD(reg,flt)  "fsub%1\n"
reg: SUBF(reg,flt)  "fsub%1\n"
reg: CVFD(reg)  "# CVFD\n"
reg: CVDF(reg)  "sub esp,4\nfstp dword ptr 0[esp]\nfld dword ptr 0[esp]\nadd esp,4\n"  12

stmt: ASGNI(addr,CVDI(reg))  "fistp dword ptr %0\n"  29
reg: CVDI(reg)  "sub esp,4\nfistp dword ptr 0[esp]\npop %c\n" 31

reg: CVID(INDIRI(addr))  "fild dword ptr %0\n"  10
reg: CVID(reg)  "push %0\nfild dword ptr 0[esp]\nadd esp,4\n"  12

addrj: ADDRGP  "%a"
addrj: reg     "%0"  2
addrj: mem     "%0"  2

stmt:  JUMPV(addrj)  "jmp %0\n"  3
stmt:  LABELV        "%a:\n"
stmt: EQI(mem,rc)  "cmp %0,%1\nje %a\n"   5
stmt: GEI(mem,rc)  "cmp %0,%1\njge %a\n"  5
stmt: GTI(mem,rc)  "cmp %0,%1\njg %a\n"   5
stmt: LEI(mem,rc)  "cmp %0,%1\njle %a\n"  5
stmt: LTI(mem,rc)  "cmp %0,%1\njl %a\n"   5
stmt: NEI(mem,rc)  "cmp %0,%1\njne %a\n"  5
stmt: GEU(mem,rc)  "cmp %0,%1\njae %a\n"  5
stmt: GTU(mem,rc)  "cmp %0,%1\nja  %a\n"  5
stmt: LEU(mem,rc)  "cmp %0,%1\njbe %a\n"  5
stmt: LTU(mem,rc)  "cmp %0,%1\njb  %a\n"  5
stmt: EQI(reg,mrc1)  "cmp %0,%1\nje %a\n"   4
stmt: GEI(reg,mrc1)  "cmp %0,%1\njge %a\n"  4
stmt: GTI(reg,mrc1)  "cmp %0,%1\njg %a\n"   4
stmt: LEI(reg,mrc1)  "cmp %0,%1\njle %a\n"  4
stmt: LTI(reg,mrc1)  "cmp %0,%1\njl %a\n"   4
stmt: NEI(reg,mrc1)  "cmp %0,%1\njne %a\n"  4

stmt: GEU(reg,mrc1)  "cmp %0,%1\njae %a\n"  4
stmt: GTU(reg,mrc1)  "cmp %0,%1\nja %a\n"   4
stmt: LEU(reg,mrc1)  "cmp %0,%1\njbe %a\n"  4
stmt: LTU(reg,mrc1)  "cmp %0,%1\njb %a\n"   4
cmpf: memf  " %0"
cmpf: reg   "p"
stmt: EQD(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\nje %a\n"
stmt: GED(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njbe %a\n"
stmt: GTD(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njb %a\n"
stmt: LED(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njae %a\n"
stmt: LTD(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\nja %a\n"
stmt: NED(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njne %a\n"

stmt: EQF(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\nje %a\n"
stmt: GEF(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njbe %a\n"
stmt: GTF(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njb %a\n"
stmt: LEF(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njae %a\n"
stmt: LTF(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\nja %a\n"
stmt: NEF(cmpf,reg)  "fcomp%0\nfstsw ax\nsahf\njne %a\n"
reg:  CALLI(addrj)  "call %0\nadd esp,%a\n"
stmt: CALLV(addrj)  "call %0\nadd esp,%a\n"
reg: CALLF(addrj)  "call %0\nadd esp,%a\n"
reg: CALLD(addrj)  "call %0\nadd esp,%a\n"

stmt: RETI(reg)  "# ret\n"
stmt: RETF(reg)  "# ret\n"
stmt: RETD(reg)  "# ret\n"
%%
static void progbeg(argc, argv) int argc; char *argv[]; {
	int i;

	{
		union {
			char c;
			int i;
		} u;
		u.i = 0;
		u.c = 1;
		swap = ((int)(u.i == 1)) != ((int)IR->little_endian);
	}
	parseflags(argc, argv);
	intreg[EAX] = mkreg("eax", EAX, 1, IREG);
	intreg[EDX] = mkreg("edx", EDX, 1, IREG);
	intreg[ECX] = mkreg("ecx", ECX, 1, IREG);
	intreg[EBX] = mkreg("ebx", EBX, 1, IREG);
	intreg[ESI] = mkreg("esi", ESI, 1, IREG);
	intreg[EDI] = mkreg("edi", EDI, 1, IREG);
	shortreg[EAX] = mkreg("ax", EAX, 1, IREG);
	shortreg[ECX] = mkreg("cx", ECX, 1, IREG);
	shortreg[EDX] = mkreg("dx", EDX, 1, IREG);
	shortreg[EBX] = mkreg("bx", EBX, 1, IREG);
	shortreg[ESI] = mkreg("si", ESI, 1, IREG);
	shortreg[EDI] = mkreg("di", EDI, 1, IREG);

	charreg[EAX]  = mkreg("al", EAX, 1, IREG);
	charreg[ECX]  = mkreg("cl", ECX, 1, IREG);
	charreg[EDX]  = mkreg("dl", EDX, 1, IREG);
	charreg[EBX]  = mkreg("bl", EBX, 1, IREG);
	for (i = 0; i < 8; i++)
		fltreg[i] = mkreg("%d", i, 0, FREG);
	rmap[C] = mkwildcard(charreg);
	rmap[S] = mkwildcard(shortreg);
	rmap[P] = rmap[B] = rmap[U] = rmap[I] = mkwildcard(intreg);
	rmap[F] = rmap[D] = mkwildcard(fltreg);
	tmask[IREG] = (1<<EDI) | (1<<ESI) | (1<<EBX)
	            | (1<<EDX) | (1<<ECX) | (1<<EAX);
	vmask[IREG] = 0;
	tmask[FREG] = 0xff;
	vmask[FREG] = 0;
	print(".486\n");
	print(".model small\n");
	print("extrn __turboFloat:near\n");
	print("extrn __setargv:near\n");
	cseg = 0;
	quo = mkreg("eax", EAX, 1, IREG);
	quo->x.regnode->mask |= 1<<EDX;
	rem = mkreg("edx", EDX, 1, IREG);
	rem->x.regnode->mask |= 1<<EAX;
}
static void segment(n) int n; {
	if (n == cseg)
		return;
	if (cseg == CODE)
		print("_TEXT ends\n");
	else if (cseg == DATA || cseg == BSS || cseg == LIT)
		print("_DATA ends\n");
	cseg = n;
	if (cseg == CODE)
		print("_TEXT segment\n");
	else if (cseg == DATA || cseg == BSS || cseg == LIT)
		print("_DATA segment\n");
}
static void progend() {
	segment(0);
	print("end\n");
}
static void target(p) Node p; {
	assert(p);
	switch (p->op) {
	case RSHI: case RSHU: case LSHI: case LSHU:
		if (generic(p->kids[1]->op) != CNST
		&& !(   generic(p->kids[1]->op) == INDIR
		     && p->kids[1]->kids[0]->op == VREG+P
		     && p->kids[1]->syms[RX]->u.t.cse
		     && generic(p->kids[1]->syms[RX]->u.t.cse->op) == CNST
)) {
			rtarget(p, 1, intreg[ECX]);
			setreg(p, intreg[EAX]);
		}
		break;
	case MULU:
		setreg(p, quo);
		rtarget(p, 0, intreg[EAX]);
		break;
	case DIVI: case DIVU:
		setreg(p, quo);
		rtarget(p, 0, intreg[EAX]);
		rtarget(p, 1, intreg[ECX]);
		break;
	case MODI: case MODU:
		setreg(p, rem);
		rtarget(p, 0, intreg[EAX]);
		rtarget(p, 1, intreg[ECX]);
		break;
	case ASGNB:
		rtarget(p, 0, intreg[EDI]);
		rtarget(p->kids[1], 0, intreg[ESI]);
		break;
	case ARGB:
		rtarget(p->kids[0], 0, intreg[ESI]);
		break;
	case CALLI: case CALLV:
		setreg(p, intreg[EAX]);
		break;
	case RETI:
		rtarget(p, 0, intreg[EAX]);
		break;
	}
}

static void clobber(p) Node p; {
	static int nstack = 0;

	assert(p);
	nstack = ckstack(p, nstack);
	switch (p->op) {
	case ASGNB: case ARGB:
		spill(1<<ECX | 1<<ESI | 1<<EDI, IREG, p);
		break;
	case EQD: case LED: case GED: case LTD: case GTD: case NED:
	case EQF: case LEF: case GEF: case LTF: case GTF: case NEF:
		spill(1<<EAX, IREG, p);
		break;
	case CALLD: case CALLF:
		spill(1<<EDX | 1<<EAX, IREG, p);
		break;
	}
}
#define isfp(p) (optype((p)->op)==F || optype((p)->op)==D)

static int ckstack(p, n) Node p; int n; {
	int i;

	for (i = 0; i < NELEMS(p->x.kids) && p->x.kids[i]; i++)
		if (isfp(p->x.kids[i]))
			n--;
	if (isfp(p) && p->count > 0)
		n++;
	if (n > 8)
		error("expression too complicated\n");
	debug(fprint(2, "(ckstack(%x)=%d)\n", p, n));
	assert(n >= 0);
	return n;
}
static int memop(p) Node p; {
	assert(p);
	assert(generic(p->op) == ASGN);
	assert(p->kids[0]);
	assert(p->kids[1]);
	if (generic(p->kids[1]->kids[0]->op) == INDIR
	&& sametree(p->kids[0], p->kids[1]->kids[0]->kids[0]))
		return 3;
	else
		return LBURG_MAX;
}
static int sametree(p, q) Node p, q; {
	return p == NULL && q == NULL
	|| p && q && p->op == q->op && p->syms[0] == q->syms[0]
		&& sametree(p->kids[0], q->kids[0])
		&& sametree(p->kids[1], q->kids[1]);
}
static void emit2(p) Node p; {
#define preg(f) ((f)[getregnum(p->x.kids[0])]->x.name)

	if (p->op == CVCI)
		print("movsx %s,%s\n", p->syms[RX]->x.name
, preg(charreg));
	else if (p->op == CVCU)
		print("movzx %s,%s\n", p->syms[RX]->x.name
, preg(charreg));
	else if (p->op == CVSI)
		print("movsx %s,%s\n", p->syms[RX]->x.name
, preg(shortreg));
	else if (p->op == CVSU)
		print("movzx %s,%s\n", p->syms[RX]->x.name
, preg(shortreg));
	else if (p->op == CVIC || p->op == CVIS
	      || p->op == CVUC || p->op == CVUS) {
		char *dst = shortreg[getregnum(p)]->x.name;
		char *src = preg(shortreg);
		if (dst != src)
			print("mov %s,%s\n", dst, src);
	}
}

static void doarg(p) Node p; {
	assert(p && p->syms[0]);
	mkactual(4, p->syms[0]->u.c.v.i);
}
static void blkfetch(k, off, reg, tmp)
int k, off, reg, tmp; {}
static void blkstore(k, off, reg, tmp)
int k, off, reg, tmp; {}
static void blkloop(dreg, doff, sreg, soff, size, tmps)
int dreg, doff, sreg, soff, size, tmps[]; {}
static void local(p) Symbol p; {
	if (isfloat(p->type))
		p->sclass = AUTO;
	if (askregvar(p, rmap[ttob(p->type)]) == 0)
		mkauto(p);
}
static void function(f, caller, callee, n)
Symbol f, callee[], caller[]; int n; {
	int i;

	print("%s:\n", f->x.name);
	print("push ebx\n");
	print("push esi\n");
	print("push edi\n");
	print("push ebp\n");
	print("mov ebp,esp\n");
usedmask[0] = usedmask[1] = 0;
freemask[0] = freemask[1] = ~(unsigned)0;
	offset = 16 + 4;
	for (i = 0; callee[i]; i++) {
		Symbol p = callee[i];
		Symbol q = caller[i];
		assert(q);
		p->x.offset = q->x.offset = offset;
		p->x.name = q->x.name = stringf("%d", p->x.offset);
		p->sclass = q->sclass = AUTO;
		offset += roundup(q->type->size, 4);
	}
	assert(caller[i] == 0);
	offset = maxoffset = 0;
	gencode(caller, callee);
	framesize = roundup(maxoffset, 4);
	if (framesize > 0)
		print("sub esp,%d\n", framesize);
	emitcode();
	print("mov esp,ebp\n");
	print("pop ebp\n");
	print("pop edi\n");
	print("pop esi\n");
	print("pop ebx\n");
	print("ret\n");
}
static void defsymbol(p) Symbol p; {
	if (p->scope >= LOCAL && p->sclass == STATIC)
		p->x.name = stringf("L%d", genlabel(1));
	else if (p->generated)
		p->x.name = stringf("L%s", p->name);
	else if (p->scope == GLOBAL || p->sclass == EXTERN)
		p->x.name = stringf("_%s", p->name);
	else if (p->scope == CONSTANTS
	&& (isint(p->type) || isptr(p->type))
	&& p->name[0] == '0' && p->name[1] == 'x')
		p->x.name = stringf("0%sH", &p->name[2]);
	else
		p->x.name = p->name;
}
static void address(q, p, n) Symbol q, p; int n; {
	if (p->scope == GLOBAL
	|| p->sclass == STATIC || p->sclass == EXTERN)
		q->x.name = stringf("%s%s%d",
			p->x.name, n >= 0 ? "+" : "", n);
	else {
		q->x.offset = p->x.offset + n;
		q->x.name = stringd(q->x.offset);
	}
}
static void defconst(ty, v) int ty; Value v; {
	switch (ty) {
		case C: print("db %d\n",   v.uc); return;
		case S: print("dw %d\n",   v.ss); return;
		case I: print("dd %d\n",   v.i ); return;
		case U: print("dd 0%xH\n", v.u ); return;
		case P: print("dd 0%xH\n", v.p ); return;
		case F:
			print("dd 0%xH\n", *(unsigned *)&v.f);
			return;
		case D: {
			unsigned *p = (unsigned *)&v.d;
			print("dd 0%xH,0%xH\n", p[swap], p[1 - swap]);
			return;
			}
	}
	assert(0);
}
static void defaddress(p) Symbol p; {
	print("dd %s\n", p->x.name);
}
static void defstring(n, str) int n; char *str; {
	char *s;

	for (s = str; s < str + n; s++)
		print("db %d\n", (*s)&0377);
}
static void export(p) Symbol p; {
	print("public %s\n", p->x.name);
}
static void import(p) Symbol p; {
	int oldseg = cseg;

	if (p->ref > 0) {
		segment(0);
		print("extrn %s:near\n", p->x.name);
		segment(oldseg);
	}
}
static void global(p) Symbol p; {
	print("align %d\n",
		p->type->align > 4 ? 4 : p->type->align);
	print("%s label byte\n", p->x.name);
	if (p->u.seg == BSS)
		print("db %d dup (0)\n", p->type->size);
}
static void space(n) int n; {
	if (cseg != BSS)
		print("db %d dup (0)\n", n);
}
Interface x86IR = {
	1, 1, 0,  /* char */
	2, 2, 0,  /* short */
	4, 4, 0,  /* int */
	4, 4, 1,  /* float */
	8, 4, 1,  /* double */
	4, 4, 0,  /* T * */
	0, 4, 0,  /* struct; so that ARGB keeps stack aligned */
	1,        /* little_endian */
	0,        /* mulops_calls */
	0,        /* wants_callb */
	1,        /* wants_argb */
	0,        /* left_to_right */
	0,        /* wants_dag */
	address,
	blockbeg,
	blockend,
	defaddress,
	defconst,
	defstring,
	defsymbol,
	emit,
	export,
	function,
	gen,
	global,
	import,
	local,
	progbeg,
	progend,
	segment,
	space,
	0, 0, 0, 0, 0, 0, 0,
	{1, blkfetch, blkstore, blkloop,
	    _label,
	    _rule,
	    _nts,
	    _kids,
	    _opname,
	    _arity,
	    _string,
	    _templates,
	    _isinstruction,
	    _ntname,
	    emit2,
	    doarg,
	    target,
	    clobber,
}
};


---

# Appendix: tests/LCC_3.6_C89_C90_Grammar_Production_Tests_Final/GRAMMAR_PRODUCTION_MATRIX.md

# C89/C90 Annex B.2 — Production-by-Production Test Matrix

Source basis: the supplied ISO/IEC 9899:1990 (E) Annex B.2 phrase-structure grammar.

This suite has **one test file per named grammar nonterminal** in B.2.1–B.3.
Each `.c` file repeats the production in its header and contains a valid C89/C90
example intended to exercise that production.

Important:
- This is a **production-level** one-to-one mapping, not one file per individual alternative.
- Terminals such as `identifier`, `constant`, `;`, `{`, `++`, etc. are exercised through the productions that contain them.
- Recursive productions are represented by examples that actually take the recursive form.
- Preprocessing tests may contain directives that are intentionally disabled (for example `#error`) so the whole file remains compilable.
- This suite is about syntax/grammar coverage, not exhaustive semantic/undefined-behavior coverage.


---

# Appendix: tests/LCC_3.6_C89_C90_Grammar_Production_Tests_Final/README.md

# LCC 3.6 C89/C90 Grammar Production Tests

This is the exhaustive **named-production** companion to the Used/Unused syntax suites.

The supplied C89/C90 standard's Annex B.2 phrase-structure grammar is represented
as one independent test file per named nonterminal (74 files).

Recommended order:
1. Compile the LCC source.
2. Run the Used Syntax suite.
3. Run the Unused Syntax suite.
4. Run this production-by-production suite.
5. If a production test fails, inspect the exact production printed in that file's header.

This does not claim that every semantic constraint or every implementation-defined
behavior of the C standard is exhaustively tested.


---

# Appendix: tests/LCC_3.6_C89_C90_Unused_Syntax_Tests_Final/AUDIT_MATRIX.md

# LCC 3.6 C89/C90 Syntax Coverage Audit

## Scope

This audit compares the C89/C90 phrase-structure grammar in the supplied C89/C90 standard document with **actual C source constructs in the LCC 3.6 compiler distribution**. The primary self-hosting scope is compiler source that is built as part of LCC itself: `src/`, `cpp/`, and `lburg/`. Mere appearances in comments, strings, keyword tables, generated assembly strings, or internal enum names are not counted as source-language use.

The `tst/` directory is treated as a separate test suite: it is evidence that a construct is supported by LCC, but it does **not** count as self-hosting coverage because those test programs are not what builds the compiler itself.

## Original 18-test suite

| Original test | C89/C90 construct | LCC 3.6 compiler-source use? | Action |
|---|---|---:|---|
| 01 unary plus | unary `+` | No actual source-language use found | KEEP |
| 02 auto | `auto` storage class | No actual declaration found | KEEP |
| 03 signed type specifier | standalone `signed` | No actual standalone use found | KEEP |
| 04 const volatile | repeated type qualifiers | No `const volatile` / `volatile const` declaration found | KEEP |
| 05 abstract array | original file used a named array parameter, not an abstract declarator | Not a valid unused-syntax test as written | REPLACE |
| 06 abstract pointer | original file used a named pointer, not an abstract declarator | Abstract pointer type-name is already used, e.g. `sizeof (Type *)` | REMOVE |
| 07 empty parameter list | `f()` | Yes: many actual definitions such as `static void emitYYnull() {}` | REMOVE |
| 08 old-style definition | `f(a,b) int a; int b;` | Yes | REMOVE |
| 09 ellipsis | `...` parameter list | Yes: `error`, `warning`, `print`, `fprint`, `stringf`, etc. | REMOVE |
| 10 function pointer | function-pointer declarator | Yes | REMOVE |
| 11 nested declarator | nested declarator/function pointer | Yes | REMOVE |
| 12 struct/union/enum | tags and definitions | Yes | REMOVE |
| 13 bit-field | `: constant-expression` | Yes: actual bit-fields in `src/config.h` | REMOVE |
| 14 initializer forms | initializer/initializer-list | Yes | REMOVE |
| 15 label/goto | label and `goto` | Yes | REMOVE |
| 16 switch/case/default | selection statement | Yes | REMOVE |
| 17 do-while | iteration statement | Yes: `src/stmt.c` has `dostmt` and actual `do` loops occur in compiler source | REMOVE |
| 18 comma/conditional | comma and `?:` expressions | Yes | REMOVE |

## Additional gaps found during the complete pass

The original 18 tests missed several grammar alternatives that are not exercised by compiler source:

| Test | C89/C90 construct | Result |
|---|---|---|
| 04 volatile | standalone `volatile` qualifier | No actual source-language declaration found |
| 06 abstract array type-name | `sizeof(int [3])` | No actual compiler-source use found |
| 07 tag-only struct declaration | `struct Forward;` | No actual compiler-source use found |
| 08 `#elif` | preprocessing directive | No actual directive found in compiler source |
| 09 `#error` | preprocessing directive | No actual directive found in compiler source |
| 10 `#pragma` | preprocessing directive | No actual directive found in compiler source |
| 11 null directive | `#` followed by newline | No actual directive found in compiler source |

`#line` is **not** included here because actual `# line` directives occur in `lburg/gram.c`. Likewise `#if`, `#ifdef`, `#ifndef`, `#else`, `#endif`, `#include`, `#define`, and `#undef` occur in compiler source.

## Important distinction

A parser implementation such as `case '+':` proves that LCC implements a grammar alternative, but it does not prove that compiling LCC itself exercised that alternative. Conversely, a real C construct in LCC source does prove that the compiler path was exercised when LCC was built with LCC.

This audit therefore uses three concepts:

1. **Implemented** — the LCC parser/preprocessor has code for the grammar feature.
2. **Self-hosting exercised** — an actual C source construct in compiler source uses the feature.
3. **Independent test required** — no self-hosting source use was found, so a dedicated C/preprocessor test is retained.

The final test directory contains only category (3). The mapping above preserves what happened to all 18 original tests.


---

# Appendix: tests/LCC_3.6_C89_C90_Unused_Syntax_Tests_Final/README.md

# LCC 3.6 C89/C90 Unused Syntax Tests — Final Audit

These tests target C89/C90 grammar alternatives that were not found as actual source-language constructs in the LCC 3.6 compiler source used for self-hosting coverage.

The audit is based on the supplied C89/C90 standard document and the supplied LCC 3.6 source tree. The original 18-test suite was rechecked rather than blindly retained.

The final suite contains 11 tests. Several original tests were removed because LCC itself already uses those constructs; the do-while test is one such example.

See `AUDIT_MATRIX.md` for the complete mapping and methodology.

## Expected handling

Most files should compile successfully. `09_preprocessor_error.c` intentionally contains `#error`; its purpose is to verify that the directive is recognized by LCC's preprocessor, so its expected diagnostic behavior is documented separately rather than treating a diagnostic as a syntax failure.


---

# Appendix: tests/LCC_3.6_C89_C90_Used_Syntax_Tests_Final/COVERAGE_MATRIX.md

# Used Syntax Coverage

This set is intended to re-validate, using independent inputs, the syntax confirmed to be actually used during the LCC 3.6 source audit.

| File | Main syntax |
|---|---|
| 01 | declarations, extern/static/register, const |
| 02 | char/short/int/long/float/double, signedness via unsigned, typedef-name |
| 03 | const-qualified declarations/pointers |
| 04 | pointer, address-of, dereference |
| 05 | array, indexing, pointer arithmetic |
| 06 | prototypes, typed parameters, function calls, void |
| 07 | old-style function definition |
| 08 | empty parameter list |
| 09 | ellipsis/variadic parameter list |
| 10 | function pointer, nested declarator |
| 11 | struct/union/enum |
| 12 | bit-field |
| 13 | initializer-list and trailing comma |
| 14 | unary/binary/logical/relational/bitwise/conditional operators |
| 15 | all compound assignment operators |
| 16 | comma, conditional, sizeof, pointer type-name |
| 17 | if/else, while, do-while, for, switch/case/default, continue/break |
| 18 | label, goto, return |
| 19 | include/define/undef/ifdef/ifndef/if/else/endif/line, function-like macro |
| 20 | abstract pointer/function type-names, sizeof |
| 21 | struct typedef-name, enum typedef-name, self-referential pointer |

This table is based on "representative independent tests" and is not a formal verification matrix that maps every alternative in the standard grammar to a separate file on a one-to-one basis.


---

# Appendix: tests/LCC_3.6_C89_C90_Used_Syntax_Tests_Final/README.md

# LCC 3.6 — C89/C90 Used Syntax Tests

Purpose:
- Re-validate, using independent input programs, the C89/C90 syntax confirmed to be actually used in the LCC 3.6 source code (`src/`, `cpp/`, `lburg`).
- Use this together with `LCC_3.6_C89_C90_Unused_Syntax_Tests_Final.zip`.
- This set is not a formal completeness proof of "all syntax implemented by LCC"; it independently tests representative syntax that was confirmed to be actually used during this source audit.

Recommended verification:
1. Run preprocessing/compilation with LCC
2. Generate assembly
3. assembler/linker
4. Verify the execution results

Notes:
- If a test fails, identify whether the failure occurs in the parser, semantic analysis, type system, backend, or runtime/library.
- Results such as `sizeof` and integer representation may vary depending on the target/ABI.
- `#line` is included because its actual use was confirmed during the source audit.
- `auto`, standalone `signed`, `volatile`, `const volatile`, abstract array type-name, tag-only struct declaration,
  `#elif`, `#error`, `#pragma`, null preprocessing directive, etc. are covered in the separate Unused test set.
