#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$ROOT/build-ubuntu24"
PREFIX="${PREFIX:-/usr/local}"
LIBDIR="$PREFIX/lib/lcc"
BINDIR="$PREFIX/bin"
NASM_VERSION="0.98.39"
NASM_URL="https://ftp2.osuosl.org/pub/blfs/conglomeration/nasm/nasm-${NASM_VERSION}.tar.bz2"
NASM_TARBALL="$BUILD/nasm-${NASM_VERSION}.tar.bz2"

log(){ printf '\n==> %s\n' "$*"; }
die(){ echo "ERROR: $*" >&2; exit 1; }

if [[ -f /etc/os-release ]]; then
    . /etc/os-release
    if [[ "${ID:-}" != "ubuntu" || "${VERSION_ID:-}" != "24.04" ]]; then
        echo "WARNING: this script is intended for Ubuntu 24.04; detected ${PRETTY_NAME:-unknown}." >&2
    fi
fi

if [[ $EUID -eq 0 ]]; then SUDO=""; else SUDO="sudo"; fi

log "Installing Ubuntu build dependencies"
$SUDO apt-get update
$SUDO apt-get install -y build-essential gcc-multilib libc6-dev-i386 nasm wget curl bzip2 tar file python3

mkdir -p "$BUILD"

log "Obtaining historical LCC x86 NASM backend"
if [[ ! -f "$NASM_TARBALL" ]]; then
    wget -O "$NASM_TARBALL" "$NASM_URL"
fi
rm -rf "$BUILD/nasm-src"
mkdir -p "$BUILD/nasm-src"
tar -xjf "$NASM_TARBALL" -C "$BUILD/nasm-src"
NASMDIR="$(find "$BUILD/nasm-src" -type d -name lcc -print -quit)"
[[ -n "$NASMDIR" ]] || die "NASM archive does not contain the historical lcc port."
[[ -f "$NASMDIR/x86nasm.md" ]] || die "Historical x86nasm.md not found."
cp "$NASMDIR/x86nasm.md" "$ROOT/src/x86nasm.md"

log "Preparing Ubuntu x86/ELF host driver"
cat > "$ROOT/etc/linux-x86.c" <<HOST
#define LCCDIR_PLACEHOLDER
char *cpp[] = { LCCDIR "cpp", "-D__STDC__=1", 0 };
char *include[] = { "-I" LCCDIR "include", "-I/usr/local/include", 0 };
char *com[] = { LCCDIR "rcc", "-target=x86-linux", "\$1", "\$2", "\$3", 0 };
char *as[] = { "$(command -v nasm)", "-f", "elf32", "-o", "\$3", "\$1", 0 };
char *ld[] = { "/usr/bin/gcc", "-m32", "-o", "\$3", "\$1", "\$2", 0 };
HOST
