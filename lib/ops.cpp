//==DEBUGGING===========================//
void debug_op(int operand) {
    std::cout<<"debug_op "<<operand<<std::endl;
    return;
}

//==LOAD=(constant)=====================//
void loadR1const(int operand) {
    printf("loadR1const %d\n", operand);
    _regs[R1] = operand;
}
void loadR2const(int operand) {
    printf("loadR2const %d\n", operand);
    _regs[R2] = operand;
}
void loadR3const(int operand) {
    printf("loadR3const %d\n", operand);
    _regs[R3] = operand;
}
void loadR4const(int operand) {
    printf("loadR4const %d\n", operand);
    _regs[R4] = operand;
}
void loadR5const(int operand) {
    printf("loadR5const %d\n", operand);
    _regs[R5] = operand;
}
void loadR6const(int operand) {
    printf("loadR6const %d\n", operand);
    _regs[R6] = operand;
}
void loadR7const(int operand) {
    printf("loadR7const %d\n", operand);
    _regs[R7] = operand;
}
void loadR8const(int operand) {
    printf("loadR8const %d\n", operand);
    _regs[R8] = operand;
}


//==LOAD=(register)=====================//
void loadR1mem(int loc) {
    try {
        _regs[R1] = _buff.at(loc);
        printf("loadR1mem [%d] (%d)\n", loc, _regs[R1]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR2mem(int loc) {
    try {
        _regs[R2] = _buff.at(loc);
        printf("loadR2mem [%d] (%d)\n", loc, _regs[R2]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR3mem(int loc) {
    try {
        _regs[R3] = _buff.at(loc);
        printf("loadR3mem [%d] (%d)\n", loc, _regs[R3]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR4mem(int loc) {
    try {
        _regs[R4] = _buff.at(loc);
        printf("loadR4mem [%d] (%d)\n", loc, _regs[R4]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR5mem(int loc) {
    try {
        _regs[R5] = _buff.at(loc);
        printf("loadR5mem [%d] (%d)\n", loc, _regs[R5]);
    } catch(...) {

        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR6mem(int loc) {
    try {
        _regs[R6] = _buff.at(loc);
        printf("loadR6mem [%d] (%d)\n", loc, _regs[R6]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR7mem(int loc) {
    try {
        _regs[R7] = _buff.at(loc);
        printf("loadR7mem [%d] (%d)\n", loc, _regs[R7]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}
void loadR8mem(int loc) {
    try {
        _regs[R8] = _buff.at(loc);
        printf("loadR8mem [%d] (%d)\n", loc, _regs[R8]);
    } catch(...) {
        printf("Heap access fault. No memory available at index %d\n", loc);
    }
}


//==PUSH================================//
void push(int reg_index) {
    _regs[SP] += 4;
    if ( _regs[SP] > _stack.size() )
        printf("warning: cannot push beyong stack limit.\n");
    _stack[_regs[SP]] = _regs[reg_index];
}

//==POP=================================//
void pop(int reg_index) {
    _regs[reg_index] = _stack[_regs[SP]];
    _regs[SP] -= 4;
    if ( _regs[SP] < 0 )
        printf("warning: cannot pop from beginning of stack.\n");
}

//==ADD=================================//
void addR1reg(int reg_index) {
    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
        printf("error: trying to add to non-existent register: R%d\n", reg_index);
    }
    _regs[R1] = _regs[R1]+_regs[reg_index];
}
void addR1const(int constant) {
    _regs[R1] = _regs[R1]+constant;
}

//==SUB=================================//
void subR1reg(int reg_index) {
    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
        printf("error: trying to subtract from non-existent register: R%d\n", reg_index);
    }
    _regs[R1] = _regs[R1]-_regs[reg_index];
}
void subR1const(int constant) {
    _regs[R1] = _regs[R1]-constant;
}

//==MULT================================//
void multR1reg(int reg_index) {
    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
        printf("error: trying to multiply by non-existent register: R%d\n", reg_index);
    }
    _regs[R1] = _regs[R1]*_regs[reg_index];
}
void multR1const(int constant) {
    _regs[R1] = _regs[R1]*constant;
}

//==DIV=================================//
void divR1reg(int reg_index) {
    if ( reg_index > MAX_REG_INDEX || reg_index < 0 ) {
        printf("error: trying to divide by non-existent register: R%d\n", reg_index);
    }
    _regs[R1] = _regs[R1]/_regs[reg_index];
}
void divR1const(int constant) {
    _regs[R1] = _regs[R1]/constant;
}

//==SYSCALL=============================//
void syscall_op(int routine_vector) {
    try {
        _syscalls.at(routine_vector)();
    } catch(...) {
        printf("error: syscall does not exist at vector %#1x.\n", routine_vector);
    }
}

void cmpRegs(int reg_index) {
    //TODO
}

void jump(int condition) {
    //jump to register location. You always jump to the location in R1. The
    //operand is a number from 0-7 that indicates the jump requirement.
    //condition == 0: jump unconditionally
    //condition == 1: jump if equal
    //condition == 2: jump if not equal
    //condition == 3: jump if less than
    //condition == 4: jump if less than or equal
    //condition == 5: jump if greather than
    //condition == 6: jump if greater than or equal
    //condition == 7: undefined
    switch(condition) {
        case 0:
            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        case 1:
            if ( _regs[CMP] == CMP_EQ )
                _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        case 2:
            if ( _regs[CMP] == CMP_NE )
                _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        case 3:
            if ( _regs[CMP] == CMP_L )
            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        case 4:
            if ( _regs[CMP] == CMP_LE )
            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        case 5:
            if ( _regs[CMP] == CMP_G )
            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        case 6:
            if ( _regs[CMP] == CMP_GE )
            _regs[IP] = _regs[R1] - 1; //bc execution will increment IP after this op
            break;
        default:
            printf("warning: jump condition operand '%d' undefined. Ignoring.\n", condition);
    }
}
