#!/usr/bin/env bash

INPUT_FILE='NE.txt'

echo "Run learned hashmap"
cmake --build build && ./build/main $INPUT_FILE
