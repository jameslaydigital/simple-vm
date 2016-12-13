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
    _syscalls[0x02] = &sys_print;
}

void loadFunctionTable() {

//==MOV=========================//
    _fmap[0x00] = &debugOp;             //PASS
    _fmap[0x01] = &movRegConst;         //PASS
    _fmap[0x02] = &movRegIndConst;      //PASS
    _fmap[0x03] = &movRegReg;           //PASS
    _fmap[0x04] = &movRegIndReg;        //PASS
//==JUMP========================//
    _fmap[0x05] = &jump;                //PASS
    _fmap[0x06] = &jumpE;               //PASS
    _fmap[0x07] = &jumpNE;              //PASS
    _fmap[0x08] = &jumpLT;              //PASS
    _fmap[0x09] = &jumpGT;              //PASS
//==CMP=========================//
    _fmap[0x0b] = &cmpRegReg;           //PASS
    _fmap[0x0c] = &cmpRegConst;         //PASS
//==PUSH========================//
    _fmap[0x0d] = &pushReg;             //PASS
    _fmap[0x0e] = &pushConst;           //PASS
//==POP=========================//
    _fmap[0x0f] = &popReg;              //PASS
//==SYSCALL=====================//
    _fmap[0x10] = &syscallOp;           //PASS
//==XOR=========================//
    _fmap[0x11] = &xorRegReg;           //PASS
    _fmap[0x12] = &xorRegConst;         //PASS
//==SHIFT=======================//
    _fmap[0x13] = &lshiftRegReg;        //PASS
    _fmap[0x14] = &rshiftRegReg;        //PASS
    _fmap[0x15] = &lshiftRegConst;      //PASS
    _fmap[0x16] = &rshiftRegConst;      //PASS
//==AND=========================//
    _fmap[0x17] = &andRegReg;           //PASS
    _fmap[0x18] = &andRegConst;         //PASS
//==NOT=========================//
    _fmap[0x19] = &notReg;              //PASS
//==ADD=========================//
    _fmap[0x1a] = &addRegReg;           //PASS
    _fmap[0x1b] = &addRegConst;         //PASS
//==SUB=========================//
    _fmap[0x1c] = &subRegReg;           //PASS
    _fmap[0x1d] = &subRegConst;         //PASS
//==MULT========================//
    _fmap[0x1e] = &multRegReg;          //PASS
    _fmap[0x1f] = &multRegConst;        //PASS
//==DIV=========================//
    _fmap[0x20] = &divRegReg;           //PASS
    _fmap[0x21] = &divRegConst;         //PASS
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
