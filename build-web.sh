#!/bin/sh
# Builds the browser version: dist/index.html (one self-contained page).
# Needs:
#   wasi-sdk   https://github.com/WebAssembly/wasi-sdk/releases   (set WASI_SDK)
#   binaryen   https://github.com/WebAssembly/binaryen/releases   (set BINARYEN)
#   Node.js
set -e
cd "$(dirname "$0")"
: "${WASI_SDK:?set WASI_SDK to your wasi-sdk folder}"
: "${BINARYEN:?set BINARYEN to your binaryen folder}"
mkdir -p build
"$WASI_SDK/bin/clang++" --target=wasm32-wasip1 --sysroot="$WASI_SDK/share/wasi-sysroot" \
  -std=c++17 -O2 -w -fno-exceptions -Iplatform/web -Iplatform \
  game/*.cpp platform/platform.cpp -o build/mopmania.raw.wasm -Wl,-z,stack-size=1048576
# Asyncify lets the page pause the game while it waits for typing or sleeps.
"$BINARYEN/bin/wasm-opt" build/mopmania.raw.wasm --asyncify \
  --pass-arg=asyncify-imports@wasi_snapshot_preview1.fd_read,wasi_snapshot_preview1.poll_oneoff,mopmania.wait_key \
  -O2 --strip-debug -o web/mopmania.wasm
node web/build-page.js
