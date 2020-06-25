#!/usr/bin/env bash

INPUT_FILE='nodes.txt'
cmake --build build && ./build/main $INPUT_FILE
