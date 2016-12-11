#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "headers/utils.h"
#include "headers/defs.h"
#include "lib/syscalls.c"
#include "lib/ops.c"
#define MAX_NUM_OPS 0xff

int main(int argc, char *argv[]) {
    printf("Firing up the virtual machine...\n\n");
    printf("OPERATION FEED:\n");
    //initialize instruction pointer
    if ( argc != 2 ) fail(ERR_ARG_COUNT);
    loadBuffer(argv[1]);
    loadFunctionTable();
    loadSysCalls();
    _regs[IP] = INSTRUCTION_BYTE_SIZE*1; //program starts at instruction 0
    _regs[SP] = _buff_size - SIZE_32; //program starts at instruction 0
    if ( executionLoop() > 0 ) fail(ERR_CMD_BUFF);
    return 0;
}

void loadSysCalls() {
    //syscalls will use a C-flavored calling convention. That is, they will
    //take arguments off the stack, instead of using your special
    //general-purpose registers.
    _syscalls[0x00] = &sys_dump;
    _syscalls[0x01] = &sys_exit;
}

void loadFunctionTable() {

//==MOV=========================//
    _fmap[0x00] = &debugOp;             //works
    _fmap[0x01] = &movRegConst;         //works
    _fmap[0x02] = &movRegIndConst;      //works
    _fmap[0x03] = &movRegReg;           //works
    _fmap[0x04] = &movRegIndReg;        //works
//==JUMP========================//
    _fmap[0x05] = &jump;                //works
    _fmap[0x06] = &jumpE;               //works
    _fmap[0x07] = &jumpNE;              //works
    _fmap[0x08] = &jumpLT;              //works
    _fmap[0x09] = &jumpGT;              //works
//==CMP=========================//
    _fmap[0x0b] = &cmpRegReg;           //works
    _fmap[0x0c] = &cmpRegConst;         //works
//==PUSH========================//
    _fmap[0x0d] = &pushReg;             //works
    _fmap[0x0e] = &pushConst;           //works
//==POP=========================//
    _fmap[0x0f] = &popReg;              //works
//==SYSCALL=====================//
    _fmap[0x10] = &syscallOp;           //works
//==XOR=========================//
    _fmap[0x11] = &xorRegReg;           //testing...
    _fmap[0x12] = &xorRegConst;         //testing...
//==SHIFT=======================//
    _fmap[0x13] = &lshiftRegReg;        //IMPL
    _fmap[0x14] = &rshiftRegReg;        //IMPL
    _fmap[0x15] = &lshiftRegConst;      //IMPL
    _fmap[0x16] = &rshiftRegConst;      //IMPL
//==AND=========================//
    _fmap[0x17] = &andRegReg;           //IMPL
    _fmap[0x18] = &andRegConst;         //IMPL
//==NOT=========================//
    _fmap[0x19] = &notReg;              //IMPL
//==ADD=========================//
    _fmap[0x1a] = &addRegReg;           //IMPL
    _fmap[0x1b] = &addRegConst;         //IMPL
//==SUB=========================//
    _fmap[0x1c] = &subRegReg;           //IMPL
    _fmap[0x1d] = &subRegConst;         //IMPL
//==MULT========================//
    _fmap[0x1e] = &multRegReg;          //IMPL
    _fmap[0x1f] = &multRegConst;        //IMPL
//==DIV=========================//
    _fmap[0x20] = &divRegReg;           //IMPL
    _fmap[0x21] = &divRegConst;         //IMPL
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
    _buff_size = fsize+1+_stack_size;
    _buff_size += _buff_size % SIZE_32; //align for SIZE_32
    rewind(f);
    if ( !(_buff = calloc(_buff_size, sizeof(char))) ) {
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
