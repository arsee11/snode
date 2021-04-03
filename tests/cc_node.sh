#!/bin/bash


g++ -o node -std=c++17 -g node_test.cpp -I ../src/core -I/home/arsee/tests \
    -L../lib -lsnode-core -lboost_json -lpthread

DIR=/home/arsee/tools/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu/bin
${DIR}/aarch64-none-linux-gnu-g++ -o node_aarch64 -std=c++17 -g node_test.cpp \
    -I ../src/core -I/home/cl/tests -I/usr/local/boost_aarch64/include \
    -L../lib_aarch64 -lsnode-core  -L/usr/local/boost_aarch64/lib -static -lboost_json \
    -lpthread

