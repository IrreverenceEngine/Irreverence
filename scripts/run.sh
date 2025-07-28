#!/bin/sh

set -e

./scripts/build.sh

cd game

echo
echo [_______________GAME_______________]

prime-run ./irreverence