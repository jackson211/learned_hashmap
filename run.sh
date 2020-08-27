#!/usr/bin/env bash

# stop scripts when fail
set -e

OBJ_TYPE=$1
INPUT_FILE=$2
FILE_NAME="${INPUT_FILE##*/}"
NAME="${FILE_NAME%.*}"
OUT=${NAME}_${OBJ_TYPE}_results.txt

mkdir -p results

echo "Run learned hashmap on ${FILE_NAME}"
cmake --build build
build/main $OBJ_TYPE $INPUT_FILE > results/${OUT}

