#!/usr/bin/env bash

INPUT_FILE=$1

echo "Run RTree"
make && ./test_rtree $INPUT_FILE
