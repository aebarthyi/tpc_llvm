#!/usr/bin/env bash
# Run build-deb.sh in an Ubuntu container, to build the packages of an Ubuntu
# release other than the host's.
#
#   tpc/release/in-container.sh RELEASE [build-deb.sh options]
#
# RELEASE is 22.04 or 24.04. Needs docker or podman. The source tree is
# mounted read-only; the build directory and dist/ go in the current
# directory, as with build-deb.sh, owned by the calling user.

set -euo pipefail

[ $# -ge 1 ] || { sed -n '2,9p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit 1; }
REL=$1; shift
SRC=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
ENGINE=$(command -v podman || command -v docker || true)
[ -n "$ENGINE" ] || { echo "in-container.sh: needs docker or podman" >&2; exit 1; }
# Rootless podman maps the container's root to the calling user already;
# docker's root is the host's, so the outputs are given back to the user.
case $ENGINE in
  *podman) OWNER=0:0 ;;
  *) OWNER=$(id -u):$(id -g) ;;
esac
ENVS=(-e "OWNER=$OWNER")
if [ -n "${MAINTAINER:-}" ]; then ENVS+=(-e "MAINTAINER=$MAINTAINER"); fi

"$ENGINE" run --rm \
  -v "$SRC:/src:ro" -v "$PWD:/work" -w /work \
  "${ENVS[@]}" \
  "docker.io/library/ubuntu:$REL" bash -c '
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq --no-install-recommends cmake ninja-build g++ \
  python3 git dpkg-dev zlib1g-dev libzstd-dev ca-certificates >/dev/null
git config --global --add safe.directory "*"
status=0
/src/tpc/release/build-deb.sh "$@" || status=$?
chown -R "$OWNER" /work/dist /work/build-release-ubuntu-* 2>/dev/null || true
exit $status
' in-container "$@"
