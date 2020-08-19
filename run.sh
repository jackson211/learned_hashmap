#!/usr/bin/env bash

# stop scripts when fail
set -e

OBJ_TYPE=$1
INPUT_FILE=$2
FILE_NAME="${INPUT_FILE##*/}"
NAME="${FILE_NAME%.*}"

mkdir -p results

echo "Run learned hashmap"
./compile.sh
build/main $OBJ_TYPE $INPUT_FILE > results/${NAME}_${OBJ_TYPE}_results.txt

if [ $? -eq 0 ]; then
    echo OK Result saved in results/${NAME}_${OBJ_TYPE}_results.txt
else
    echo FAIL
fi


