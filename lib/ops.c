char fnote[23] = {'R','e','g','i','s','t','e','r',' ','i','n','d','e','x',' ','f','a','i','l','u','r','e','\0'};

//==DEBUGGING===========================//
void debugOp(unsigned short A,unsigned short B,unsigned short C) {
    unsigned int i = toUInt32(B,C);
    //printf("debugOp(%u, %u, %u);\n", A, B, C);
    printf("debugOp(%#1x, %#1x);\n", A, i);
    return;
}

void movRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    //loads register A immediate constant
    if ( r < MAX_REG_INDEX ) {
        _regs[r] = toUInt32(LSB,MSB);
    } else printf("ERR: movRegConst: %s: %#1x.\n",fnote,r);
}

void movRegIndConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    //loads register with value in memory located at `constant`.
    if ( r < MAX_REG_INDEX ) {
        unsigned int b = toUInt32(LSB,MSB);
        _regs[r] = bytesToUInt(_buff+b);
    } else printf("ERR: movRegIndConst: %s: %#1x.\n",fnote,r);
}

void movRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    //loads register A with register B contents directly
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rB];
    } else 
        printf("ERR: movRegReg: %s: %u, %u.\n",
            fnote, (unsigned int)rA, rB);
}

void movRegIndReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    //loads register A with contents of effective address in register B.
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        int b = _regs[rB];
        _regs[rA] = bytesToUInt(_buff+b);
    } else 
        printf("ERR: movRegIndReg: %s: %#1x, %#1x.\n",fnote,rA,rB);
}


void pushReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if ( r < MAX_REG_INDEX ) {
        _regs[SP] -= SIZE_32;
        unsigned int base = _regs[SP];
        unsigned int num = _regs[r];
        uIntToBytes(num,_buff+base);
    } else printf("ERR: pushReg: %s: %#1x.\n",fnote,r);
}
void pushConst(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    _regs[SP] -= SIZE_32;
    int base = _regs[SP];
    unsigned int num = toUInt32(LSB,MSB);
    uIntToBytes(num,_buff+base);
}
void popReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if ( r < MAX_REG_INDEX ) {
        _regs[r] = bytesToUInt(_buff+_regs[SP]);
        _regs[SP]+=SIZE_32;
    } else printf("ERR: popReg: %s: %#1x.\n",fnote,r);
}

void jump(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    int loc = toUInt32(LSB, MSB);
    _regs[IP] = loc-INSTRUCTION_BYTE_SIZE;
}
void jumpE(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    if (_regs[CMP] == CMP_EQ ) jump(NA,LSB,MSB);
}
void jumpNE(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    if (_regs[CMP] != CMP_EQ ) jump(NA,LSB,MSB);
}
void jumpLT(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    if (_regs[CMP] == CMP_L ) jump(NA,LSB,MSB);
}
void jumpGT(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    if (_regs[CMP] == CMP_G ) jump(NA,LSB,MSB);
}

void syscallOp(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    unsigned int routine_vector = toUInt32(LSB,MSB);
    _syscalls[routine_vector]();
}

int cmp_util(unsigned int A, unsigned int B) {
    if ( A == B ) return CMP_EQ;
    if ( A < B ) return CMP_L;
    return CMP_G;
}
void cmpRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    _regs[CMP] = cmp_util(_regs[rA],_regs[rB]);
}
void cmpRegConst(unsigned short rA,unsigned short lsb,unsigned short msb) {
    _regs[CMP] = cmp_util(_regs[rA],toUInt32(lsb,msb));
}

