#!/bin/bash

g++ --std=c++20 -fopenmp -I include/ -std=c++20 src/astar.cpp -o astar
python main.py
# ./astar
rm -f astar