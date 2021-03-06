#!/usr/bin/python
import re
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
    print "No source file specified. Usage: asm source_file"
    exit(1)

################# DEFS #########################################
INSTRUCTION_LENGTH = 8

################# TYPES ########################################

class Operation(object):
    def __init__(self, name, lineNo=0):
        self.name = name
        self.type = "Operation"
        if not name in ops:
            err("Operation not found: \""+name+"\"", lineNo)
        self.arguments = []
        self.lineNo = lineNo

    def setArguments(self, arguments):
        if len(arguments) == 0:
            self.argTypes = "None"
        else:
            self.argTypes = " ".join([arg.type for arg in arguments])
        print "Trying: "+self.name+"(\""+self.argTypes+"\") in: "
        print ops[self.name]
        print "on line "+str(self.lineNo)
        self.arguments = arguments
        return self.validate()

    def validate(self):
        if self.argTypes in ops[self.name]:
            self.value = ops[self.name][self.argTypes]
            return True
        else:
            return False

    def toString(self):
        return self.name+"("+', '.join([arg.toString() for arg in self.arguments])+")"
    def compile(self, compLen):
        self.validate()
        compilation = ""+self.value
        for arg in self.arguments:
            compLen += len(compilation)
            compilation += arg.compile(compLen)
        length = len(compilation)
        diff = 8-length
        zeroPad= "\x00"*diff
        return compilation+zeroPad

class LabelRef(object):
    def __init__(self, name, lineNo):
        self.lineNo = lineNo
        self.name = name[1:]
        self.value = name
        self.type = "Const"
    def toString(self):
        return "<Label_Reference: \""+self.name+"\">"
    def compile(self, compLen):
        return toLittleEndian32(labels[self.name]) if self.name in labels else "\0\0\0\0";
        #if self.name in labels:
        #    return toLittleEndian32(labels[self.name])
        #else:
        #    err("Label referenced before definition: \""+self.name+"\"",
        #        self.lineNo)

class LabelDef(object):
    def __init__(self, name, lineNo):
        self.name = name[1:-1]
        self.value = name
        self.type = "LabelDef"
        self.lineNo = lineNo
    def toString(self):
        return "<Label_Definition \""+self.name+"\">"
    def compile(self, compLen):
        #compLen is length of compilation output
        labels[self.name] = compLen
        return ""

class Reg(object):
    def __init__(self, name, lineNo):
        self.name = name
        self.type = "Reg"
        if name in registers:
            self.value = registers[name]
        else:
            err("Cannot instantiate Reg object; \""+name+"\" not found in register table.", line)
    def toString(self):
        return "<Reg "+self.name+">"
    def compile(self, compLen):
        return self.value

class IndReg(object):
    def __init__(self, name, lineNo):
        self.name = name
        self.type = "IndReg"
        if name in registers:
            self.value = registers[name]
        else:
            err("Cannot instantiate IndReg object; \""+name+"\" not found in register table.", line)
    def toString(self):
        return "<Reg "+self.name+">"
    def compile(self, compLen):
        return self.value

class String(object):
    def __init__(self, token, lineNo):
        self.lineNo = lineNo
        self.type = "String"
        noquotes = token[1:-1]
        self.value = noquotes.decode('string_escape')
    def toString(self):
        return "<String "+str(self.value)+">"
    def compile(self, compLen):
        zeroPad = "\0"*(8-(len(self.value)%8))
        return self.value+zeroPad

class Char(object):
    def __init__(self, token, lineNo):
        self.type="Char"
        self.lineNo = lineNo
        self.value = ord(token)
    def toString(self):
        return "<Char "+str(self.value)+">"
    def compile(self, compLen):
        if self.value < 256:
            return chr(self.value)+"\x00"
        else:
            err("Cannot have char value greater than 255", lineNo)

class Const(object):
    def __init__(self, token, lineNo):
        self.type="Const"
        self.lineNo = lineNo
        match = re.match(r"^\d+$", token)
        if match != None:
            self.value = int(token)
            return
        match = re.match(r"^(0x)([0-9a-fA-F]+)$", token)
        if match != None:
            self.value = int(token, 16)
            return
        match = re.match(r"^(0b)([01]+)$", token)
        if match != None:
            self.value = int(token, 2)
            return
    def toString(self):
        return "<Const "+str(self.value)+">"
    def compile(self, compLen):
        #TODO: if self.value > 255, two bytes. If > 65535, 4 bytes
        return chr(self.value)+"\x00"
