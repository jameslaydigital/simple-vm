#!/usr/bin/python
symbols = { "debugOp":"0000",
    "movRegConst"     : "0100",
    "movRegIndConst"  : "0200",
    "movRegReg"       : "0300",
    "movRegIndReg"    : "0400",
    "jump"            : "0500",
    "jumpE"           : "0600",
    "jumpNE"          : "0700",
    "jumpLT"          : "0800",
    "jumpGT"          : "0900",
    "cmpRegReg"       : "0b00",
    "cmpRegConst"     : "0c00",
    "pushReg"         : "0d00",
    "pushConst"       : "0e00",
    "popReg"          : "0f00",
    "syscallOp"       : "1000",
    "xorRegReg"       : "1100",
    "xorRegConst"     : "1200",
    "lshiftRegReg"    : "1300",
    "rshiftRegReg"    : "1400",
    "lshiftRegConst"  : "1500",
    "rshiftRegConst"  : "1600",
    "andRegReg"       : "1700",
    "andRegConst"     : "1800",
    "notReg"          : "1900",
    "addRegReg"       : "1a00",
    "addRegConst"     : "1b00",
    "subRegReg"       : "1c00",
    "subRegConst"     : "1d00",
    "multRegReg"      : "1e00",
    "multRegConst"    : "1f00",
    "divRegReg"       : "2000",
    "divRegConst"     : "2100",
    "syscall"         : "1000",
    "sys_dump"  : "0000",
    "sys_exit"  : "0100",
    "sys_print" : "0200",
    "R0" : "0000",
    "R1" : "0100",
    "R2" : "0200",
    "R3" : "0300",
    "R4" : "0400",
    "R5" : "0500",
    "R6" : "0600",
    "R7" : "0700",
    "R8" : "0800",
    "R9" : "0900",
    "R10": "0a00",
    "BP" : "0a00" }
data = open("source.asm").readlines()
for i in range(len(data)):
    for token in symbols:
        data[i] = data[i].replace(token, symbols[token])
    data[i] = data[i].replace(' ', '')
    data[i] = data[i].replace('\n', '')
    zeroPad = '0'*(16-len(data[i]))
    data[i] = data[i]+zeroPad #zero-pad
    data[i] = " ".join([data[i][0:4], data[i][4:8], data[i][8:12], data[i][12:]])
    data[i] = hex(i*8)+": "+data[i]

open("a.out", "w").write('\n'.join(data)+"\n")
