#!/bin/sh

set -e

mkdir -p build
cd build

echo "[IRREVERENCE] - STARTED BUILDING"
cmake --build . -j$(nproc)
echo "[IRREVERENCE] - FINISHED BUILDING"

cd ..