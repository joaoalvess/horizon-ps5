#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
set -euo pipefail
root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
common=$(git -C "$root" rev-parse --path-format=absolute --git-common-dir)
mount=$(dirname "$(dirname "$common")")
image=${HORIZON_BUILD_IMAGE:-horizon-ps5-build-amd64}
home=${HORIZON_BUILD_HOME:-$mount/home-amd64}
rosetta=0
[[ $(uname -s) == Darwin && $(uname -m) == arm64 ]] && rosetta=1
if ! docker image inspect "$image" >/dev/null 2>&1; then
    docker build --platform linux/amd64 -t "$image" -f "$root/tools/docker/Dockerfile" "$root/tools"
fi
mkdir -p "$home"
exec docker run --rm -t --platform linux/amd64 -v "$mount:$mount" \
    -e HOME="$home" -e CCACHE_DIR="$home/.ccache" -e CCACHE_MAXSIZE=8G \
    -e EDEN_ROSETTA="$rosetta" \
    -w "$root" "$image" "$@"
