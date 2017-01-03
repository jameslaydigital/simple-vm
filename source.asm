debug 0 0 0 ;; always required to start the program
call .testfxn
call .testfxn
call .testfxn
jump .end
.testfxn: push BP
    mov BP SP
        push 10
        push .msg
        syscall print
    mov SP BP
    pop BP
    ret
.end: syscall exit
.msg: ds "hi there!!"
