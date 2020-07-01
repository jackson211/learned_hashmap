#!/usr/bin/env bash

INPUT_FILE='nodes.txt'

echo "Run learned hashmap"
cmake --build build && ./build/main $INPUT_FILE
