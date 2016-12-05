#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/utils.h"
#include "headers/defs.h"
#include "lib/ops.c"
#include "lib/syscalls.c"
#define MAX_NUM_OPS 0xff

int main(int argc, char *argv[]) {
    printf("Firing up the virtual machine...\n\n");
    printf("OPERATION FEED:\n");
    //initialize instruction pointer
    _regs[IP] = INSTRUCTION_BYTE_SIZE*1; //program starts at instruction 0
    if ( argc != 2 ) fail(ERR_ARG_COUNT);
    loadBuffer(argv[1]);
    loadFunctionTable();
    loadSysCalls();
    if ( executionLoop() > 0 ) fail(ERR_CMD_BUFF);
    sys_dump();
    return 0;
}

void loadSysCalls() {
    //syscalls will use a C-flavored calling convention. That is, they will
    //take arguments off the stack, instead of using your special
    //general-purpose registers.
    _syscalls[0x00] = &sys_dump;
}

void loadFunctionTable() {

//==MOV=========================//
    _fmap[0x00] = &debugOp;             //works
    _fmap[0x01] = &movRegConst;         //works
    _fmap[0x02] = &movRegIndConst;
    _fmap[0x03] = &movRegReg;           //works
    _fmap[0x04] = &movRegIndReg;        //works
//==JUMP========================//
    _fmap[0x05] = &jump;
    _fmap[0x06] = &jumpE;
    _fmap[0x07] = &jumpNE;
    _fmap[0x08] = &jumpLT;
    _fmap[0x09] = &jumpGT;
//==CMP=========================//
    _fmap[0x0b] = &cmpRegReg;
    _fmap[0x0c] = &cmpRegConst;
//==PUSH========================//
    _fmap[0x0d] = &pushReg;
    _fmap[0x0e] = &pushConst;
//==POP=========================//
    _fmap[0x0f] = &popReg;
//==SYSCALL=====================//
    _fmap[0x10] = &syscallOp;

////==XOR=========================//
//    _fmap[0x06] = &xorRegReg;
//    _fmap[0x07] = &xorRegConst;
////==SHIFT=======================//
//    _fmap[0x08] = &lshiftRegReg;
//    _fmap[0x09] = &rshiftRegReg;
//    _fmap[0x0a] = &lshiftRegConst;
//    _fmap[0x0b] = &rshiftRegConst;
////==AND=========================//
//    _fmap[0x0c] = &andRegReg;
//    _fmap[0x0d] = &andRegConst;
////==NOT=========================//
//    _fmap[0x0e] = &notRegReg;
//    _fmap[0x0f] = &notRegConst;
////==ADD=========================//
//    _fmap[0x10] = &addR1reg;
//    _fmap[0x11] = &addR1const;
////==SUB=========================//
//    _fmap[0x12] = &subR1reg;
//    _fmap[0x13] = &subR1const;
////==MULT========================//
//    _fmap[0x14] = &multR1reg;
//    _fmap[0x15] = &multR1const;
////==DIV=========================//
//    _fmap[0x16] = &divR1reg;
//    _fmap[0x17] = &divR1const;
////==CMP=========================//
//    _fmap[0x18] = &cmpRegs;
////==JUMP========================//
//    //Jump is interesting. You always jmp to the location in R1. The operand
//    //is a number from 0-7 that indicates the condition.
////==SYSCALL=====================//
//    _fmap[0x1a] = &syscall_op;  //syscall
}

int executionLoop() {
    while ( _regs[IP] < (_buff_size-INSTRUCTION_BYTE_SIZE) ) {
        execute(_regs[IP]);
        _regs[IP] += INSTRUCTION_BYTE_SIZE;
    }
    return 0;
}

int execute(int loc) {
    op fn;
    int opcode   = toUInt16( _buff[loc],   _buff[loc+1]);
    int operandA = toUInt16( _buff[loc+2], _buff[loc+3]);
    int operandB = toUInt16( _buff[loc+4], _buff[loc+5]);
    int operandC = toUInt16( _buff[loc+6], _buff[loc+7]);
    if ( opcode >= MAX_NUM_OPS ) {
        printf("\nNo operation in op table for opcode '%#1x'.", opcode);
        return 1;
    }
    fn = _fmap[opcode];
    fn(operandA, operandB, operandC);
    return 0;
}

int loadBuffer(char *fname) {
    FILE *f = fopen(fname, "rb");
    if ( f == NULL ) {
        fail(ERR_FILE);
    }
    fseek(f,0,SEEK_END);
    long fsize = ftell(f);
    _buff_size = fsize+1;
    rewind(f);
    if ( !(_buff = malloc(_buff_size)) ) {
        fail(ERR_MEM);
    }
    fread(_buff, fsize, 1, f);
    fclose(f);
    _buff[fsize]=0;
    return 0;
}

int fail( Err failure_code ) {
    if ( failure_code == ERR_FILE )
        printf("ERR: File could not be opened.\n");
    if ( failure_code == ERR_ARG_COUNT )
        printf("ERR: Not enough arguments.\nUsage: vm source_file\n");
    if ( failure_code == ERR_MEM )
        printf("ERR: Failed to allocate enough memory read input file.\n");
    exit(1);
}
