#!/usr/bin/env bash

INPUT_FILE='NE.txt'
cmake --build build && ./build/main $INPUT_FILE
