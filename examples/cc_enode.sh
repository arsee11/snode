#!/bin/bash


g++ -o enode -std=c++17 -g enode_test.cpp -I ../src \
	-I/home/arsee/gits/\
	-I/home/arsee/gits/snode/src\
    -L../lib -lenode -lsnode-core -lsnode-cmd -lboost_json -lpthread

#DIR=/home/arsee/tools/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu/bin
#${DIR}/aarch64-none-linux-gnu-g++ -o enode_aarch64 -std=c++17 -g enode_test.cpp \
#    -I ../src -I/home/arsee/gits -I/usr/local/boost_aarch64/include \
#    -L../lib_aarch64 -lsnode-core  -L/usr/local/boost_aarch64/lib -static -lboost_json \
#    -lpthread

