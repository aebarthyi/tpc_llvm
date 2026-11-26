#!/usr/bin/env bash
# Build the tpc-llvm release packages for the Ubuntu release this runs on.
#
#   tpc/release/build-deb.sh [-j N] [-b BUILD_DIR] [-o OUT_DIR] [--no-test]
#                            [--allow-dirty]
#
# Run it on Ubuntu 22.04 or 24.04, x86-64: the binaries need the glibc of the
# release they were built on, or newer. For another release than the host's,
# use in-container.sh. Needs:
#   sudo apt install cmake ninja-build g++ python3 git dpkg-dev \
#                    zlib1g-dev libzstd-dev
#
# Builds tpc-clang and tpc-llvm-objdump (Release, assertions off, TPC target
# only, as buildTPC_CLANG.sh), runs the TPC lit tests on that build, and
# writes to OUT_DIR (default ./dist):
#   tpc-llvm_<version>_ubuntu-<release>_amd64.deb      installs /opt/tpc-llvm
#   tpc-llvm-<version>-ubuntu-<release>-x86_64.tar.gz  tpc-llvm/, unpack anywhere
#   SHA256SUMS-ubuntu-<release>
# <version> is tpc/release/VERSION. BUILD_DIR defaults to
# ./build-release-ubuntu-<release>. -j defaults to one job per 3 GB of memory,
# at most one per core: some of clang's files take 2-3 GB each to compile
# with GCC, and more jobs than that ran a 46 GB machine out of memory.
# MAINTAINER sets the package's Maintainer field.

set -euo pipefail
umask 022

SRC=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
VERSION=$(cat "$SRC/tpc/release/VERSION")
PREFIX=/opt/tpc-llvm
HOMEPAGE=https://github.com/aebarthyi/tpc_llvm
MAINTAINER=${MAINTAINER:-"tpc-llvm <$HOMEPAGE>"}
MEM_GB=$(awk '/^MemTotal:/ { print int($2 / 1048576) }' /proc/meminfo)
JOBS=$((MEM_GB / 3))
[ "$JOBS" -ge 1 ] || JOBS=1
[ "$JOBS" -le "$(nproc)" ] || JOBS=$(nproc)
BUILD=
OUT=$PWD/dist
TEST=1
ALLOW_DIRTY=0

# The complete TPC test set.
LIT_DIRS=(
  clang/test/RC99
  llvm/test/CodeGen/TPC
  llvm/test/Analysis/CostModel/TPC
  llvm/test/LTO/TPC
  llvm/test/Transforms/LICM/TPC
  llvm/test/Transforms/LoopUnroll/TPC
  llvm/test/Transforms/CastSwizzleOpt
  llvm/test/Transforms/LoopSWPPass
  llvm/test/Transforms/IterationInterleave
  llvm/test/Transforms/LoopTaken
  llvm/test/Transforms/EarlyCSE/TPC
)
# What those tests run. lit falls back to PATH for a tool the build lacks, so
# a missing one shows up only on a machine without LLVM installed.
LIT_TOOLS=(llc opt FileCheck not count split-file llvm-mc llvm-objcopy
           llvm-readobj llvm-readelf llvm-nm llvm-as llvm-dis llvm-config)

die() { echo "build-deb.sh: error: $*" >&2; exit 1; }
usage() { sed -n '2,24p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit "$1"; }

while [ $# -gt 0 ]; do
  case $1 in
    -j) JOBS=$2; shift ;;
    -b) BUILD=$2; shift ;;
    -o) OUT=$2; shift ;;
    --no-test) TEST=0 ;;
    --allow-dirty) ALLOW_DIRTY=1 ;;
    -h|--help) usage 0 ;;
    *) echo "build-deb.sh: unknown option $1" >&2; usage 1 >&2 ;;
  esac
  shift
done