class IndConst(object):
    def __init__(self, token, lineNo):
        self.value = int(token)
        self.lineNo = lineNo
        self.type="IndConst"
    def toString(self):
        return "<IndConst "+str(self.value)+">"
    def compile(self, compLen):
        #TODO: if self.value > 255, two bytes. If > 65535, 4 bytes
        if self.value < 256:
            return chr(self.value)+"\x00\x00\x00"
        if self.value < 65536:
            return chr(self.value & 0xff) + chr((self.value >> 8) & 0xff) + "\x00\x00"
        return chr(self.value&0xff)+chr((self.value>>8)&0xff)+chr((self.value>>16)&0xff)+chr((self.value>>24)&0xff)


class SysCall(object):
    def __init__(self, token, lineNo):
        self.name = token
        self.type = "SysCall"
        if token in syscalls:
            self.value = syscalls[token]
        else:
            err("Syscall does not exist: \""+token+"\".", lineNo)
    def toString(self):
        return "<SysCall "+self.name+">"
    def compile(self, compLen):
        return self.value

################# UTILS ########################################
def err(msg, line):
    print msg + " on line "+str(line)+"."
    exit(1)

################# TABLES #######################################
labels = {}

ops = {
    "debug" : {
        "Const Const Const" : "\x00\x00"
    },
    "mov" : {
        "Reg Reg"       :"\x01\x00",
        "Reg Const"     :"\x02\x00",
        "Reg IndConst"  :"\x03\x00",
        "Reg IndReg"    :"\x04\x00",
        "IndReg Reg"    :"\x05\x00",
        "IndReg Const"  :"\x06\x00",
        "IndConst Reg"  :"\x07\x00"
        #"IndConst Const":"\x08\x00" instr size limitation
    },
    "label" : {
        "Name" : "\0"
    },
    "noop" : {
        ""  :"\x08\x00"
    },
    "jump" : {
        "Const" : "\x09\x00\x00\x00",
        "Reg" : "\x25\x00\x00\x00"
    },
    "jumpE" : {
        "Const" : "\x0a\x00\x00\x00",
        "Reg" : "\x26\x00\x00\x00"
    },
    "jumpNE" : {
        "Const" : "\x0b\x00\x00\x00",
        "Reg" : "\x27\x00\x00\x00"
    },
    "jumpLT" : {
        "Const" : "\x0c\x00\x00\x00",
        "Reg" : "\x28\x00\x00\x00"
    },
    "jumpGT" : {
        "Const" : "\x0d\x00\x00\x00",
        "Reg" : "\x29\x00\x00\x00"
    },
    "cmpRegReg" : {
        "Reg Reg" : "\x0e\x00"
    },
    "cmpRegConst" : {
        "Reg Const" : "\x0f\x00"
    },
    "push" : {
        "Reg" : "\x11\x00",
        "Const" : "\x12\x00\x00\x00"
    },
    "pop" : {
        "Reg" : "\x13\x00"
    },
    "xor" : {
        "Reg Reg" : "\x14\x00",
        "Reg Const" : "\x15\x00"
    },
    "lshift" : {
        "Reg Reg" : "\x16\x00",
        "Reg Const" : "\x17\x00"
    },
    "rshift" : {
        "Reg Reg" : "\x18\x00",
        "Reg Const" : "\x19\x00"
    },
    "and" : {
        "Reg Reg" : "\x1a\x00",
        "Reg Const" : "\x1b\x00"
    },
    "not" : {
        "Reg" : "\x1c\x00"
    },
    "add" : {
        "Reg Reg" : "\x1d\x00",
        "Reg Const" : "\x1e\x00"
    },
    "sub" : {
        "Reg Reg" : "\x1f\x00",
        "Reg Const" : "\x20\x00"
    },
    "mult" : {
        "Reg Reg" : "\x21\x00",
        "Reg Const" : "\x22\x00"
    },
    "div" : {
        "Reg Reg" : "\x23\x00",
        "Reg Const" : "\x24\x00"
    },
    "call" : {
        "Const" : "\x2a\x00\x00\x00"
    },
    "ret" : {
        "None" : "\x2b\x00\x00\x00\x00\x00\x00\x00"
    },
    "syscall" : {
        "SysCall" : "\x10\x00\x00\x00"
    },
    "db" : {
        "Char Char Char Char" : ""
    },
    "ds" : {
        "String" : ""
    }
}

