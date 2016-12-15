debug 0 0 0 ;; always required to start the program
push 8
push .printme
.start: 
    syscall print
    jump .start
syscall exit

;;;; DATA BED SECTION      ;;;;
;;;; no jumping on the bed ;;;;
.printme: db 0x30 0x31 0x32 0x33
          db 0x34 0x35 0x36 0x00
