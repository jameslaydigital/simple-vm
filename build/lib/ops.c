char fnote[23] = {'R','e','g','i','s','t','e','r',' ','i','n','d','e','x',' ','f','a','i','l','u','r','e','\0'};

//==DEBUGGING===========================//
void debugOp(unsigned short A,unsigned short B,unsigned short C) {
    printf("debugOp(%#1x, %#1x, %#1x);\n", A, B, C);
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
