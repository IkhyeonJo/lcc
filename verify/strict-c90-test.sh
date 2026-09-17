#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
LCC="$ROOT/bin/lcc"
pass=0; fail=0
run_ok() { name=$1; shift; if "$@" >/tmp/lcc-v.out 2>/tmp/lcc-v.err; then echo "PASS $name"; pass=$((pass+1)); else echo "FAIL $name"; cat /tmp/lcc-v.err; fail=$((fail+1)); fi; }
run_bad() { name=$1; shift; if "$@" >/tmp/lcc-v.out 2>/tmp/lcc-v.err; then echo "FAIL $name (accepted)"; fail=$((fail+1)); else echo "PASS $name (rejected)"; pass=$((pass+1)); fi; }
cat >/tmp/c90.c <<'SRC'
int square(int x) { return x*x; }
int main(void) { int i; i = 6; return square(i); }
SRC
run_ok 'C90 source -> Ubuntu ELF32' "$LCC" -o /tmp/lcc-c90 /tmp/c90.c
file /tmp/lcc-c90 | grep -q 'ELF 32-bit'
readelf -h /tmp/lcc-c90 | grep -q 'Intel 80386'
cat >/tmp/cpp.c <<'SRC'
int main(void) { return 0; } // extension
SRC
run_bad '// comment' "$LCC" -c /tmp/cpp.c
cat >/tmp/e.c <<'SRC'
#error forbidden
int main(void) { return 0; }
SRC
run_bad '#error' "$LCC" -c /tmp/e.c
cat >/tmp/eval.c <<'SRC'
#eval 1+2
int main(void) { return 0; }
SRC
run_bad '#eval' "$LCC" -c /tmp/eval.c
cat >/tmp/pr.c <<'SRC'
#pragma once
int main(void) { return 0; }
SRC
run_bad '#pragma' "$LCC" -c /tmp/pr.c
cat >/tmp/enum.c <<'SRC'
enum E { A, B, };
int main(void) { return 0; }
SRC
run_bad 'enum trailing comma' "$LCC" -c /tmp/enum.c
cat >/tmp/init.c <<'SRC'
int a[] = { 1, 2, };
int main(void) { return 0; }
SRC
run_bad 'initializer trailing comma' "$LCC" -c /tmp/init.c
cat >/tmp/c99.c <<'SRC'
int main(void) { for (int i=0; i<1; i++) return i; return 0; }
SRC
run_bad 'C99 for declaration' "$LCC" -c /tmp/c99.c
printf '\nRESULT pass=%d fail=%d\n' "$pass" "$fail"
test "$fail" -eq 0
