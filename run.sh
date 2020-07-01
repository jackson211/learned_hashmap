#!/usr/bin/env bash

INPUT_FILE=$1

echo "Run learned hashmap"
cmake --build build && ./build/main $INPUT_FILE
