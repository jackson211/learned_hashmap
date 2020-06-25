#!/usr/bin/env bash

INPUT_FILE='test3.txt'
cmake --build build && ./build/main $INPUT_FILE
