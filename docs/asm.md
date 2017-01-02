ASSEMBLY DOCUMENTATION
======================

####Important:
Before writing any programs, keep in mind a unique quirk of this machine. Instruction 0 will not be read, so all programs should be prefixed with a noop instruction. This will assemble to an instruction of 0's.

####Instructions:
There is allowed only one instruction per line. Comments
begin with a semi colon `;`, everything after the semicolon on a line will be
discarded before parsing.  
Each instruction is of the form `mnemonic [operands...]`, where each token is
delimited by one or more space characters. Other than that, whitespace is
ignored.

####Using the assembler:

    asm input_file [options]
    
    OPTIONS
    -o output_file
        Writes to `output_file'. Without this argument, the default output file is `a.out'.
    
    EXAMPLES
        asm source.asm              #assembles source.asm and writes output to a.out
        asm -o new.bin source.asm   #assembles source.asm and writes output to new.bin
        asm source.asm -o new.bin   #same as above

####Language Syntax:
This assembly is based on the intel style syntax employed by NASM, MASM, TASM, 
etc... Only, there are some notable differences. First, the language employs a context free grammar. That is, a register is always a register, a syscall is always a syscall, an label reference is a label reference, regardless of where these symbols appear in the code.

The types are as follows:

- Syscalls: any of the tokens in the syscall table. A syscall is implemented in the virtual hardware and may not be modified by the programmer. Syscalls are generally used to interface with the host machine.
- Registers: any of the tokens in the register table (see below). Used to simulate the fast access registers in the CPU employed by modern systems.
- Operations: any of the tokens in the operation table (see below). Operations map one-to-one with a corresponding virtual hardware function.
- Label References: alphanumeric word beginning with a ".". Labels can be defined *before or after* they are used.
- Label Definitions: alphanumeric word beginning with a "." and ending with a ":" (e.g. .startLabel5:).
- Constants: numeric word defining a constant (e.g. 5, 0xf, and 0b1011)
- Char: a single character, outlined by single quotes (e.g. 'a', or '\0')
- String: a series of characters, outlined by double quotes (e.g. "Hi there!\0")

####Registers:

You have 11 general purpose registers available for use.  

| Register   | Function                   |
| -----------| -------------------------- |
| R0-R10     | general purpose registers  |
| SP         | stack pointer              |
| IP         | instruction pointer        |
| CMP        | comparison register        |

R10 is also known as the base pointer, and can be referenced via `BP` in the
assembly. This is akin to the base pointer in other languages that serves to
distinguish stack frames.

Registers need no special syntax to be denoted. Simply drop it in as you would
a constant:

    mov R0 R1 ; copy R1's value to R0
    mov R0 52 ; set R0's value to 52

####Constants

32-bit constants can be written in decimal, binary, or hexadecimal:

    ; decimal is just a number:
    mov R0 10 ; move decimal 10 to R0

    ; binary is prefixed with '0b':
    mov R0 0b10 ; move decimal 2 to R0

    ; hexadecimal is prefixed with '0x':
    mov R0 0x10 ; move decimal 16 to R0

####Addressing modes

Constants and registers can be used for indirect addressing like so:

    mov R0 [0x35] ; copy contents of address 0x35 to R0
    mov R0 [R1]   ; copy contents of address pointed by R1 to R0

Offsets are not implemented, but you can get around that using other registers:

    ; instead of mov R0 [R1+8]
    add R1, 8       ; add 8 to R1
    mov R0, [R1] 

Label references are evaluated to 32-bit constants, so you can use them interchangeably:

    ; instead of mov R0, 32
    mov R0, .data_location

####Operations

Here is a list of operations. This will likely be added to as more instructions
become available.

    debug Const Const Const
    ;; Used to print the constants to serial out (as standard out).

    ;mov: copy values to and from registers and memory locations:
        ;Reg Const       ;set Reg value to Const
        ;Reg IndConst    ;set Reg value to memory value reference by Const
        ;RegA RegB       ;copy RegB value to RegA
        ;Reg IndReg      ;copy value in memory pointed by RegB to RegA
    ;;examples (in order)
    mov R0 33
    mov R9 [1024]
    mov R2 R3
    mov R5 [BP]

Okay, the rest you should be able to figure out:

    jump 
        Const 
    jumpE 
        Const 
    jumpNE 
        Const 
    jumpLT 
        Const 
    jumpGT 
        Const 
    cmpRegReg 
        Reg Reg 
    cmpRegConst 
        Reg Const 
    push 
        Reg 
        Const 
    pop 
        Reg 
    xor 
        Reg Reg 
        Reg Const 
    lshift 
        Reg Reg 
        Reg Const 
    rshift 
        Reg Reg 
        Reg Const 
    and 
        Reg Reg 
        Reg Const 
    not 
        Reg 
    add 
        Reg Reg 
        Reg Const 
    sub 
        Reg Reg 
        Reg Const 
    mult 
        Reg Reg 
        Reg Const 
    div 
        Reg Reg 
        Reg Const 
    syscall 
        SysCall 


####System Calls

Interfacing with the host operating system will happen through two major ways:
the most common way is through predefined system calls. These are not interrupt
vectors. These are not accessible by memory and are implemented on the host OS. 

- `dump` : dump the value of registers and other helpful memory information to stdout.
- `exit` : exit the program and terminate the vm.
- `print` : print a string (NIY)


Examples:

    syscall dump ; dump meminfo
    syscall print ; print will use stack arguments
    syscall exit ; will exit with exit code from stack


####I/O

The other way to interface with the host operating system is messaging through
an I/O bus, which has not currently been implemented. I plan on adding `in` and
`out` instructions to achieve this end.

####Multitasking

Multitasking is not currently implemented, but I am working on a model.

CONCLUSION
==========

This basic assembly language should be useful for writing some basic programs
for the simple-vm. Please post feature requests, questions, and bug-reports to the project's github at https://github.com/jameslaydigital/simple-vm/.
