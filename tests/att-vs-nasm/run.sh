#!/usr/bin/env bash
# AT&T back end (x86/linux) regression test against the NASM back end (x86/nasm).
#
# For every C file in tests/*/ and tst/:
#   gcc -m32 -std=c89 (reference) | LCC AT&T: cpp->rcc->as->gcc | LCC NASM: cpp->rcc->nasm->gcc
# then
#   1) runtime : stdout + exit code of AT&T and NASM must match (gcc shown for reference)
#   2) tst/    : output compared with LCC's expected x86/dos/tst/*.1bk (informational)
#   3) code    : AT&T and NASM objects must be instruction-for-instruction equivalent
#   4) grammar : src/x86linux.md must mirror src/x86nasm.md (rules, order, costs)
#
# Prerequisite: ./BUILD-UBUNTU24.sh (build-ubuntu24/lcc-build/{cpp,rcc}).
# Usage: tests/att-vs-nasm/run.sh [out-dir]       exit 0 = no unexpected difference
set -u
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
B=$ROOT/build-ubuntu24/lcc-build
INC=$ROOT/include/x86/linux          # glibc-compatible ANSI headers (see README there)
OUT=${1:-$ROOT/build-ubuntu24/test-att-vs-nasm}
CPPFLAGS="-D__STDC__=1 -D__ELF__ -Di386 -D__i386 -D__i386__ -Dlinux -D__linux -D__linux__"

# ---- documented, expected differences -------------------------------------
# build stage that is expected to fail identically for AT&T and NASM
declare -A EXPECT_FAIL=(
  [Grammar_Production/22_declaration]=LD_FAIL               # test uses 'extern int e' with no definition
  [Grammar_Production/25_storage_class_specifier]=LD_FAIL   # same: extern without definition
  [Unused_Syntax/09_preprocessor_error]=CPP_FAIL            # #error must stop preprocessing
  [tst/front]=RCC_FAIL                                      # front-end diagnostics test
)
# exit code is garbage (main(){} returns whatever is in %eax): compare stdout only
declare -A RC_UNDEFINED=( [tst/incr]=1 [tst/spill]=1 )
# objects allowed to differ at instruction level ("<suite>/<test>").
# Empty: x86linux.md mirrors x86nasm.md rule-for-rule (checked by check_grammar.py).
KNOWN_CODE_DIFF=()
# ---------------------------------------------------------------------------

for t in "$B/cpp" "$B/rcc" /usr/bin/as /usr/bin/nasm /usr/bin/gcc; do
  [[ -x $t ]] || { echo "missing $t (run ./BUILD-UBUNTU24.sh first)"; exit 2; }
done
rm -rf "$OUT"; mkdir -p "$OUT"
RES=$OUT/results.tsv

run() { # exe stdin out-prefix
  if [[ -n $2 ]]; then timeout 20 "$1" <"$2" >"$3.out" 2>"$3.err"; else timeout 20 "$1" </dev/null >"$3.out" 2>"$3.err"; fi
  echo $? >"$3.rc"
}

build_lcc() { # src backend dir
  local src=$1 be=$2 d=$3 n
  n=$(basename "$src" .c)
  "$B/cpp" $CPPFLAGS -I"$INC" "$src" "$d/$n.i" 2>"$d/$n.cpp.err" || { echo CPP_FAIL; return; }
  if [[ $be == att ]]; then
    "$B/rcc" -target=x86/linux "$d/$n.i" "$d/$n.s" 2>"$d/$n.rcc.err" || { echo RCC_FAIL; return; }
    /usr/bin/as --32 -o "$d/$n.o" "$d/$n.s" 2>"$d/$n.as.err" || { echo AS_FAIL; return; }
  else
    "$B/rcc" -target=x86/nasm "$d/$n.i" "$d/$n.s" 2>"$d/$n.rcc.err" || { echo RCC_FAIL; return; }
    /usr/bin/nasm -f elf32 -o "$d/$n.o" "$d/$n.s" 2>"$d/$n.as.err" || { echo AS_FAIL; return; }
  fi
  /usr/bin/gcc -m32 "$d/$n.o" -o "$d/$n" -lm 2>"$d/$n.ld.err" || { echo LD_FAIL; return; }
  echo OK
}

sig() { # out-prefix [stdout-only]
  [[ -f $1.rc ]] || return
  local o; o=$(md5sum <"$1.out" | cut -c1-8)
  if [[ -n ${2:-} ]]; then echo "out=$o"; else echo "rc=$(cat "$1.rc") out=$o"; fi
}

