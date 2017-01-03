#!/usr/local/bin/bash

gcc -o ./bin/vm ./build/vm.c -Wall && bin/asm source.asm >/dev/null && ./bin/vm ./a.out
