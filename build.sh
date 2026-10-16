#!/bin/sh
# Builds Mop Mania to run in a terminal on macOS or Linux.
#   ./build.sh            then:   cd game && ../build/mopmania
# Needs a C++17 compiler (g++ or clang++). On a Mac, run: xcode-select --install
set -e
cd "$(dirname "$0")"
mkdir -p build
CXX="${CXX:-c++}"
"$CXX" -std=c++17 -O2 -w -Iplatform game/*.cpp platform/platform.cpp -o build/mopmania
echo "Built build/mopmania"
echo "Play it with:  cd game && ../build/mopmania"
