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


R1-R8 | general purpose registers
------+---------------------------
SP    | stack pointer
BP    | base pointer
IP    | instruction pointer


Operations are 32 bit integers and operands are 32 bit integers. Every
instruction is therefore two adjacent 32-bit integers, and after execution of
each instruction, the IP will auto-increment by 8 bytes (64 bits). If you think
this is wasteful, go fuck yourself...  


