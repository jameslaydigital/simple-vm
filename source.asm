loadR2const 5
loadR1const 0
addR1reg R2
addR1reg R2
label: somewhere
syscall sys_dump
jump somewhere
