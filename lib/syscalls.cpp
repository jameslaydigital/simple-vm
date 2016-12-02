//==DEBUGGING===========================//
void sys_dump(void) {
    printf("========SYS DUMP========\n");
    printf("REGISTERS: \n");
    for ( int i = 0; i < 8; i++ ) {
        //printing general purpose registers:
        printf("\tR%d:\t\t%d\n", i+1, _regs[i]);
    }
    printf("\tinstruction pointer:\t%d (%#1x)\n", _regs[IP], _regs[IP]);
    printf("\t      stack pointer:\t%d (%#1x)\n", _regs[SP], _regs[SP]);
    printf("\t       base pointer:\t%d (%#1x)\n", _regs[BP], _regs[BP]);
    printf("STACK SIZE: %lu\n", _stack.size());
    printf(" HEAP SIZE: %lu\n", _buff.size());
    printf("========END SYS DUMP====\n");
    return;
}