[ -r /etc/os-release ] || die "no /etc/os-release; run this on Ubuntu"
# In a subshell: os-release sets VERSION, NAME, ... of its own.
OS=$(. /etc/os-release && echo "${ID:-unknown}")
REL=$(. /etc/os-release && echo "${VERSION_ID:-}")
[ "$OS" = ubuntu ] || die "run this on Ubuntu (this is $OS)"
for t in cmake ninja g++ python3 git dpkg-deb dpkg-shlibdeps; do
  command -v "$t" >/dev/null ||
    die "$t not found: sudo apt install cmake ninja-build g++ python3 git dpkg-dev zlib1g-dev libzstd-dev"
done
ARCH=$(dpkg --print-architecture)
[ "$ARCH" = amd64 ] || die "amd64 only (this is $ARCH)"

if [ "$ALLOW_DIRTY" = 0 ] &&
   [ -n "$(git -C "$SRC" status --porcelain --untracked-files=no)" ]; then
  die "$SRC has uncommitted changes (--allow-dirty builds them anyway)"
fi
REV=$(git -C "$SRC" rev-parse --short=12 HEAD)
BUILD=$(mkdir -p "${BUILD:-$PWD/build-release-ubuntu-$REL}" && cd "${BUILD:-$PWD/build-release-ubuntu-$REL}" && pwd)
mkdir -p "$OUT"
OUT=$(cd "$OUT" && pwd)

echo "== tpc-llvm $VERSION ($REV) for Ubuntu $REL, build in $BUILD"
LINKER=()
if command -v ld.lld >/dev/null; then LINKER=(-DLLVM_USE_LINKER=lld); fi
cmake -G Ninja -S "$SRC/llvm" -B "$BUILD" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DLLVM_ENABLE_PROJECTS=clang \
  -DLLVM_TARGETS_TO_BUILD=TPC \
  -DLLVM_BUILD_EXAMPLES=OFF \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_INCLUDE_BENCHMARKS=OFF \
  -DCLANG_BUILD_EXAMPLES=OFF \
  -DLLVM_ENABLE_ZLIB=FORCE_ON \
  -DLLVM_ENABLE_ZSTD=FORCE_ON \
  -DLLVM_ENABLE_LIBXML2=OFF \
  -DLLVM_PARALLEL_LINK_JOBS=2 \
  -DTPC_LLVM_VERSION="$VERSION" \
  "${LINKER[@]}"

TARGETS=(clang llvm-objdump clang-resource-headers)
if [ "$TEST" = 1 ]; then TARGETS+=("${LIT_TOOLS[@]}"); fi
ninja -C "$BUILD" -j "$JOBS" "${TARGETS[@]}"

if [ "$TEST" = 1 ]; then
  echo "== lit"
  (cd "$SRC" && "$BUILD/bin/llvm-lit" -s -j "$JOBS" "${LIT_DIRS[@]}")
fi

echo "== package"
STAGE=$(mktemp -d)
trap 'rm -rf "$STAGE"' EXIT
# The layout dpkg-shlibdeps expects: debian/control, and the package's files
# under debian/<package> next to its DEBIAN directory.
ROOT=$STAGE/debian/tpc-llvm
P=$ROOT$PREFIX
install -d "$ROOT/DEBIAN"
RES=$(cd "$BUILD/lib/clang" && ls)          # resource directory, e.g. 23
[ "$(echo "$RES" | wc -l)" = 1 ] || die "more than one $BUILD/lib/clang/*"
install -d "$P/bin" "$P/lib/clang/$RES" "$P/share/doc/tpc-llvm" \
  "$ROOT/usr/share/doc/tpc-llvm"
# The real binaries under their TPC names only, so that /opt/tpc-llvm/bin in
# PATH does not hide a system clang or llvm-objdump. The driver takes its
# target from the name tpc-clang and finds the headers at ../lib/clang.
install -m 755 "$(readlink -f "$BUILD/bin/tpc-clang")" "$P/bin/tpc-clang"
install -m 755 "$(readlink -f "$BUILD/bin/llvm-objdump")" \
  "$P/bin/tpc-llvm-objdump"
cp -R "$BUILD/lib/clang/$RES/include" "$P/lib/clang/$RES/"
install -m 644 "$SRC/LICENSE.TXT" "$SRC/README.md" "$P/share/doc/tpc-llvm/"
{
  echo "tpc-llvm $VERSION, built from $HOMEPAGE at $REV."
  echo "The LLVM project, Habana Labs' TPC fork and this fork are under the"
  echo "license below."
  echo
  cat "$SRC/LICENSE.TXT"
} > "$ROOT/usr/share/doc/tpc-llvm/copyright"
chmod 644 "$ROOT/usr/share/doc/tpc-llvm/copyright"

