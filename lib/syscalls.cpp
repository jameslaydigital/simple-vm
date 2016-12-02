//==DEBUGGING===========================//
void sys_dump(void) {
    printf("sys_dump invoked.\n");
    printf("REGISTERS: \n");
    for ( int i = 0; i < 8; i++ ) {
        //printing general purpose registers:
        printf("\tR%d:\t\t%d\n", i+1, _regs[i]);
    }
    printf("\tinstruction pointer:\t%d (%#1x)\n", _regs[IP], _regs[IP]);
    printf("\t      stack pointer:\t%d (%#1x)\n", _regs[SP], _regs[SP]);
    printf("\t       base pointer:\t%d (%#1x)\n", _regs[BP], _regs[BP]);
    printf("\n\nSTACK SIZE: %lu\n", _stack.size());
    printf("\n\n HEAP SIZE: %lu\n", _buff.size());
    printf("\n\n\t\tGARBAGE DUMPED.\n\n");
    return;
}
