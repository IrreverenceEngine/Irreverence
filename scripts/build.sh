#!/bin/sh

set -e

mkdir -p build
cd build

echo "[IRREVERENCE] - STARTED BUILDING"
cmake --build . -j8
echo "[IRREVERENCE] - FINISHED BUILDING"

cd ..

./scripts/clear_shaders.sh