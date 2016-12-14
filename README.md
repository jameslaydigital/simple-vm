VIRTUAL MACHINE
===============

Instructions to run:  
If you're on Windows, run `rundows.sh`.  
If you're on Unix, Linux, or MacOs, use `runnix.sh`.  
This will assemble the source.asm assembly file to bytecode at a.out, then build the source for the vm, and finally, execute the vm against the `a.out` object.
If you've already compiled the vm, you can add it to your path and run `vm bytecode_file`, where `bytecode_file` can be any binary file.

DESCRIPTION
===========

Simple 32-bit, little endian virtual machine in which each instruction,
including both the opcode and operands, is exactly 8 bytes long (64 bits).
Thus, the instruction pointer can increment by a fixed amount.  

Each operation can take anywhere from 0 to 3 operands, but since the instruction is fixed length, the number of operands an operation can have changes the size of the operands.
For example:

    not R0;         0x19  0x00  0x00  0x00  0x00  0x00  0x00  0x00
                    └─opcode─┘  └─regnum─┘  └───────not used─────┘

    debug 1 2 3;    0x00  0x00  0x00  0x00  0x00  0x00  0x00  0x00
                    └─opcode─┘  └─op 1───┘  └─op 2───┘  └─op 3───┘

    syscall dump;   0x10  0x00  0x00  0x00  0x01  0x00  0x00  0x00
                    └─opcode─┘  └not used┘  └───syscall vector───┘
    
The focus here is simplicity.

The executable file format is basically a disk image that is copied to the heap
verbatim, meaning byte 0 in the file is byte 0 on the heap.

Instructions are executed off the heap beginning at heap address 8.
There are 11 general purpose registers, R0-R10 (R10 is also known as BP,
typically used as the base pointer), a stack pointer SP, and an instruction
pointer IP.  

REGISTERS:


| Register   | Function                   |
| -----------| -------------------------- |
| R0-R10     | general purpose registers  |
| SP         | stack pointer              |
| IP         | instruction pointer        |
| CMP        | comparison register        |


MEMORY: HEAP AND STACK
======================

The heap and stack coexist on the same buffer, so overflows are possible and
must be carefully considered when designing a program. You are at liberty to
move the stack pointer to any byte location, which means you can create
alignment issues which will segfault the host vm. Be mindful of that.

ASSEMBLY LANGUAGE
=================

This vm comes with a very simple assembler that is loosely inspired by the
intel-style syntax employed by NASM. The assembler requires python 2.7 and
lives in `./bin/asm` and takes the source file as its argument.

    asm -o output.o source_file.asm

There's a manual at `./docs/asm.md` detailing the syntax and available
instructions.  

PERFORMANCE FEATURES IN DEVELOPMENT
===================================

Each opcode is essentially an index to an array of functions in C instead of a
hashmap. Memory cannot currently be allocated dynamically from the host
operating system, but a syscall is in development to add this feature. Another
consideration is using this software on a big-endian system. It should work,
but I plan to optimize performance when running on little-endian systems via a
compiler flag. Though I haven't performed any official benchmarks, it should
run pretty quick. I'll post a runtime analysis here soon.

CONSIDERATIONS
==============

It is important to padd the beginning of the file with a null instruction
(debug 0 0 0). This is because execution starts at byte 8. This is a technical
necessity that comes from the behavior of the instruction pointer. When a jump
is issued, the jump operation sets the IP register to your location minus 8
bytes. This is because after each operation, the IP is auto-incremented by 8
bytes. Thus, there is no way to ever jump to instruction 0 because that would
put the IP register's value at -8, which is not possible because it is an
unsigned value.

The entire file is read into memory, including the EOF signatures at the end of
the file (0x0a).  
  
There is no magic number or separation between data and instructions. This
means this vm could easily be exploited for the purpose of privilege
escalation.  

