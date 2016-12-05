#!/usr/bin/python

import sys

_CMD_SIZE = 8
opcodes = {
    "debugOp"           : bytearray([chr(0),chr(0),chr(0),chr(0)]),
    "movRegReg"         : bytearray([chr(0),chr(1),chr(0),chr(0)]),
    "movRegIndReg"      : bytearray([chr(0),chr(2),chr(0),chr(0)]),
    "movRegConst"       : bytearray([chr(0),chr(3),chr(0),chr(0)]),
    "movRegBaseOffset"  : bytearray([chr(0),chr(4),chr(0),chr(0)])
}

symbols = {
#register indexes:
    "R0" : chr(0),
    "R1" : chr(1),
    "R2" : chr(2),
    "R3" : chr(3),
    "R4" : chr(4),
    "R5" : chr(5),
    "R6" : chr(6),
    "R7" : chr(7),
    "R8" : chr(8),
    "R9" : chr(9),
    "BP" : chr(10),
#syscalls:
    "sys_dump" : chr(0)
}

def err(arg):
    sys.stderr.write("FATAL ERROR: ")
    sys.stderr.write(arg+"\n")
    exit(1)

def convert(tkns, i):
    if len(tkns) < 4:
        err("Not enough tokens on line "+str(i)+": ["+','.join(tkns)+"]");
    op  = tkns[0]
    if op == "label:":
        symbols[arg] = i*_CMD_SIZE
        return []
    else:
        opcode = opcodes[op]
    opA = toBytes16(int(tkns[1]))
    opB = toBytes16(int(tkns[2]))
    opC = toBytes16(int(tkns[3]))
    #operand = symbols[arg] if arg in symbols else int(arg)
    instructions = opcode+opA+opB+opC
    return instructions 

def toBytes(num):
bytearray([chr(int(tkns[1])),chr(0)]);
    try:
        if num > 127:
            return bytearray([num,0])
        else:
            return bytearray([num])
    except Exception as e:
        print "%s: number %s" % (e,num)
        raise Exception("fail")

def readLoop():
    i = 0;
    byteCode = bytearray()
    for text in sys.stdin.readlines():
        tkns = text.strip().split(";")[0].strip().split()
        if len(tkns) != 0:
            instructions = convert(tkns, i)
            if len(instructions) > 0:
                byteCode += instructions
        i += 1
    return byteCode

bc = readLoop()
open("a.out", "wb").write(bc)
print "SYMBOL DUMP:"
print symbols
