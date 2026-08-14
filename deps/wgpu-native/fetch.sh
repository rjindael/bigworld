#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Downloads the prebuilt wgpu-native release for the host platform into
# deps/wgpu-native/{include,lib}. Not vendored in git (see .gitignore) since
# it's a ~15MB prebuilt binary; fetched once and cached like any build tool.
set -e

VERSION="v29.0.1.1"
DIR="$(cd "$(dirname "$0")" && pwd)"

if [ -f "$DIR/lib/libwgpu_native.a" ]; then
    exit 0
fi

case "$(uname -s)" in
    Linux) OS="linux" ;;
    Darwin) OS="macos" ;;
    *) echo "fetch.sh: unsupported OS $(uname -s), see https://github.com/gfx-rs/wgpu-native/releases/$VERSION" >&2; exit 1 ;;
esac

case "$(uname -m)" in
    x86_64|amd64) ARCH="x86_64" ;;
    aarch64|arm64) ARCH="aarch64" ;;
    *) echo "fetch.sh: unsupported arch $(uname -m)" >&2; exit 1 ;;
esac

ASSET="wgpu-${OS}-${ARCH}-release.zip"
URL="https://github.com/gfx-rs/wgpu-native/releases/download/${VERSION}/${ASSET}"

echo "Fetching wgpu-native ${VERSION} (${ASSET})..."
curl -sL -o "$DIR/$ASSET" "$URL"

rm -rf "$DIR/include" "$DIR/lib"
unzip -q "$DIR/$ASSET" -d "$DIR"
rm -f "$DIR/$ASSET"
