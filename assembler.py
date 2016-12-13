#!/usr/bin/python
import re

################# TYPES ########################################
class Operation(object):
    def __init__(self, name, arguments, lineNum=0):
        self.name = name
        if not name in ops:
            err("Operation not found: \""+name+"\"", lineNum)
        self.arguments = arguments
        self.lineNum = lineNum
        self.value = ops[name][0]
        self.argumentTypes = ops[name][1]
        #Argument types: [["Reg","Const"],["Reg","IndConst"],["Reg","IndReg"]]
        #for           `mov R1    35`, `mov R1,   [35]`,  `mov R1    [R2]`.
        argTypes = [type(arg).__name__ for arg in arguments]
        if not argTypes in self.argumentTypes:
            err("Incorrect argument pattern for nmemonic \""+name+"\". " +
                "Acceptable argument patterns are: \n" +
                ", ".join([" ".join(a) for a in self.argumentTypes]), lineNum)
    def toString(self):
        return self.name+"("+', '.join([arg.toString() for arg in self.arguments])+")"

    def compile(self):
        compilation = self.value+"".join([arg.compile() for arg in self.arguments])
        length = len(compilation)
        diff = 8-length
        zeroPadding = "\x00"*diff
        return compilation+zeroPadding


class Reg(object):
    def __init__(self, name, line):
        self.name = name
        if name in registers:
            self.value = registers[name]
        else:
            err("Cannot instantiate Reg object; \""+name+"\" not found in register table.", line)
    def toString(self):
        return "<Reg "+self.name+">"
    def compile(self):
        return self.value
class IndReg(object):
    def __init__(self, token, lineNum):
        self.name = name
        if name in registers:
            self.value = registers[name]
        else:
            err("Cannot instantiate IndReg object; \""+name+"\" not found in register table.", line)
    def toString(self):
        return "<Reg "+self.name+">"
    def compile(self):
        return self.value

class Const(object):
    def __init__(self, token, lineNum):
        #ID using original token what kind of constant we're dealing
        #with. Could be hex, binary, decimal, or octal.
        self.value = int(token)
        self.lineNum = lineNum
    def toString(self):
        return "<Const "+str(self.value)+">"
    def compile(self):
        #TODO: if self.value > 255, two bytes. If > 65535, 4 bytes
        return chr(self.value)+"\x00"
class IndConst(object):
    def __init__(self, token, lineNum):
        self.value = int(token)
        self.lineNum = lineNum
    def toString(self):
        return "<IndConst "+str(self.value)+">"
    def compile(self):
        #TODO: if self.value > 255, two bytes. If > 65535, 4 bytes
        return chr(self.value)+"\x00"


class SysCall(object):
    def __init__(self, token, lineNum):
        self.name = token
        if token in syscalls:
            self.value = syscalls[token]
        else:
            err("Syscall does not exist: \""+token+"\".", lineNum)
    def toString(self):
        return "<SysCall "+self.name+">"
    def compile(self):
        return self.value


################# UTILS ########################################
def err(msg, line):
    print msg + " on line "+str(line+1)+"."
    exit(1)

################# TABLES #######################################
ops = { 
    "debugOp"         : ["\x00\x00",[["Const","Const","Const"]]],
    "movRegConst"     : ["\x01\x00",[["Reg","Const"]]],
    "movRegIndConst"  : ["\x02\x00",[["Reg","IndConst"]]],
    "movRegReg"       : ["\x03\x00",[["Reg","Reg"]]],
    "movRegIndReg"    : ["\x04\x00",[["Reg","IndReg"]]],
    "jump"            : ["\x05\x00\x00\x00",[["Const"]]],
    "jumpE"           : ["\x06\x00\x00\x00",[["Const"]]],
    "jumpNE"          : ["\x07\x00\x00\x00",[["Const"]]],
    "jumpLT"          : ["\x08\x00\x00\x00",[["Const"]]],
    "jumpGT"          : ["\x09\x00\x00\x00",[["Const"]]],
    "cmpRegReg"       : ["\x0b\x00",[["Reg","Reg"]]],
    "cmpRegConst"     : ["\x0c\x00",[["Reg","Const"]]],
    "pushReg"         : ["\x0d\x00",[["Reg"]]],
    "pushConst"       : ["\x0e\x00\x00\x00",[["Const"]]],
    "popReg"          : ["\x0f\x00",[["Reg"]]],
    "xorRegReg"       : ["\x11\x00",[["Reg","Reg"]]],
    "xorRegConst"     : ["\x12\x00",[["Reg","Const"]]],
    "lshiftRegReg"    : ["\x13\x00",[["Reg","Reg"]]],
    "rshiftRegReg"    : ["\x14\x00",[["Reg","Reg"]]],
    "lshiftRegConst"  : ["\x15\x00",[["Reg","Const"]]],
    "rshiftRegConst"  : ["\x16\x00",[["Reg","Const"]]],
    "andRegReg"       : ["\x17\x00",[["Reg","Reg"]]],
    "andRegConst"     : ["\x18\x00",[["Reg","Const"]]],
    "notReg"          : ["\x19\x00",[["Reg"]]],
    "addRegReg"       : ["\x1a\x00",[["Reg","Reg"]]],
    "addRegConst"     : ["\x1b\x00",[["Reg","Const"]]],
    "subRegReg"       : ["\x1c\x00",[["Reg","Reg"]]],
    "subRegConst"     : ["\x1d\x00",[["Reg","Const"]]],
    "multRegReg"      : ["\x1e\x00",[["Reg","Reg"]]],
    "multRegConst"    : ["\x1f\x00",[["Reg","Const"]]],
    "divRegReg"       : ["\x20\x00",[["Reg","Reg"]]],
    "divRegConst"     : ["\x21\x00",[["Reg","Const"]]],
    "syscall"         : ["\x10\x00\x00\x00",[["SysCall"]]]
}

syscalls = {
    "dump"  : "\x00\x00",
    "exit"  : "\x01\x00",
    "print" : "\x02\x00"
}

registers = {
    "R0" : "\x00\x00",
    "R1" : "\x01\x00",
    "R2" : "\x02\x00",
    "R3" : "\x03\x00",
    "R4" : "\x04\x00",
    "R5" : "\x05\x00",
    "R6" : "\x06\x00",
    "R7" : "\x07\x00",
    "R8" : "\x08\x00",
    "R9" : "\x09\x00",
    "R10": "\x0a\x00",
    "BP" : "\x0a\x00"
}

################# AST ##########################################
class AST(object):
    def __init__(self, source):
        self.source = source
        self.astSequence = []
        for i in range(len(source)):
            tokens = source[i]
            arguments = [self.identify(token,i) for token in tokens[1:]] if len(tokens) > 1 else [] #creates your AST
            op = Operation(tokens[0], arguments, i) #name, arguments, line#
            self.astSequence.append(op)

    def identify(self, token, line):
        match = re.match(r'\[(.+)\]', token)
        if match != None:
            key = match.group(1)
            if key in registers:
                return IndReg(key, line)
            else:
                return IndConst(key, line)
        else:
            if token in registers:
                return Reg(token, line)
            if token in syscalls:
                return SysCall(token, line)
            return Const(token, line)

    def toString(self):
        return "{\n\t"+",\n\t".join([op.toString() for op in self.astSequence])+"\n}"

    def compile(self):
        return "".join([op.compile() for op in self.astSequence])


################# ENTRY POINT ##################################
sourceData = [line.split() for line in open("source.asm").readlines()] #heh, this is your parser
ast = AST(sourceData)
print ast.toString()
open("a.out","wb").write(ast.compile())
