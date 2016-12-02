#ifndef __DEFS_H_INCLUDED__
#define __DEFS_H_INCLUDED__

//ERRORS
#define ERR_FILE 0
#define ERR_ARG_COUNT 1
#define ERR_CMD_BUFF 2
//ERRORS DONE

#define INSTRUCTION_BYTE_SIZE 8

//REGISTERS
//general-purpose:
#define R1 0
#define R2 1
#define R3 2
#define R4 3
#define R5 4
#define R6 5
#define R7 6
#define R8 7
//specialized:
#define IP 8
#define SP 9
#define BP 10
#define CMP 11
#define MAX_REG_INDEX 7
#define NUM_REGS 12 //used to initialize _regs[]
//REGISTERS DONE

//COMPARISON IDS
#define CMP_EQ 0
#define CMP_NE 1
#define CMP_L 2
#define CMP_LE 3
#define CMP_G 4
#define CMP_GE 5
//COMPARISON IDS DONE

#endif