# Only files of this tree go in: every header comes from clang/lib/Headers, or
# is generated from it by the build (the HLSL intrinsic tables).
(cd "$P/lib/clang/$RES/include" && find . -type f | sed 's|^\./||') |
  while read -r h; do
    [ -f "$SRC/clang/lib/Headers/$h" ] ||
      [ -f "$BUILD/tools/clang/lib/Headers/$h" ] ||
      die "lib/clang/$RES/include/$h is neither in clang/lib/Headers nor generated"
  done

printf 'Source: tpc-llvm\n\nPackage: tpc-llvm\nArchitecture: any\n' \
  > "$STAGE/debian/control"
DEPENDS=$(cd "$STAGE" &&
  dpkg-shlibdeps -O -e "debian/tpc-llvm$PREFIX/bin/tpc-clang" \
    -e "debian/tpc-llvm$PREFIX/bin/tpc-llvm-objdump" |
  sed -n 's/^shlibs:Depends=//p')
[ -n "$DEPENDS" ] || die "dpkg-shlibdeps found no dependencies"
SIZE=$(du -sk --exclude=DEBIAN "$ROOT" | cut -f1)
cat > "$ROOT/DEBIAN/control" <<EOF
Package: tpc-llvm
Version: $VERSION-1~ubuntu$REL
Architecture: $ARCH
Maintainer: $MAINTAINER
Installed-Size: $SIZE
Depends: $DEPENDS
Section: devel
Priority: optional
Homepage: $HOMEPAGE
Description: TPC C compiler for Intel Gaudi accelerators
 tpc-clang compiles TPC C kernels for the Tensor Processor Cores of Intel
 Gaudi accelerators, and tpc-llvm-objdump disassembles them. It is based
 on LLVM and installs to $PREFIX, apart from the tpc-clang of
 habanalabs-tools.
EOF

NAME=tpc-llvm-$VERSION-ubuntu-$REL
DEB=$OUT/tpc-llvm_${VERSION}_ubuntu-${REL}_amd64.deb
TGZ=$OUT/$NAME-x86_64.tar.gz
dpkg-deb --root-owner-group -Zxz --build "$ROOT" "$DEB"
tar -C "$(dirname "$P")" --owner=0 --group=0 --sort=name -czf "$TGZ" \
  "$(basename "$P")"

echo "== smoke test of the package"
T=$STAGE/smoke
dpkg-deb -x "$DEB" "$T"
CC=$T$PREFIX/bin/tpc-clang
BANNER=$("$CC" --version | sed -n 1p)
echo "$BANNER"
case $BANNER in *"(tpc-llvm $VERSION)"*) ;; *) die "--version lacks tpc-llvm $VERSION" ;; esac
cat > "$STAGE/smoke.c" <<'EOF'
void main(tensor in, tensor out) {
  int5 c = get_index_space_offset();
  float64 x = v_f32_ld_tnsr_b(c, in);
  v_f32_st_tnsr(c, out, x * 2.0f);
}
EOF
for a in gaudi gaudi2; do
  "$CC" -march=$a -O2 -c "$STAGE/smoke.c" -o "$STAGE/smoke-$a.o"
  "$T$PREFIX/bin/tpc-llvm-objdump" --triple=tpc --mcpu=$a -d \
    "$STAGE/smoke-$a.o" > "$STAGE/smoke-$a.dis"
  grep -q halt "$STAGE/smoke-$a.dis" || die "no halt in the $a kernel"
done

(cd "$OUT" && sha256sum "$(basename "$DEB")" "$(basename "$TGZ")" \
  > "SHA256SUMS-ubuntu-$REL")
echo "== done"
dpkg-deb --field "$DEB" Package Version Depends Installed-Size
ls -l "$DEB" "$TGZ" "$OUT/SHA256SUMS-ubuntu-$REL"
