char fnote[23] = {'R','e','g','i','s','t','e','r',' ','i','n','d','e','x',' ','f','a','i','l','u','r','e','\0'};

//==DEBUGGING===========================//
void debugOp(unsigned short A,unsigned short B,unsigned short C) {
    printf("debugOp(%#1x, %#1x, %#1x);\n", A, B, C);
    return;
}

void noOp(unsigned short A,unsigned short B,unsigned short C) {
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

void movIndConstReg(unsigned short LSB,unsigned short MSB,unsigned short r) {
    //mov [Const] Reg
    if ( r < MAX_REG_INDEX ) {
        unsigned int base = toUInt32(LSB,MSB);
        unsigned int num = _regs[r];
        if ( base < _buff_size ) {
            uIntToBytes(num,_buff+base);
        } else printf("ERR: out of bounds memory access: %#1x.\n",base);
    } else printf("ERR: movRegIndReg: %s: %#1x.\n",fnote,r);
}

void movIndRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    //mov [Reg] Reg
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        unsigned int base = _regs[rA];
        unsigned int num = _regs[rB];
        if ( base < _buff_size ) {
            uIntToBytes(num,_buff+base);
        } else printf("ERR: out of bounds memory access: %#1x.\n",base);
    } else printf("ERR: movRegIndReg: %s: %#1x, %#1x.\n",fnote,rA,rB);
}

void movIndRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    //mov [Reg] Reg
    if ( r < MAX_REG_INDEX ) {
        unsigned int base = _regs[r];
        unsigned int val = toUInt32(LSB,MSB);
        if ( base < _buff_size ) {
            uIntToBytes(val,_buff+base);
        } else printf("ERR: out of bounds memory access: %#1x.\n",base);
    } else printf("ERR: movRegIndReg: %s: %#1x.\n",fnote,r);
}


void pushReg(unsigned short r,unsigned short NA,unsigned short NB) {
    printf("RUNNING PUSH: %#1x %#1x %#1x\n", r, NA, NB);
    if ( r < MAX_REG_INDEX ) {
        _regs[SP] -= SIZE_32;
        unsigned int base = _regs[SP];
        unsigned int num = _regs[r];
        uIntToBytes(num,_buff+base);
        //printf("\tmemory size     %lu\n", _buff_size); //debug
        //printf("\tpushed register %#1x to %u.\n", r, _regs[SP]); //debug
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
    if ( loc % INSTRUCTION_BYTE_SIZE != 0 ) {
        printf("WARN: jumping to middle of instruction (byte %#1x). Prepare for confusing shit.", loc);
    }
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

void jumpReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int loc = _regs[r];
        if ( loc % INSTRUCTION_BYTE_SIZE != 0 ) {
            printf("WARN: jumping to middle of instruction (byte %#1x). Prepare for confusing shit.", loc);
        }
        _regs[IP] = loc-INSTRUCTION_BYTE_SIZE;
    } else printf("ERR: jumpReg: %s: %#1x.\n",fnote,r);
}
void jumpEReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if (_regs[CMP] == CMP_EQ ) jumpReg(r,NA,NB);
}
void jumpNEReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if (_regs[CMP] != CMP_EQ ) jumpReg(r,NA,NB);
}
void jumpLTReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if (_regs[CMP] == CMP_L ) jumpReg(r,NA,NB);
}
void jumpGTReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if (_regs[CMP] == CMP_G ) jumpReg(r,NA,NB);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//  :    :                                                                                     //
//  |  2 | [ebp + 16] (3rd function argument)                                                  //
//  |  5 | [ebp + 12] (2nd argument)                                                           //
//  | 10 | [ebp + 8]  (1st argument)                                                           //
//  | RA | [ebp + 4]  (return address)                                                         //
//  | FP | [ebp]      (old ebp value)                                                          //
//  |    | [ebp - 4]  (1st local variable)                                                     //
//  :    :                                                                                     //
//  :    :                                                                                     //
//  |    | [ebp - X]  (esp - the current stack pointer. The use of push / pop is valid now)    //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////

void callOp(unsigned short NA,unsigned short LSB,unsigned short MSB) {
    unsigned int returnAddress = _regs[IP]+INSTRUCTION_BYTE_SIZE;
    //push IP+(2 instructions)
    unsigned short plsb = (unsigned short)(returnAddress & 0xff);
    unsigned short pmsb = (unsigned short)((returnAddress>>8) & 0xff);
    pushConst(NA,plsb,pmsb);
    //jump to call location
    jump(NA,LSB,MSB);
}

