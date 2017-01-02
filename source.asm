.start: debug 0 0 0 ;; always required to start the program
push 20
push .printme
syscall print
pop R1
push .readme
syscall read
syscall print
syscall exit

;;;; DATA BED SECTION      ;;;;
;;;; no jumping on the bed ;;;;
.printme: ds "How old are you? > "
.readme: ds "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
