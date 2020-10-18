#!/usr/bin/env bash

INPUT_FILE=$1
FILE_NAME="${INPUT_FILE##*/}"
NAME="${FILE_NAME%.*}"
OUT=rtree_${NAME}_range_results.txt

echo "Run RTree"
make && (gtime -v ./test_rtree $INPUT_FILE) &> ../results/${OUT}
