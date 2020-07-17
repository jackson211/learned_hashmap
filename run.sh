#!/usr/bin/env bash

OBJ_TYPE=$1
INPUT_FILE=$2

echo "Run learned hashmap"
cmake --build build && ./build/main $OBJ_TYPE $INPUT_FILE
