#!/usr/bin/python

import sys

_CMD_SIZE = 8
opcodes = {
    "loadR1const" : 0x00,
    "loadR2const" : 0x01,
    "loadR3const" : 0x02,
    "loadR4const" : 0x03,
    "loadR5const" : 0x04,
    "loadR6const" : 0x05,
    "loadR7const" : 0x06,
    "loadR8const" : 0x07,
    "loadR1mem"   : 0x08,
    "loadR2mem"   : 0x09,
    "loadR3mem"   : 0x0a,
    "loadR4mem"   : 0x0b,
    "loadR5mem"   : 0x0c,
    "loadR6mem"   : 0x0d,
    "loadR7mem"   : 0x0e,
    "loadR8mem"   : 0x0f,
    "syscall"     : 0x10,
    "push"        : 0x11,
    "pop"         : 0x12,
    "addR1reg"    : 0x13,
    "addR1const"  : 0x14,
    "subR1reg"    : 0x15,
    "subR1const"  : 0x16,
    "multR1reg"   : 0x17,
    "multR1const" : 0x18,
    "divR1reg"    : 0x19,
    "divR1const"  : 0x1A,
    "cmpRegs"     : 0x1B,
    "jump"        : 0x1B
}

symbols = {
#registers:
    "R1" : 0,
    "R2" : 1,
    "R3" : 2,
    "R4" : 3,
    "R5" : 4,
    "R6" : 5,
    "R7" : 6,
    "R8" : 7,
#syscalls:
    "sys_dump" : 0
}

def err(arg):
    sys.stderr.write(arg+"\n")
    exit(1)

def convert(tkns, i):
    op = tkns[0]
    arg = tkns[1]
    if op == "label:":
        symbols[arg] = i * _CMD_SIZE #TODO: this will cause problems
        return []
    else:
        opcode = opcodes[op]
    operand = symbols[arg] if arg in symbols else int(arg)
    instructions = [opcode, operand]
    print "%s(%s) %s(%s)" % (opcode, op, operand, arg)
    return instructions 

def toBytes(num):
    try:
        return bytearray([0,0,0,num])
    except Exception as e:
        print "%s: number %s" % (e,num)
        raise Exception("fail")

def readLoop():
    i = 0;
    byteCode = bytearray()
    for text in sys.stdin.readlines():
        tkns = text.strip().split(";")[0].strip().split()
        instructions = convert(tkns, i)
        if len(instructions) > 0:
            byteCode += toBytes(instructions[0])
            byteCode += toBytes(instructions[1])
        i += 1
    return byteCode

bc = readLoop()
open("a.out", "wb").write(bc)
print "SYMBOL DUMP:"
print symbols
