#!/bin/sh

set -e

mkdir -p build

cd build

echo "[IRREVERENCE] - STARTED WINDOWS SETUP"
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchains/toolchain-windows.cmake -DCMAKE_BUILD_TYPE=Debug -DSDL_STATIC=ON
echo "[IRREVERENCE] - FINISHED WINDOWS SETUP"

cd ..

ln -sf build/compile_commands.json .
