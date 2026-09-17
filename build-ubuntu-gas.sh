#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
B="$ROOT/build-ubuntu"
CC=${CC:-gcc}
AS=${AS:-/usr/bin/as}
LD=${LD:-/usr/bin/ld}
rm -rf "$B"
mkdir -p "$B"
cp "$ROOT/lburg/gram.c" "$B/gram.c"
CFLAGS="-std=gnu89 -O0 -g -D_POSIX_SOURCE"
# build lburg from the shipped generated parser
$CC $CFLAGS -I"$ROOT/lburg" -c "$ROOT/lburg/lburg.c" -o "$B/lburg.o"
$CC $CFLAGS -I"$ROOT/lburg" -c "$B/gram.c" -o "$B/gram.o"
$CC -o "$B/lburg" "$B/lburg.o" "$B/gram.o"
# build rcc front/backend objects
for f in alloc bind dag decl enode error expr event init input lex list main output prof profio simp stmt string sym trace tree types null symbolic gen mips sparc x86; do
  $CC $CFLAGS -I"$ROOT/src" -c "$ROOT/src/$f.c" -o "$B/$f.o"
done
"$B/lburg" < "$ROOT/src/x86gas.md" > "$B/x86gas.c"
$CC $CFLAGS -I"$ROOT/src" -c "$B/x86gas.c" -o "$B/x86gas.o"
objs=""
for f in alloc dag decl enode error expr event init input lex list main output prof profio simp stmt string sym trace tree types null symbolic gen mips sparc x86; do objs="$objs $B/$f.o"; done
$CC -O0 -g -o "$B/rcc" $objs "$B/bind.o" "$B/x86gas.o"
# cpp
for f in cpp lex nlist tokens macro eval include hideset unix; do
  $CC $CFLAGS -I"$ROOT/cpp" -c "$ROOT/cpp/$f.c" -o "$B/$f.o"
done
$CC -o "$B/cpp" "$B/cpp.o" "$B/lex.o" "$B/nlist.o" "$B/tokens.o" "$B/macro.o" "$B/eval.o" "$B/include.o" "$B/hideset.o" "$B/unix.o"
# lcc wrapper + Ubuntu ELF32 driver
$CC -m32 -c "$ROOT/src/start.s" -o "$B/start.o"
$CC -std=gnu89 -O0 -g -D_POSIX_SOURCE -DLCCDIR=\"$B/\" -c "$ROOT/etc/lcc.c" -o "$B/lcc.o"
$CC -std=gnu89 -O0 -g -D_POSIX_SOURCE -DLCCDIR=\"$B/\" -c "$ROOT/etc/lin-elf-gas.c" -o "$B/host.o"
$CC -o "$B/lcc" "$B/lcc.o" "$B/host.o"
printf '%s\n' "Built: $B/lcc"
printf '%s\n' "Target: Ubuntu Linux i386 ELF via GNU as + GNU ld"
