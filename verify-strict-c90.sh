#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
CPP="$ROOT/build/cpp"
RCC="$ROOT/build/rcc"

tmp=${TMPDIR:-/tmp}/lcc-strict-c90-$$
trap 'rm -rf "$tmp"' EXIT INT TERM
mkdir "$tmp"

fail_if_ok() {
  name=$1; shift
  if "$@" >"$tmp/out" 2>"$tmp/err"; then
    echo "FAIL: $name was accepted" >&2
    cat "$tmp/err" >&2
    exit 1
  fi
}

printf 'int x; // comment\n' | "$CPP" >"$tmp/slash.i"
if grep -q '// comment' "$tmp/slash.i"; then
  fail_if_ok '// comment rejected by C parser' "$RCC" -target=null "$tmp/slash.i"
else
  echo 'FAIL: // comment was silently stripped' >&2
  exit 1
fi
printf '#error test\n' | fail_if_ok '#error rejected' "$CPP"
printf '#eval 1+2\n' | fail_if_ok '#eval rejected' "$CPP"
printf '#pragma once\n' | fail_if_ok '#pragma rejected' "$CPP"

cat >"$tmp/enum.c" <<'SRC'
enum E { A, };
SRC
"$CPP" "$tmp/enum.c" >"$tmp/enum.i"
fail_if_ok 'enum trailing comma' "$RCC" -target=null "$tmp/enum.i"

cat >"$tmp/init.c" <<'SRC'
int x[] = { 1, };
SRC
"$CPP" "$tmp/init.c" >"$tmp/init.i"
fail_if_ok 'initializer trailing comma' "$RCC" -target=null "$tmp/init.i"

cat >"$tmp/c90.c" <<'SRC'
int sum(a,b) int a; int b; { return a+b; }
int main(void) { int x[2] = {1,2}; return sum(x[0],x[1]); }
SRC
"$CPP" "$tmp/c90.c" >"$tmp/c90.i"
"$RCC" -target=null "$tmp/c90.i" >"$tmp/c90.out"

echo 'Strict ANSI C90 verification passed.'
