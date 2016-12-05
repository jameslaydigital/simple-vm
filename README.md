VIRTUAL MACHINE
===============

Simple 32-bit virtual machine in which each operation takes only one operand.
Code is read 64-bits at a time; if you break that 64-bit number into two 32-bit
components, the left number is your opcode and the right number is your
operand.  

The focus here is simplicity and ease of learning.  

The executable file format is basically a disk image that is copied to the heap
verbatim, meaning byte 0 in the file is byte 0 on the heap. The architecture is
big-endian, which should simplify writing binary code by hand (perhaps using
xxd). The entire file is read into memory, including the EOF signatures. Thus,
if you need to begin the vm with more heap space, pad the file with nulls.  

There is no magic number or separation between data and instructions. This
means this vm could easily be exploited for the purpose of privilege
escalation. However, the stack is seperate from the heap. This helps the
virtual machine capitalize on the available resources of the host machine
without overflowing the stack into the heap.  

Instructions are executed off the heap beginning at heap address 0.
There are 8 general purpose registers, R1-R8, a stack pointer SP, a base
pointer BP, and an instruction pointer IP.  

REGISTERS:


| Register   | Function                   |
| -----------| -------------------------- |
| R1-R8      | general purpose registers  |
| SP         | stack pointer              |
| BP         | base pointer               |
| OP         | offset pointer             |
| IP         | instruction pointer        |
| CMP        | comparison register        |


Operations are 32 bit integers and operands are 32 bit integers. Every
instruction is therefore two adjacent 32-bit integers, and after execution of
each instruction, the IP will auto-increment by 8 bytes (64 bits). If you think
this is wasteful, go fuck yourself...  


LOAD INSTRUCTIONS AND ADDRESSING
================================

I had to take some liberties in order to make this whole "one operand per opcode" thing work.
Ultimately, it could also be thought of that each instruction is 64 bits long
and contains an operation of 1-2 bytes and the rest could be any number of
operands. Which ever way you prefer to think of it is fine; that's the beauty
of it.  

MEMORY: HEAP AND STACK
======================

Soooo, the head and stack are seperate in this architecture.
I made this decision to increase the simplicity of the design and to mitigate
stack overflows. It is because of this, however, that you need seperate
instructions for accessing each. No big deal, just a couple more instructions.
I said it was simple, not that it was RISC.  
By the way, code lives on the heap, so heap access can self-modify code.  

PERFORMANCE
===========

Each opcode is mapped to a function. There's a number of ways to achieve this
internally, via a linked-list (ew), a hashmap (eh), or an actual offset of an
array (yea!). I chose the last.