syscalls = {
    "dump"  : "\x00\x00",
    "exit"  : "\x01\x00",
    "print" : "\x02\x00",
    "read" : "\x03\x00"
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
    "BP" : "\x0a\x00",
    "SP" : "\x0b\x00",
    "IP" : "\x0c\x00"
}

################# AST ##########################################
#class AST(object):
#    def __init__(self, source):
#        self.source = source
#        self.astSequence = []
#        for i in range(len(source)):
#            tokens = source[i]
#            arguments = [self.identify(token,i) for token in tokens[1:]] if len(tokens) > 1 else []
#            op = Operation(tokens[0], arguments, i) #name, arguments, line#
#            self.astSequence.append(op)
#
#    def identify(self, token, line, arguments=[]):
#        if token[0] == '[' and token[-1] == ']':
#            key = token[1:-1]
#            if key in registers:
#                return IndReg(key, line)
#            else:
#                return IndConst(key, line)
#        else:
#            if token in registers:
#                return Reg(token, line)
#            if token in syscalls:
#                return SysCall(token, line)
#            if token in labels:
#                return Const(str(labels[token]), line)
#            if tokIsNumber(token):
#                return Const(token, line)
#            if tokIsChar(token):
#                return Char(token[1:-1], line)
#            if tokIsString(token):
#                return String(token[1:-1], line)
#            if tokIsLabelDef(token):
#                return LabelDef(token, line)
#            if tokIsOperation(token):
#                return Operation(name, arguments, line)
#            err("Unrecognized token: \""+token+"\"", line)
#    def toString(self):
#        return "{\n\t"+",\n\t".join([op.toString() for op in self.astSequence])+"\n}"
#    def compile(self):
#        output = ""
#        byteCount = 0
#        for op in self.astSequence:
#            print op
#            byteStr = op.compile()
#            byteCnt = len(byteStr)
#            output += byteStr
#        return output
#        #return "".join([op.compile() for op in self.astSequence])


def toLittleEndian32(integer):
    one     = integer       & 0xff
    ten     = (integer>>8)  & 0xf
    hundred = (integer>>12) & 0xff
    thousand= (integer>>16) & 0xf
    return chr(one)+chr(ten)+chr(hundred)+chr(thousand)

def tokIsLabelDef(string):
    # ".blah:"
    if re.match(r"\.[a-zA-Z0-9]+:", string) != None:
        return True
    return False
def tokIsLabelRef(string):
    # ".blah"
    if re.match(r"\.[a-zA-Z0-9]+\b", string) != None:
        return True
    return False
def tokIsOp(string):
    if string in ops:
        return True
    return False
def tokIsReg(string):
    return True if string in registers else False
def tokIsSysCall(string):
    return True if string in syscalls else False
def tokIsNumber(string):
    if re.match(r"^0x[0-9a-fA-F]+$", string) != None:
        return True
    if re.match(r"^0b[01]+$", string) != None:
        return True
    if re.match(r"^\d+$", string) != None:
        return True
    return False
def tokIsChar(string):
    if len(string) == 3 and string.startswith("'") and string.endswith("'"):
        return True
    return False
def tokIsString(string):
    if string.startswith('"') and string.endswith('"'):
        return True
    return False

################# ENTRY POINT ##################################
#sourceData = [line.split() for line in open(sourceFile).readlines()]

fdata = open(sourceFile).read()

#LABEL PREPROCESSOR
#start by giving labels their own line
#flines = re.sub(r"\n *(.[a-zA-Z0-9.]+):", r"\nlabel \1\n", fdata).split("\n")

#i = 0
#while i < len(flines):
#    line = flines[i]
#    nocmt = line.split(";").pop(0).strip() #rm comments
#    if nocmt == "": #ignore whitespace and empties
#        flines.pop(i) #get rid of it so it doesn't fuck our line count
#    else:
#        toks = line.split()
#        if toks[0] == "label":
#            if len(toks) == 1:
#                err("Label has no name.", i)
#            if toks[1] in labels:
#                err("Label already defined: \""+toks[1]+"\"", i)
#            labels[toks[1]] = i*INSTRUCTION_LENGTH
#            flines.pop(i)
#            i -= 1
#        i += 1

#   PARSER
#   PARSING RULES:
#   [X] Newlines are statement terminators.
#   [X] Empty lines are ignored, but continue to increment the line tally.
#   [X] Comments can be preprocessed out, but empty lines left over must be retained.
#   [ ] Identification happens on each token.
#       [ ] Each token is tested for a match (in this order) to be any of [ops, registers, syscalls, label]
#       [ ] Failure to match any throws a parsing error.
class NoneType(object):
    def __init__(self):
        self.type = "None"

