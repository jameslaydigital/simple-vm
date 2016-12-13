#!/usr/bin/python

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-o", "--output", dest="outputFile", help="Assemble output to FILE", metavar="FILE")
parser.add_option("-v", "--verbose", action="store_true", dest="verbose", default=False, help="Verbose output.")
(options, args) = parser.parse_args()
sourceFile = args[0] if len(args) > 0 else ""
outputFile = options.outputFile
outputFile = outputFile if outputFile else "a.out"
verbose = options.verbose
if not sourceFile:
    print "No source file specified. Usage: SASM source_file"
    exit(1)

################# TYPES ########################################
class Operation(object):
    def __init__(self, name, arguments, lineNum=0):
        self.name = name
        if not name in ops:
            err("Operation not found: \""+name+"\"", lineNum)
        self.arguments = arguments
        self.lineNum = lineNum
        argTypes = " ".join([type(arg).__name__ for arg in arguments])
        if argTypes in ops[name]:
            self.value = ops[name][argTypes]
        else:
            err("Incorrect argument pattern for nmemonic \""+name+"\". " +
                "Acceptable argument patterns are: \n" + argTypes, lineNum)
        self.argumentTypes = argTypes
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
    "debug" : {
        "Const Const Const" : "\x00\x00"
    },
    "mov" : {
        "Reg Const"     :"\x01\x00",
        "Reg IndConst"  :"\x02\x00",
        "Reg Reg"       :"\x03\x00",
        "Reg IndReg"    :"\x04\x00"
    },
    "jump" : {
        "Const" : "\x05\x00\x00\x00"
    },
    "jumpE" : {
        "Const" : "\x06\x00\x00\x00"
    },
    "jumpNE" : {
        "Const" : "\x07\x00\x00\x00"
    },
    "jumpLT" : {
        "Const" : "\x08\x00\x00\x00"
    },
    "jumpGT" : {
        "Const" : "\x09\x00\x00\x00"
    },
    "cmpRegReg" : {
        "Reg Reg" : "\x0b\x00"
    },
    "cmpRegConst" : {
        "Reg Const" : "\x0c\x00"
    },
    "push" : {
        "Reg" : "\x0d\x00",
        "Const" : "\x0e\x00\x00\x00"
    },
    "pop" : {
        "Reg" : "\x0f\x00"
    },
    "xor" : {
        "Reg Reg" : "\x11\x00",
        "Reg Const" : "\x12\x00"
    },
    "lshift" : {
        "Reg Reg" : "\x13\x00",
        "Reg Const" : "\x15\x00"
    },
    "rshift" : {
        "Reg Reg" : "\x14\x00",
        "Reg Const" : "\x16\x00"
    },
    "and" : {
        "Reg Reg" : "\x17\x00",
        "Reg Const" : "\x18\x00"
    },
    "not" : {
        "Reg" : "\x19\x00"
    },
    "add" : {
        "Reg Reg" : "\x1a\x00",
        "Reg Const" : "\x1b\x00"
    },
    "sub" : {
        "Reg Reg" : "\x1c\x00",
        "Reg Const" : "\x1d\x00"
    },
    "mult" : {
        "Reg Reg" : "\x1e\x00",
        "Reg Const" : "\x1f\x00"
    },
    "div" : {
        "Reg Reg" : "\x20\x00",
        "Reg Const" : "\x21\x00"
    },
    "syscall" : {
        "SysCall" : "\x10\x00\x00\x00"
    }
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
        if token[0] == '[' and token[-1] == ']':
            key = token[1:-1]
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
sourceData = [line.split() for line in open(sourceFile).readlines()] #heh, this is your parser
ast = AST(sourceData)
if verbose: print ast.toString()
open(outputFile, "wb").write(ast.compile())
