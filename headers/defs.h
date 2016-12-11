#ifndef __DEFS_H_INCLUDED__
#define __DEFS_H_INCLUDED__

//ERRORS
enum Error {
    ERR_FILE,
    ERR_ARG_COUNT,
    ERR_CMD_BUFF,
    ERR_MEM
};

typedef enum Error Err;
//ERRORS DONE

#define INSTRUCTION_BYTE_SIZE 8
#define SIZE_8 1
#define SIZE_16 2
#define SIZE_32 4
#define SIZE_64 8

//REGISTERS
//general-purpose:
#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7
#define R8 8
#define R9 9
#define BP 10    //Base pointer is another general use register. More for naming convenience.
//specialized:
#define SP 11   //stack pointer
#define IP 12   //instruction pointer or program counter
#define CMP 13  //comparison register
#define MAX_REG_INDEX 11
#define NUM_REGS 14 //used to initialize _regs[]
//REGISTERS DONE

//COMPARISON IDS
#define CMP_EQ 0
#define CMP_L 1
#define CMP_G 2
//COMPARISON IDS DONE

typedef void (*op)(unsigned short, unsigned short, unsigned short);
typedef void(*sysc)(void);
op _fmap[0xff];
sysc _syscalls[0xff];

unsigned char *_buff;
long _buff_size = 0;
long _stack_size = 1024;

int _regs[NUM_REGS];
int loadBuffer(char*);
int fail(Err);
int executionLoop();
int execute(int);
void loadFunctionTable();
void loadSysCalls();

#endif
