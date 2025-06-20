#!/bin/bash

set -e

mkdir -p build

cd build

echo "[IRREVERENCE] - STARTED SETUP"
cmake ..
echo "[IRREVERENCE] - FINISHED SETUP"

cd ..

ln -sf build/compile_commands.json .