class PStatement(object):
    def __init__(self, tokens, lineNo):
        self.tokens = tokens
        self.statement = []
        self.lineNo = lineNo
        self.identify()

    def identify(self):
        for token in self.tokens:
            newObj = self.identifyToken(token)
            self.statement.append(newObj)
        if len(self.statement) > 0 and not (self.statement[0].type in ["Operation", "LabelDef"]):
            err("Statement does not begin with Operation or Label "+
                    "Definition (if you meant to define a label, check for "+
                    "trailing colon \":\")", self.lineNo)

    def identifyToken(self, token):
        newObj = NoneType()
        lineNo = self.lineNo
        if tokIsOp(token):
            newObj = Operation(token,lineNo)
        elif tokIsSysCall(token):
            newObj = SysCall(token,lineNo)
        elif tokIsReg(token):
            newObj = Reg(token,lineNo)
        elif tokIsLabelDef(token):
            newObj = LabelDef(token,lineNo)
        elif tokIsLabelRef(token):
            newObj = LabelRef(token,lineNo)
        elif tokIsNumber(token):
            newObj = Const(token,lineNo)
        elif tokIsChar(token):
            newObj = Char(token,lineNo)
        elif tokIsString(token):
            newObj = String(token,lineNo)
        else:
            err("Unrecognized token: \""+token+"\"", lineNo)
        return newObj

    def toString(self):
        statementStr = ", ".join([tok.toString() for tok in self.statement])
        return "< PStatement: "+statementStr+">"

class Parser(object):
    def __init__(self, fdata):
        self.fdata = fdata
        self.sourceData = []
        self.statements = []
        self.lineNo = 0

    def tokenize(self):
        lineNo = 0
        for line in self.fdata.split("\n"):
            lineNo += 1
            nocmt = line.split(";").pop(0).strip() #rm comments
            splitDblQt = nocmt.split('"')
            if len(splitDblQt) %2 == 0:
                err("Parsing Error: Unmatched double quotes: `"+line+"`", lineNo)
            deepTokens = [splitDblQt[i].split() if i % 2 == 0 else ['"'+splitDblQt[i]+'"'] for i in range(len(splitDblQt))]
            flatTokens = [i for o in deepTokens for i in o]
            self.statements.append(PStatement(flatTokens, lineNo))

    def toString(self):
        return "\n".join([n.toString() for n in self.statements])

class AST(object):
    def __init__(self, parser):
        self.parser = parser
        self.stream = []
        for statement in parser.statements:
            for obj in statement.statement:
                self.stream.append(obj)
        self.sequence = []
        self.output = ""
        self.createSequence()

    def createSequence(self):
        i=0
        while i < len(self.stream):
            obj = self.stream[i]
            if obj.type == "Operation":
                #Operation types can accept a maximum of 3 arguments.
                #Try passing 3, then 2, then 1, then 0. If they all fail,
                #syntax error.
                if obj.setArguments(self.stream[i+1:i+4]):
                    symbolLen = 4
                elif obj.setArguments(self.stream[i+1:i+3]):
                    symbolLen = 3
                elif obj.setArguments(self.stream[i+1:i+2]):
                    symbolLen = 2
                elif obj.setArguments([]):
                    symbolLen = 1
                else:
                    acceptable_arguments = "\n\t".join([i for i in ops[obj.name]])
                    err("Incorrect argument pattern for nmemonic \""+obj.name+"\". " +
                        "Acceptable argument patterns are: \n\t"+acceptable_arguments, obj.lineNo)
                    return False
                self.sequence.append(obj)
                i+=symbolLen
            elif obj.type == "LabelDef":
                if i+1 < len(self.stream) and self.stream[i+1].type != "Operation":
                    err("Label definitions must be followed by an operation", obj.lineNo)
                else:
                    self.sequence.append(obj)
                    i+=1
            else:
                i+=1

    def compile(self):
        self.output = ""
        for branch in self.sequence:
            currentLen = len(self.output)
            self.output += branch.compile(currentLen)
        return self.output

parser = Parser(fdata)
parser.tokenize()
print parser.toString()
print

ast = AST(parser)

print 
print "Labels"
print labels
print

print "compiling..."
ast.compile()
open(outputFile, "wb").write(ast.compile())


print 
print "Labels"
print labels
print

#print toLittleEndian32(0)+toLittleEndian32(8)+toLittleEndian32(16)+toLittleEndian32(31)
