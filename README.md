# Learned HashMap
> HashMap that optimized with regression model.

## Installation
Requirement: cmake  

To build
```sh
cmake --build build
```

## Usage example

To run Learned HashMap:
```sh
./run.sh [OBJECT_TYPE] [DATA_FILE]
```
Example point file:
```sh
./run.sh p test.txt
```
Example object file:
```sh
./run.sh o melbourne.txt
```

To run RTree:
```sh
cd rtree
./run_rtree.sh [DATA_FILE]
```
Example:
```sh
cd rtree
./run_rtree.sh ../test.txt
```