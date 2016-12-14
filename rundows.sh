#!/usr/bin/bash

#first, assemble the source:
./bin/asm -o ./a.out source.asm

#now compile and run the VM against the assembled object
gcc -o ./bin/vm.exe ./build/vm.c -Wall && ./bin/vm.exe ./a.out
