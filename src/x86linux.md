%{
/*
 * x86linux.md -- LCC 3.6 x86 back end emitting AT&T syntax for GNU as (ELF).
 *
 * Instruction selection, register allocation, frame layout and calling
 * convention are identical to x86nasm.md; only the assembly syntax differs:
 *   - operand order is  src,dst
 *   - registers are prefixed with %, immediates with $
 *   - operand size is carried by the mnemonic suffix (b/w/l, s/l for x87)
 *   - memory operands are disp(base,index,scale)
 *
 * Select it with  rcc -target=x86/linux  (alias: -target=x86/att).
 */
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
base: reg             "(%0)"
base: ADDI(reg,acon)  "%1(%0)"
base: ADDP(reg,acon)  "%1(%0)"
base: ADDU(reg,acon)  "%1(%0)"
base: ADDRFP  "%a(%%ebp)"
base: ADDRLP  "%a(%%ebp)"
index: reg "%0,1"
index: LSHI(reg,con1)  "%0,2"
index: LSHI(reg,con2)  "%0,4"
index: LSHI(reg,con3)  "%0,8"

con1:  CNSTI  "1"  range(a, 1, 1)
con1:  CNSTU  "1"  range(a, 1, 1)
con2:  CNSTI  "2"  range(a, 2, 2)
con2:  CNSTU  "2"  range(a, 2, 2)
con3:  CNSTI  "3"  range(a, 3, 3)
con3:  CNSTU  "3"  range(a, 3, 3)
index: LSHU(reg,con1)  "%0,2"
index: LSHU(reg,con2)  "%0,4"
index: LSHU(reg,con3)  "%0,8"

gsym:  ADDRGP  "%a"
fpoff: ADDRFP  "%a"
fpoff: ADDRLP  "%a"

addr: base              "%0"
addr: ADDI(index,reg)              "(%1,%0)"
addr: ADDP(index,reg)              "(%1,%0)"
addr: ADDU(index,reg)              "(%1,%0)"
addr: ADDI(index,gsym)             "%1(,%0)"
addr: ADDP(index,gsym)             "%1(,%0)"
addr: ADDU(index,gsym)             "%1(,%0)"
addr: ADDI(index,fpoff)            "%1(%%ebp,%0)"
addr: ADDP(index,fpoff)            "%1(%%ebp,%0)"
addr: ADDU(index,fpoff)            "%1(%%ebp,%0)"
addr: ADDI(index,ADDI(reg,acon))   "%2(%1,%0)"
addr: ADDP(index,ADDP(reg,acon))   "%2(%1,%0)"
addr: ADDU(index,ADDU(reg,acon))   "%2(%1,%0)"
addr: index  "(,%0)"  1
mem: INDIRC(addr)  "%0"
mem: INDIRI(addr)  "%0"
mem: INDIRP(addr)  "%0"
mem: INDIRS(addr)  "%0"
rc:   reg  "%0"
rc:   con  "$%0"

mr:   reg  "%0"
mr:   mem  "%0"

mrc0: mem  "%0"
mrc0: rc   "%0"
mrc1: mem  "%0"  1
mrc1: rc   "%0"

mrc3: mem  "%0"  3
mrc3: rc   "%0"
reg: addr        "leal %0,%c\n"  1
reg: mrc0        "mov %0,%c\n"  1
reg: LOADC(reg)  "movb %0,%c\n"  move(a)
reg: LOADI(reg)  "movl %0,%c\n"  move(a)
reg: LOADP(reg)  "movl %0,%c\n"  move(a)
reg: LOADS(reg)  "movw %0,%c\n"  move(a)
reg: LOADU(reg)  "movl %0,%c\n"  move(a)
reg: ADDI(reg,mrc1)  "?movl %0,%c\naddl %1,%c\n"  1
reg: ADDP(reg,mrc1)  "?movl %0,%c\naddl %1,%c\n"  1
reg: ADDU(reg,mrc1)  "?movl %0,%c\naddl %1,%c\n"  1
reg: SUBI(reg,mrc1)  "?movl %0,%c\nsubl %1,%c\n"  1
reg: SUBP(reg,mrc1)  "?movl %0,%c\nsubl %1,%c\n"  1
reg: SUBU(reg,mrc1)  "?movl %0,%c\nsubl %1,%c\n"  1
reg: BANDU(reg,mrc1)  "?movl %0,%c\nandl %1,%c\n"  1
reg: BORU(reg,mrc1)   "?movl %0,%c\norl %1,%c\n"   1
reg: BXORU(reg,mrc1)  "?movl %0,%c\nxorl %1,%c\n"  1
stmt: ASGNI(addr,ADDI(mem,con1))  "incl %1\n"  memop(a)
stmt: ASGNI(addr,ADDU(mem,con1))  "incl %1\n"  memop(a)
stmt: ASGNP(addr,ADDP(mem,con1))  "incl %1\n"  memop(a)
stmt: ASGNI(addr,SUBI(mem,con1))  "decl %1\n"  memop(a)
stmt: ASGNI(addr,SUBU(mem,con1))  "decl %1\n"  memop(a)
stmt: ASGNP(addr,SUBP(mem,con1))  "decl %1\n"  memop(a)
stmt: ASGNI(addr,ADDI(mem,rc))   "addl %2,%1\n"  memop(a)
stmt: ASGNI(addr,ADDU(mem,rc))   "addl %2,%1\n"  memop(a)
stmt: ASGNI(addr,SUBI(mem,rc))   "subl %2,%1\n"  memop(a)
stmt: ASGNI(addr,SUBU(mem,rc))   "subl %2,%1\n"  memop(a)

stmt: ASGNI(addr,BANDU(mem,rc))  "andl %2,%1\n"  memop(a)
stmt: ASGNI(addr,BORU(mem,rc))   "orl %2,%1\n"   memop(a)
stmt: ASGNI(addr,BXORU(mem,rc))  "xorl %2,%1\n"  memop(a)
reg: BCOMU(reg)  "?movl %0,%c\nnotl %c\n"  2
reg: NEGI(reg)   "?movl %0,%c\nnegl %c\n"  2

stmt: ASGNI(addr,BCOMU(mem))  "notl %1\n"  memop(a)
stmt: ASGNI(addr,NEGI(mem))   "negl %1\n"  memop(a)
reg: LSHI(reg,rc5)  "?movl %0,%c\nsall %1,%c\n"  2
reg: LSHU(reg,rc5)  "?movl %0,%c\nshll %1,%c\n"  2
reg: RSHI(reg,rc5)  "?movl %0,%c\nsarl %1,%c\n"  2
reg: RSHU(reg,rc5)  "?movl %0,%c\nshrl %1,%c\n"  2

stmt: ASGNI(addr,LSHI(mem,rc5))  "sall %2,%1\n"  memop(a)
stmt: ASGNI(addr,LSHU(mem,rc5))  "shll %2,%1\n"  memop(a)
stmt: ASGNI(addr,RSHI(mem,rc5))  "sarl %2,%1\n"  memop(a)
stmt: ASGNI(addr,RSHU(mem,rc5))  "shrl %2,%1\n"  memop(a)

rc5: CNSTI  "$%a"  range(a, 0, 31)
rc5: reg    "%%cl"
reg: MULI(reg,mrc3)  "?movl %0,%c\nimull %1,%c\n"  14
reg: MULI(con,mr)    "imull $%0,%1,%c\n"  13
reg: MULU(reg,mr)  "mull %1\n"  13
reg: DIVU(reg,reg)  "xorl %%edx,%%edx\ndivl %1\n"
reg: MODU(reg,reg)  "xorl %%edx,%%edx\ndivl %1\n"
reg: DIVI(reg,reg)  "cltd\nidivl %1\n"
reg: MODI(reg,reg)  "cltd\nidivl %1\n"
reg: CVIU(reg)  "movl %0,%c\n"  move(a)
reg: CVPU(reg)  "movl %0,%c\n"  move(a)
reg: CVUI(reg)  "movl %0,%c\n"  move(a)
reg: CVUP(reg)  "movl %0,%c\n"  move(a)
reg: CVCI(INDIRC(addr))  "movsbl %0,%c\n"  3
reg: CVCU(INDIRC(addr))  "movzbl %0,%c\n"  3
reg: CVSI(INDIRS(addr))  "movswl %0,%c\n"  3
reg: CVSU(INDIRS(addr))  "movzwl %0,%c\n"  3
reg: CVCI(reg)  "# extend\n"  3
reg: CVCU(reg)  "# extend\n"  3
reg: CVSI(reg)  "# extend\n"  3
reg: CVSU(reg)  "# extend\n"  3

reg: CVIC(reg)  "# truncate\n"  1
reg: CVIS(reg)  "# truncate\n"  1
reg: CVUC(reg)  "# truncate\n"  1
reg: CVUS(reg)  "# truncate\n"  1
stmt: ASGNC(addr,rc)  "movb %1,%0\n"   1
stmt: ASGNI(addr,rc)  "movl %1,%0\n"  1
stmt: ASGNP(addr,rc)  "movl %1,%0\n"  1
stmt: ASGNS(addr,rc)  "movw %1,%0\n"   1
stmt: ARGI(mrc3)  "pushl %0\n"  1
stmt: ARGP(mrc3)  "pushl %0\n"  1
stmt: ASGNB(reg,INDIRB(reg))  "movl $%a,%%ecx\nrep\nmovsb\n"
stmt: ARGB(INDIRB(reg))  "subl $%a,%%esp\nmovl %%esp,%%edi\nmovl $%a,%%ecx\nrep\nmovsb\n"

memf: INDIRD(addr)        "l %0"
memf: INDIRF(addr)        "s %0"
memf: CVFD(INDIRF(addr))  "s %0"
reg: memf  "fld%0\n"  3
stmt: ASGND(addr,reg)        "fstpl %0\n"  7
stmt: ASGNF(addr,reg)        "fstps %0\n"  7
stmt: ASGNF(addr,CVDF(reg))  "fstps %0\n"  7
stmt: ARGD(reg)  "subl $8,%%esp\nfstpl (%%esp)\n"
stmt: ARGF(reg)  "subl $4,%%esp\nfstps (%%esp)\n"
reg: NEGD(reg)  "fchs\n"
reg: NEGF(reg)  "fchs\n"
reg: ADDD(reg,memf)  "fadd%1\n"
reg: ADDD(reg,reg)  "faddp %%st,%%st(1)\n"
reg: ADDF(reg,memf)  "fadd%1\n"
reg: ADDF(reg,reg)  "faddp %%st,%%st(1)\n"
reg: DIVD(reg,memf)  "fdiv%1\n"
reg: DIVD(reg,reg)  "fdivrp %%st,%%st(1)\n"
reg: DIVF(reg,memf)  "fdiv%1\n"
reg: DIVF(reg,reg)  "fdivrp %%st,%%st(1)\n"
reg: MULD(reg,memf)  "fmul%1\n"
reg: MULD(reg,reg)  "fmulp %%st,%%st(1)\n"
reg: MULF(reg,memf)  "fmul%1\n"
reg: MULF(reg,reg)  "fmulp %%st,%%st(1)\n"
reg: SUBD(reg,memf)  "fsub%1\n"
reg: SUBD(reg,reg)  "fsubrp %%st,%%st(1)\n"
reg: SUBF(reg,memf)  "fsub%1\n"
reg: SUBF(reg,reg)  "fsubrp %%st,%%st(1)\n"
reg: CVFD(reg)  "# CVFD\n"
reg: CVDF(reg)  "subl $4,%%esp\nfstps (%%esp)\nflds (%%esp)\naddl $4,%%esp\n"  12

stmt: ASGNI(addr,CVDI(reg))  "fistpl %0\n"  29
reg: CVDI(reg)  "subl $4,%%esp\nfistpl (%%esp)\npopl %c\n" 31

reg: CVID(INDIRI(addr))  "fildl %0\n"  10
reg: CVID(reg)  "pushl %0\nfildl (%%esp)\naddl $4,%%esp\n"  12

addrj: ADDRGP  "%a"
addrj: reg     "*%0"  2
addrj: mem     "*%0"  2

stmt:  JUMPV(addrj)  "jmp %0\n"  3
stmt:  LABELV        "%a:\n"
stmt: EQI(mem,rc)  "cmpl %1,%0\nje %a\n"   5
stmt: GEI(mem,rc)  "cmpl %1,%0\njge %a\n"  5
stmt: GTI(mem,rc)  "cmpl %1,%0\njg %a\n"   5
stmt: LEI(mem,rc)  "cmpl %1,%0\njle %a\n"  5
stmt: LTI(mem,rc)  "cmpl %1,%0\njl %a\n"   5
stmt: NEI(mem,rc)  "cmpl %1,%0\njne %a\n"  5
stmt: GEU(mem,rc)  "cmpl %1,%0\njae %a\n"  5
stmt: GTU(mem,rc)  "cmpl %1,%0\nja %a\n"   5
stmt: LEU(mem,rc)  "cmpl %1,%0\njbe %a\n"  5
stmt: LTU(mem,rc)  "cmpl %1,%0\njb %a\n"   5
stmt: EQI(reg,mrc1)  "cmpl %1,%0\nje %a\n"   4
stmt: GEI(reg,mrc1)  "cmpl %1,%0\njge %a\n"  4
stmt: GTI(reg,mrc1)  "cmpl %1,%0\njg %a\n"   4
stmt: LEI(reg,mrc1)  "cmpl %1,%0\njle %a\n"  4
stmt: LTI(reg,mrc1)  "cmpl %1,%0\njl %a\n"   4
stmt: NEI(reg,mrc1)  "cmpl %1,%0\njne %a\n"  4

stmt: GEU(reg,mrc1)  "cmpl %1,%0\njae %a\n"  4
stmt: GTU(reg,mrc1)  "cmpl %1,%0\nja %a\n"   4
stmt: LEU(reg,mrc1)  "cmpl %1,%0\njbe %a\n"  4
stmt: LTU(reg,mrc1)  "cmpl %1,%0\njb %a\n"   4
cmpf: memf  "%0"
cmpf: reg   "p"
stmt: EQD(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\nje %a\n"
stmt: GED(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njbe %a\n"
stmt: GTD(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njb %a\n"
stmt: LED(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njae %a\n"
stmt: LTD(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\nja %a\n"
stmt: NED(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njne %a\n"

stmt: EQF(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\nje %a\n"
stmt: GEF(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njbe %a\n"
stmt: GTF(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njb %a\n"
stmt: LEF(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njae %a\n"
stmt: LTF(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\nja %a\n"
stmt: NEF(cmpf,reg)  "fcomp%0\nfstsw %%ax\nsahf\njne %a\n"
reg:  CALLI(addrj)  "call %0\naddl $%a,%%esp\n"
stmt: CALLV(addrj)  "call %0\naddl $%a,%%esp\n"
reg: CALLF(addrj)  "call %0\naddl $%a,%%esp\n"
reg: CALLD(addrj)  "call %0\naddl $%a,%%esp\n"

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
		swap = (u.i == 1) != IR->little_endian;
	}
	parseflags(argc, argv);
	intreg[EAX] = mkreg("%%eax", EAX, 1, IREG);
	intreg[EDX] = mkreg("%%edx", EDX, 1, IREG);
	intreg[ECX] = mkreg("%%ecx", ECX, 1, IREG);
	intreg[EBX] = mkreg("%%ebx", EBX, 1, IREG);
	intreg[ESI] = mkreg("%%esi", ESI, 1, IREG);
	intreg[EDI] = mkreg("%%edi", EDI, 1, IREG);
	shortreg[EAX] = mkreg("%%ax", EAX, 1, IREG);
	shortreg[ECX] = mkreg("%%cx", ECX, 1, IREG);
	shortreg[EDX] = mkreg("%%dx", EDX, 1, IREG);
	shortreg[EBX] = mkreg("%%bx", EBX, 1, IREG);
	shortreg[ESI] = mkreg("%%si", ESI, 1, IREG);
	shortreg[EDI] = mkreg("%%di", EDI, 1, IREG);

	charreg[EAX]  = mkreg("%%al", EAX, 1, IREG);
	charreg[ECX]  = mkreg("%%cl", ECX, 1, IREG);
	charreg[EDX]  = mkreg("%%dl", EDX, 1, IREG);
	charreg[EBX]  = mkreg("%%bl", EBX, 1, IREG);
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
	cseg = 0;
	quo = mkreg("%%eax", EAX, 1, IREG);
	quo->x.regnode->mask |= 1<<EDX;
	rem = mkreg("%%edx", EDX, 1, IREG);
	rem->x.regnode->mask |= 1<<EAX;
}
static void segment(n) int n; {
	if (n == cseg)
		return;
	cseg = n;
	if (cseg == CODE)
		print(".text\n");
	else if (cseg == DATA)
		print(".data\n");
	else if (cseg == LIT)
		print(".section .rodata\n");
	else if (cseg == BSS)
		print(".bss\n");
}
static void progend() {
	/* Mark the stack non-executable; silences modern ld warnings. */
	print(".section .note.GNU-stack,\"\",@progbits\n");
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
	assert(p->count > 0 || nstack == 0);
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
		print("movsbl %s,%s\n", preg(charreg), p->syms[RX]->x.name);
	else if (p->op == CVCU)
		print("movzbl %s,%s\n", preg(charreg), p->syms[RX]->x.name);
	else if (p->op == CVSI)
		print("movswl %s,%s\n", preg(shortreg), p->syms[RX]->x.name);
	else if (p->op == CVSU)
		print("movzwl %s,%s\n", preg(shortreg), p->syms[RX]->x.name);
	else if (p->op == CVIC || p->op == CVIS
	      || p->op == CVUC || p->op == CVUS) {
		char *dst = shortreg[getregnum(p)]->x.name;
		char *src = preg(shortreg);
		if (dst != src)
			print("movw %s,%s\n", src, dst);
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

	print(".type %s,@function\n", f->x.name);
	print("%s:\n", f->x.name);
	print("pushl %%ebx\n");
	print("pushl %%esi\n");
	print("pushl %%edi\n");
	print("pushl %%ebp\n");
	print("movl %%esp,%%ebp\n");
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
		print("subl $%d,%%esp\n", framesize);
	emitcode();
	print("movl %%ebp,%%esp\n");
	print("popl %%ebp\n");
	print("popl %%edi\n");
	print("popl %%esi\n");
	print("popl %%ebx\n");
	print("ret\n");
	print(".size %s,.-%s\n", f->x.name, f->x.name);
}
static void defsymbol(p) Symbol p; {
	if (p->scope >= LOCAL && p->sclass == STATIC)
		p->x.name = stringf(".LS%d", genlabel(1));
	else if (p->generated)
		p->x.name = stringf(".L%s", p->name);
	else
		/* GNU as needs no escaping: registers carry a % prefix,
		   so C identifiers can never collide with them.  Integer
		   constants (including 0x... names) are already valid. */
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
		case C: print(".byte %d\n",  v.uc); return;
		case S: print(".short %d\n", v.ss); return;
		case I: print(".long %d\n",  v.i ); return;
		case U: print(".long 0x%x\n", v.u ); return;
		case P: print(".long 0x%x\n", v.p ); return;
		case F:
			print(".long 0x%x\n", *(unsigned *)&v.f);
			return;
		case D: {
			unsigned *p = (unsigned *)&v.d;
			print(".long 0x%x,0x%x\n", p[swap], p[1 - swap]);
			return;
			}
	}
	assert(0);
}
static void defaddress(p) Symbol p; {
	print(".long %s\n", p->x.name);
}
static void defstring(n, str) int n; char *str; {
	char *s;

	print(".ascii \"");
	for (s = str; s < str + n; s++) {
		int c = (*s) & 0377;
		if (c == '"' || c == '\\')
			print("\\%c", c);
		else if (c >= ' ' && c < 0177)
			print("%c", c);
		else {
			print("\\");
			/* always three octal digits so a following digit
			   is never absorbed into the escape */
			print("%c%c%c", '0' + ((c>>6)&3), '0' + ((c>>3)&7), '0' + (c&7));
		}
	}
	print("\"\n");
}
static void export(p) Symbol p; {
	print(".globl %s\n", p->x.name);
}
static void import(p) Symbol p; {
	/* GNU as treats every undefined symbol as external. */
}
static void global(p) Symbol p; {
	print(".balign %d\n", p->type->align > 4 ? 4 : p->type->align);
	if (p->x.name[0] != '.')	/* skip compiler-generated .L labels */
		print(".type %s,@object\n", p->x.name);
	print("%s:\n", p->x.name);
	if (p->u.seg == BSS)
		print(".space %d\n", p->type->size);
}
static void space(n) int n; {
	if (cseg != BSS)
		print(".space %d\n", n);
}
Interface x86linuxIR = {
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