fails=0; n_total=0; n_pass=0; n_expected=0
printf 'suite\ttest\tgcc\tatt\tnasm\tverdict\n' >"$RES"
printf '%-20s %-45s %-22s %-22s %s\n' SUITE TEST ATT NASM VERDICT

for src in "$ROOT"/tests/*/*.c "$ROOT"/tst/*.c; do
  suite=$(basename "$(dirname "$src")"); n=$(basename "$src" .c)
  suite=${suite#LCC_3.6_C89_C90_}; suite=${suite%_Tests_Final}
  key=$suite/$n; n_total=$((n_total+1))
  in=""; [[ $suite == tst && -f $ROOT/tst/$n.0 ]] && in=$ROOT/tst/$n.0
  for k in gcc att nasm; do mkdir -p "$OUT/$suite/$k"; done

  if gcc -m32 -std=c89 -w -o "$OUT/$suite/gcc/$n" "$src" -lm 2>"$OUT/$suite/gcc/$n.cc.err"; then
    run "$OUT/$suite/gcc/$n" "$in" "$OUT/$suite/gcc/$n"; gs=$(sig "$OUT/$suite/gcc/$n")
  else gs=BUILD_FAIL; fi
  a=$(build_lcc "$src" att  "$OUT/$suite/att");  [[ $a == OK ]] && run "$OUT/$suite/att/$n"  "$in" "$OUT/$suite/att/$n"
  m=$(build_lcc "$src" nasm "$OUT/$suite/nasm"); [[ $m == OK ]] && run "$OUT/$suite/nasm/$n" "$in" "$OUT/$suite/nasm/$n"
  so=${RC_UNDEFINED[$key]:-}
  as_=$a; [[ $a == OK ]] && as_=$(sig "$OUT/$suite/att/$n" $so)
  ms=$m;  [[ $m == OK ]] && ms=$(sig "$OUT/$suite/nasm/$n" $so)

  exp=${EXPECT_FAIL[$key]:-}
  if [[ -n $exp ]]; then
    if [[ $a == "$exp" && $m == "$exp" ]]; then v="EXPECTED($exp)"; n_expected=$((n_expected+1))
    else v="FAIL(expected $exp)"; fails=$((fails+1)); fi
  elif [[ $a != OK || $m != OK ]]; then v=FAIL; fails=$((fails+1))
  elif [[ $as_ != "$ms" ]]; then v=FAIL; fails=$((fails+1))
  else
    v=PASS; n_pass=$((n_pass+1))
    [[ -n $so ]] && v="PASS(rc undefined)"
  fi
  printf '%s\t%s\t%s\t%s\t%s\t%s\n' "$suite" "$n" "$gs" "$as_" "$ms" "$v" >>"$RES"
  printf '%-20s %-45s %-22s %-22s %s\n' "$suite" "$n" "$as_" "$ms" "$v"
done

echo
echo "== tst/ output vs LCC expected (x86/dos/tst/*.1bk, informational) =="
for f in "$ROOT"/tst/*.c; do
  n=$(basename "$f" .c); e=$ROOT/x86/dos/tst/$n.1bk
  [[ -s $e && -f $OUT/tst/att/$n.out ]] || continue
  if cmp -s "$e" "$OUT/tst/att/$n.out"; then r=match
  else r="differs ($(diff "$e" "$OUT/tst/att/$n.out" | grep -c '^[<>]') lines)"; fi
  case $n in
    limits) r="$r  [DOS: unsigned char; Linux: signed char]";;
    cvt)    r="$r  [old DOS .1bk; LCC result is the correct one]";;
  esac
  printf '  %-10s %s\n' "$n" "$r"
done

echo
echo "== AT&T vs NASM machine code =="
python3 -I "$HERE/cmp_obj.py" "$OUT" "${KNOWN_CODE_DIFF[@]}"; code_rc=$?

echo
echo "== x86linux.md vs x86nasm.md grammar =="
python3 -I "$HERE/check_grammar.py" "$ROOT"; gram_rc=$?

echo
echo "runtime: $n_total tests, $n_pass pass, $n_expected expected failures, $fails unexpected failures"
echo "details: $OUT"
[[ $fails -eq 0 && $code_rc -eq 0 && $gram_rc -eq 0 ]] && { echo "RESULT: OK"; exit 0; }
echo "RESULT: FAILED"; exit 1
