#!/bin/bash

set -e

FILE_PATTERN="data/*.txt"
FILES="$(find ${FILE_PATTERN})"
OBJ_TYPE="p"
cmake --build build

for f in $FILES
do
    FILE_NAME="${f##*/}"
    NAME="${FILE_NAME%.*}"
    OUT=${NAME}_${OBJ_TYPE}_results.txt
    echo "Running ${FILE_NAME}"
    build/main p "$f" > results/${OUT}
    echo "Saved result to ${OUT}"
done
