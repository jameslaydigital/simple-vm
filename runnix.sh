#!/usr/local/bin/bash

gcc -o ./bin/vm ./build/vm.c -Wall && ./bin/vm ./a.out
