#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build-strict-c90"
mkdir -p "$BUILD"
cc=${CC:-cc}
$cc -std=c90 -Wall -Wextra -I"$ROOT/cpp" -c "$ROOT/cpp/cpp.c" -o "$BUILD/cpp.o"
for f in lex nlist tokens macro eval include hideset unix; do
  $cc -std=c90 -Wall -Wextra -I"$ROOT/cpp" -c "$ROOT/cpp/$([ "$f" = lex ] && echo lex || echo "$f").c" -o "$BUILD/$f.o"
done
$cc -o "$BUILD/cpp" "$BUILD"/*.o

expect_fail() {
  name=$1; shift
  if "$BUILD/cpp" "$@" >/dev/null 2>&1; then
    echo "FAIL: $name was accepted"
    exit 1
  fi
  echo "PASS: rejected $name"
}
expect_pass() {
  name=$1; shift
  if ! "$BUILD/cpp" "$@" >/dev/null 2>&1; then
    echo "FAIL: $name was rejected"
    exit 1
  fi
  echo "PASS: accepted $name"
}

tmp="$BUILD/cases"; mkdir -p "$tmp"
printf 'int main(void) { return 0; } // C++ comment\n' > "$tmp/slashslash.c"
printf '#error not C90\n' > "$tmp/error.c"
printf '#eval 1+1\n' > "$tmp/eval.c"
printf '#pragma once\nint main(void) { return 0; }\n' > "$tmp/pragma.c"
printf 'enum E { A, };\nint main(void) { return 0; }\n' > "$tmp/enumcomma.c"
printf 'int a[] = { 1, 2, };\nint main(void) { return a[1] != 2; }\n' > "$tmp/initcomma.c"
if ! grep -q '// C++ comment' <("$BUILD/cpp" "$tmp/slashslash.c" 2>/dev/null); then
  echo 'FAIL: preprocessor unexpectedly removed // comment text'
  exit 1
fi
echo 'PASS: // is not treated as a comment by cpp'
expect_fail '#error' "$tmp/error.c"
expect_fail '#eval' "$tmp/eval.c"
echo 'NOTE: enum trailing comma is a parser-level C90 rejection; cpp intentionally passes tokens through.'
expect_pass '#pragma (implementation-defined)' "$tmp/pragma.c"
expect_pass 'initializer trailing comma (C90)' "$tmp/initcomma.c"
echo 'Strict C90 frontend checks passed.'
