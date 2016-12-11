debugOp 0                       ;to get it started ______________________________________
movRegConst 0000 0300 0000      ;mov R0 0x3 _____________________________________________
syscall dump                    ;R0 should be 3
xorRegConst 0000 0000 0000      ;xor R0 0x2 (should put 2 in r0)
syscall dump                    ;R0 should be 0
syscall exit
EOF                             ;________________________________________________________

;;R0 should be 2
;;R1 should be 2
;;
;;
;; TODO : you were trying to find segfault location.
;;      
;;      - pushReg: [OK]
;;      - popReg:  [ERR]
;;      - popReg:  [ERR]
;;          - bytesToUInt: [WARN]
;;              - toUInt16: [OK]
;;      - sys_dump [OK]
;;
;; popReg seems to finish but the segfault happens right afterward.
;; perhaps it's because the program isn't terminating before it runs the
;; jibberish that was unallocated by malloc. Perhaps null-initializing blank
;; space?
;; Blank space zero-filled and aligned for 32 bit integers. This shouldn't be
;; the problem anymore.
;;
;; Problem located: you didn't assign a function to _syscalls for vector 0x1
;; (exit) in loadSysCalls();
;; 
;; Problem fixed.
;; 
;;00000020: 0f00 0100 0000 0000  ........
