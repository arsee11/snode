#!/bin/bash

g++ -o node -std=c++11 -g node_test.cpp -I ../../ -L./ -lsnode -lpthread

g++ -o snode -std=c++11 -g snode_test.cpp -I ../../ -L./ -lsnode
