#!/usr/bin/env bash

INPUT_FILE='test.txt'

echo "Run RTree"
make && ./test_rtree "../$INPUT_FILE"