void retOp(unsigned short NA,unsigned short NB,unsigned short NC) {
    //move the instruction pointer back to whatever is currently on the stack
    unsigned int returnAddressOffset = _regs[BP] - SIZE_32;
    unsigned int returnAddress = bytesToUInt(_buff+returnAddressOffset);
    unsigned short raLSB = returnAddress & 0xff;
    unsigned short raMSB = (returnAddress>>8) & 0xff;
    //this is your jump: popping the return address into IP. This also undoes
    //the push initiated by "callOp".
    //popReg((unsigned short)IP,0,0);
    _regs[IP] = (bytesToUInt(_buff+_regs[SP]) - INSTRUCTION_BYTE_SIZE);
    //restore RA to IP, and subtract an instruction because it will be
    //auto-incremented when this operation completes.
    _regs[SP] += SIZE_32; //pop undoes the push that callOp does to store RA.
}
void syscallOp(unsigned short r,unsigned short LSB,unsigned short MSB) {
    unsigned int routine_vector = toUInt32(LSB,MSB);
    _syscalls[routine_vector]();
}

int cmp_util(unsigned int A, unsigned int B) {
    if ( A == B ) return CMP_EQ;
    if ( A < B ) return CMP_L;
    return CMP_G;
}
void cmpRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    _regs[CMP] = 0;
    unsigned int numA = toUInt32(_regs[rA], (unsigned short)0);
    unsigned int numB = toUInt32(_regs[rB], (unsigned short)0);
    _regs[CMP] = cmp_util(numA, numB);
}
void cmpRegConst(unsigned short rA,unsigned short lsb,unsigned short msb) {
    _regs[CMP] = 0;
    unsigned int numA = toUInt32(_regs[rA], (unsigned short)0);
    _regs[CMP] = cmp_util(numA, toUInt32(lsb,msb));
}

//==XOR=========================//

void xorRegReg(unsigned short rA, unsigned short rB, unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] ^ _regs[rB];
    } else printf("ERR: xorRegreg: %s: %#1x %#1x.\n",fnote,rA,rB);
}

void xorRegConst(unsigned short r, unsigned short LSB, unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] ^ val;
    } else printf("ERR: xorRegreg: %s: %#1x.\n",fnote,r);
}

//==SHIFT=======================//
void lshiftRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] << _regs[rB];
    } else printf("ERR: lshiftRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void rshiftRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] >> _regs[rB];
    } else printf("ERR: rshiftRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void lshiftRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] << val;
    } else printf("ERR: lshiftRegConst: %s: %#1x.\n",fnote,r);
}
void rshiftRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] >> val;
    } else printf("ERR: rshiftRegConst: %s: %#1x.\n",fnote,r);
}

//==AND=========================//
void andRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] & _regs[rB];
    } else printf("ERR: andRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void andRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] & val;
    } else printf("ERR: andRegConst: %s: %#1x.\n",fnote,r);
}

//==NOT=========================//
void notReg(unsigned short r,unsigned short NA,unsigned short NB) {
    if ( r < MAX_REG_INDEX ) {
        _regs[r] = ~_regs[r];
    } else printf("ERR: notReg: %s: %#1x.\n",fnote,r);
}

//==ADD=========================//
void addRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] + _regs[rB];
    } else printf("ERR: addRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void addRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] + val;
    } else printf("ERR: addRegConst: %s: %#1x.\n",fnote,r);
}

//==SUB=========================//
void subRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] - _regs[rB];
    } else printf("ERR: subRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void subRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] - val;
    } else printf("ERR: subRegConst: %s: %#1x.\n",fnote,r);
}

//==MULT========================//
void multRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] * _regs[rB];
    } else printf("ERR: multRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void multRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] * val;
    } else printf("ERR: multRegConst: %s: %#1x.\n",fnote,r);
}

//==DIV=========================//
void divRegReg(unsigned short rA,unsigned short rB,unsigned short NA) {
    if ( rA < MAX_REG_INDEX && rB < MAX_REG_INDEX ) {
        _regs[rA] = _regs[rA] / _regs[rB];
    } else printf("ERR: divRegReg: %s: %#1x %#1x.\n",fnote,rA,rB);
}
void divRegConst(unsigned short r,unsigned short LSB,unsigned short MSB) {
    if ( r < MAX_REG_INDEX ) {
        unsigned int val = toUInt32(LSB,MSB);
        _regs[r] = _regs[r] / val;
    } else printf("ERR: divRegConst: %s: %#1x.\n",fnote,r);
}
