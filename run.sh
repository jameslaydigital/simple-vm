#!/usr/bin/bash

g++ -std=gnu++11 -o ./bin/vm ./vm.cpp && ./bin/vm ./source.bc
