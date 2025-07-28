#!/bin/sh

set -e

mkdir -p build

cd build

echo "[IRREVERENCE] - STARTED SETUP"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DSDL_STATIC=ON
echo "[IRREVERENCE] - FINISHED SETUP"

cd ..

ln -sf build/compile_commands.json .