#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/defs.h"
#include "headers/vm.h"
#include "lib/ops.cpp"
#include "lib/syscalls.cpp"

//TODO: start making operations. Put them in a header file?

int main(int argc, char *argv[]) {
    if ( argc != 2 ) return fail(ERR_ARG_COUNT);
    if ( loadBuffer(argv[1]) > 0 ) return fail(ERR_FILE);
    loadFunctionTable();
    loadSysCalls();
    if ( executionLoop() > 0 ) return fail(ERR_CMD_BUFF);
    return 0;
}

void loadSysCalls() {
    //syscalls will use a C-flavored calling convention. That is, they will
    //take arguments off the stack, instead of using your special
    //general-purpose registers.
    _syscalls[0x00] = &sys_dump;
}

void loadFunctionTable() {
    //each command is composed of 8 characters (8 bytes, or 64 bits).
    //the first half is the instruction, the second half is the data.
    //So, this is a 32-bit architecture.
    //Officially, each op can only support one operand.
    //For example, there will be 8 load opcodes, one for each register.
    //There will be 9 push opcodes, one for each register plus one for pushing constants.
    //There will be 8 pop opcodes, one for each register.
    //There will be 2 add opcodes, R1=R1+register, R1=R1+constant.
    //There will be 2 mult opcodes, R1=R1*register, R1=R1*constant.
    //There will be 2 div opcodes, R1=R1/register, R1=R1/constant.

//==LOAD========================//
    _fmap[0x00] = &loadR1const;
    _fmap[0x01] = &loadR2const;
    _fmap[0x02] = &loadR3const;
    _fmap[0x03] = &loadR4const;
    _fmap[0x04] = &loadR5const;
    _fmap[0x05] = &loadR6const;
    _fmap[0x06] = &loadR7const;
    _fmap[0x07] = &loadR8const;
    _fmap[0x08] = &loadR1mem;
    _fmap[0x09] = &loadR2mem;
    _fmap[0x0a] = &loadR3mem;
    _fmap[0x0b] = &loadR4mem;
    _fmap[0x0c] = &loadR5mem;
    _fmap[0x0d] = &loadR6mem;
    _fmap[0x0e] = &loadR7mem;
    _fmap[0x0f] = &loadR8mem;
//==SYSCALL=====================//
    _fmap[0x10] = &syscall_op;  //syscall
//==PUSH========================//
    _fmap[0x11] = &push;
//==POP=========================//
    _fmap[0x12] = &pop;
//==ADD=========================//
    _fmap[0x13] = &addR1reg;    //R1=R1+register
    _fmap[0x14] = &addR1const;  //R1=R1+constant
//==SUB=========================//
    _fmap[0x15] = &subR1reg;    //R1=R1*register
    _fmap[0x16] = &subR1const;  //R1=R1*constant
//==MULT========================//
    _fmap[0x17] = &multR1reg;    //R1=R1*register
    _fmap[0x18] = &multR1const;  //R1=R1*constant
//==DIV=========================//
    _fmap[0x19] = &divR1reg;    //R1=R1/register
    _fmap[0x1A] = &divR1const;  //R1=R1/constant
//==CMP=========================//
    _fmap[0x1B] = &cmpRegs;     //Compare R1 to another register
//==JUMP========================//
    _fmap[0x1B] = &jump;        //jump to register location
    //Jump is interesting. You always jump to the location in R1. The operand
    //is a number from 0-7 that indicates the condition.
}

int executionLoop() {
    while ( _regs[IP] < (_buff.size()-INSTRUCTION_BYTE_SIZE) ) {
        execute(_regs[IP]);
        _regs[IP] += INSTRUCTION_BYTE_SIZE;
    }
    return 0;
}

//big-endian
int toInt32(char a, char b, char c, char d) {
    int intA = ((int)d); //LSB
    int intB = ((int)c)<<8;
    int intC = ((int)b)<<16;
    int intD = ((int)a)<<24; //MSB
    return intA+intB+intC+intD;
}

int execute(int loc) {
    int opcode, operand;
    op fn;
        opcode  = toInt32(
            _buff.at(loc),
            _buff.at(loc+1),
            _buff.at(loc+2),
            _buff.at(loc+3));
        operand = toInt32(_buff.at(loc+4),
            _buff.at(loc+5),
            _buff.at(loc+6),
            _buff.at(loc+7));

    try {
        fn = _fmap.at(opcode);
    } catch(...) {
        printf("\nNo operation in op table for opcode '%#1x'.", opcode);
        return 1;
    }
    fn(operand);
    return 0;
}

int loadBuffer(char *fname) {
    std::ifstream src(fname);
    char c;
    if( src.is_open() ) {
        while(src.good()) {
            src.get(c);
            printf("%#1x ", (int)c);
            _buff.push_back(c);
        }
        return 0;
    }
    std::cout<<"Couldn't open file."<<std::endl;
    return 1;
}

int fail( int failure_code ) {
    std::cout<<"Error encountered:"<<std::endl;
    if ( failure_code == ERR_FILE ) {
        std::cout<<"File could not be opened."<<std::endl;
    }

    if ( failure_code == ERR_ARG_COUNT ) {
        std::cout<<"Not enough arguments."<<std::endl;
        std::cout<<"Usage: vm source_file"<<std::endl;
    }
    return 1;
}
