#ifndef __VM_H_INCLUDED__
#define __VM_H_INCLUDED__

typedef void (*op)(int);
typedef void(*sysc)(void);
std::unordered_map <int, op>_fmap;
std::unordered_map <int, sysc>_syscalls;
std::vector <int>_stack(1024);
std::vector <char>_buff;
int _regs[NUM_REGS];
int loadBuffer(char*);
int fail(int);
int executionLoop();
int execute(int);
void loadFunctionTable();
void loadSysCalls();

#endif
