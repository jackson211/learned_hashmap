#!/usr/bin/env bash

# stop scripts when fail
set -e

OBJ_TYPE=$1
INPUT_FILE=$2

echo "Run learned hashmap"
./compile.sh
build/main $OBJ_TYPE $INPUT_FILE