//void movReg(unsigned short reg, unsigned short off, int constant) {
//    //loads register A immediate constant
//    if ( checkRegIndex(reg) ) {
//        _regs[regA] = constant+off;
//    }
//}
//
//void movRegStackD(short reg, short offset, int loc) {
//}
//
//void loadR1const(int operand) {
//    printf("loadR1const %d\n", operand);
//    _regs[R1] = operand;
//}
//void loadR2const(int operand) {
//    printf("loadR2const %d\n", operand);
//    _regs[R2] = operand;
//}
//void loadR3const(int operand) {
//    printf("loadR3const %d\n", operand);
//    _regs[R3] = operand;
//}
//void loadR4const(int operand) {
//    printf("loadR4const %d\n", operand);
//    _regs[R4] = operand;
//}
//void loadR5const(int operand) {
//    printf("loadR5const %d\n", operand);
//    _regs[R5] = operand;
//}
//void loadR6const(int operand) {
//    printf("loadR6const %d\n", operand);
//    _regs[R6] = operand;
//}
//void loadR7const(int operand) {
//    printf("loadR7const %d\n", operand);
//    _regs[R7] = operand;
//}
//void loadR8const(int operand) {
//    printf("loadR8const %d\n", operand);
//    _regs[R8] = operand;
//}
//
//
////==LOAD=(register)=====================//
//void loadR1mem(int loc) {
//    try {
//        _regs[R1] = _buff.at(_r);
//        printf("loadR1mem [%d] (%d)\n", loc, _regs[R1]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR2mem(int loc) {
//    try {
//        _regs[R2] = _buff.at(_);
//        printf("loadR2mem [%d] (%d)\n", loc, _regs[R2]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR3mem(int loc) {
//    try {
//        _regs[R3] = _buff.at(_);
//        printf("loadR3mem [%d] (%d)\n", loc, _regs[R3]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR4mem(int loc) {
//    try {
//        _regs[R4] = _buff.at(_);
//        printf("loadR4mem [%d] (%d)\n", loc, _regs[R4]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR5mem(int loc) {
//    try {
//        _regs[R5] = _buff.at(_);
//        printf("loadR5mem [%d] (%d)\n", loc, _regs[R5]);
//    } catch(...) {
//
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR6mem(int loc) {
//    try {
//        _regs[R6] = _buff.at(_);
//        printf("loadR6mem [%d] (%d)\n", loc, _regs[R6]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR7mem(int loc) {
//    try {
//        _regs[R7] = _buff.at(_);
//        printf("loadR7mem [%d] (%d)\n", loc, _regs[R7]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//void loadR8mem(int loc) {
//    try {
//        _regs[R8] = _buff.at(_r);
//        printf("loadR8mem [%d] (%d)\n", loc, _regs[R8]);
//    } catch(...) {
//        printf("Heap access fault. No memory available at index %d\n", loc);
//    }
//}
//
//inline bool checkRegIndex(int reg_index) {
//    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
//        printf("Register index %d invalid.\n", reg_index);
//        return false;
//    }
//    return true;
//}
//
//inline bool checkStackIndex(int index) {
//    int sz = _buff.size();
//    if ( index > sz ) {
//        printf("Stack index %d is not within buff of size %d.\n",index,sz);
//        return false;
//    }
//    return true;
//}
//
//void loadR1reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R1] = _buff.at(reg_index);
//    }
//}
//void loadR2reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R2] = _buff.at(reg_index);
//    }
//}
//void loadR3reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R3] = _buff.at(reg_index);
//    }
//}
//void loadR4reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R4] = _buff.at(reg_index);
//    }
//}
//void loadR5reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R5] = _buff.at(reg_index);
//    }
//}
//void loadR6reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R6] = _buff.at(reg_index);
//    }
//}
//void loadR7reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R7] = _buff.at(reg_index);
//    }
//}
//void loadR8reg(int reg_index) {
//    if ( checkRegIndex(reg_index) ) {
//        _regs[R8] = _buff.at(reg_index);
//    }
//}
//
////loads offset of base pointer
//
//
//
////==PUSH================================//
//void push(int reg_index) {
//    _regs[SP] += SIZE_32;
//    if ( checkStackIndex(_regs[SP]) )
//        printf("warning: cannot push beyong stack limit.\n");
//    _stack[_regs[SP]] = _regs[reg_index];
//}
//
////==ADD=================================//
//void addR1reg(int reg_index) {
//    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
//        printf("error: trying to add to non-existent register: R%d\n", reg_index);
//    }
//    _regs[R1] = _regs[R1]+_regs[reg_index];
//}
//void addR1const(int constant) {
//    _regs[R1] = _regs[R1]+constant;
//}
//
////==SUB=================================//
//void subR1reg(int reg_index) {
//    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
//        printf("error: trying to subtract from non-existent register: R%d\n", reg_index);
//    }
//    _regs[R1] = _regs[R1]-_regs[reg_index];
//}
//void subR1const(int constant) {
//    _regs[R1] = _regs[R1]-constant;
//}
//
////==MULT================================//
//void multR1reg(int reg_index) {
//    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
//        printf("error: trying to multiply by non-existent register: R%d\n", reg_index);
//    }
//    _regs[R1] = _regs[R1]*_regs[reg_index];
//}
//void multR1const(int constant) {
//    _regs[R1] = _regs[R1]*constant;
//}
//
////==DIV=================================//
//void divR1reg(int reg_index) {
//    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
//        printf("error: trying to divide by non-existent register: R%d\n", reg_index);
//    }
//    _regs[R1] = _regs[R1]/_regs[reg_index];
//}
//void divR1const(int constant) {
//    _regs[R1] = _regs[R1]/constant;
//}
//
////==SYSCALL=============================//
//
//void cmpRegs(int reg_index) {
//    //TODO
//}
//
//void jump(int condition) {
//    //jump to register location. You always jump to the location in R1. The
//    //operand is a number from 0-7 that indicates the jump requirement.
//    //condition == 0: jump unconditionally
//    //condition == 1: jump if equal
//    //condition == 2: jump if not equal
//    //condition == 3: jump if less than
//    //condition == 4: jump if less than or equal
//    //condition == 5: jump if greather than
//    //condition == 6: jump if greater than or equal
//    //condition == 7: undefined
//    switch(condition) {
//        case 0:
//            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        case 1:
//            if ( _regs[CMP] == CMP_EQ )
//                _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        case 2:
//            if ( _regs[CMP] == CMP_NE )
//                _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        case 3:
//            if ( _regs[CMP] == CMP_L )
//            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        case 4:
//            if ( _regs[CMP] == CMP_LE )
//            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        case 5:
//            if ( _regs[CMP] == CMP_G )
//            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        case 6:
//            if ( _regs[CMP] == CMP_GE )
//            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
//            break;
//        default:
//            printf("warning: jump condition operand '%d' undefined. Ignoring.\n", condition);
//    }
//}
