//==DEBUGGING===========================//
void sys_dump(void) {
    printf("\n========SYS DUMP========\n");
    printf("REGISTERS: \n");
    for ( int i = 0; i < MAX_REG_INDEX; i++ ) {
        //printing general purpose registers:
        printf("\tR%d:\t\t%#1x\n", i, _regs[i]);
    }
    printf("\tCMP:\t\t%#1x\n", _regs[CMP]);

    printf("\tinstruction pointer:\t%#1x\n", _regs[IP]);
    printf("\t      stack pointer:\t%#1x\n", _regs[SP]);
    printf("\t       base pointer:\t%#1x\n", _regs[BP]);
    printf(" MEM SIZE: %lu\n", _buff_size);
    printf("========END SYS DUMP====\n\n");
    return;
}

void sys_exit(void) {
    exit(0);
}

void sys_print(void) {
    //write(int fildes, const void *buf, size_t nbyte);
    unsigned int base = bytesToUInt(_buff+_regs[SP]);
    unsigned int length = bytesToUInt(_buff+(_regs[SP]+4));
    write(1, _buff+base, length);
}

void sys_read(void) {
    //read(int fildes, void *buf, size_t nbyte);
    unsigned int base = bytesToUInt(_buff+_regs[SP]);
    unsigned int length = bytesToUInt(_buff+(_regs[SP]+4));
    read(0, _buff+base, length);
}
