; test_errors.asm - Error and Warning detection test
; This file contains deliberate errors and warnings.
; Your assembler should report ALL of them correctly.
; NO object file should be generated (or only valid lines assembled).
;
; Expected errors   (8):
;   1. Duplicate label         : 'start' defined twice
;   2. Invalid label name      : '2bad' starts with digit
;   3. Undefined label         : 'nowhere' never defined
;   4. Missing operand         : 'ldc' needs a value
;   5. Unexpected operand      : 'add' takes no operand
;   6. Extra text after operand: 'ldc 5 6' has junk after operand
;   7. Invalid mnemonic        : 'jump' is not a SIMPLEX instruction
;   8. Missing operand for data: 'data' with no value
;
; Expected warnings (2):
;   1. Unused label            : 'unused' is never referenced
;   2. Unlabeled data          : 'data 99' has no label
; -------------------------------------------------------

; --- valid setup (these lines should assemble fine) ---
        ldc     0x1000
        a2sp
        adj     -1

; ERROR 1: duplicate label (start defined again below)
start:  ldc     10
        stl     0

start:  ldc     20          ; >> ERROR: Duplicate label: start

; ERROR 2: invalid label name (begins with digit)
2bad:   ldc     5           ; >> ERROR: Invalid label name: 2bad

; WARNING 1: unused label (defined but never used)
unused: ldc     0

; ERROR 3: branch to undefined label
        br      nowhere     ; >> ERROR: Undefined label: nowhere

; ERROR 4: missing operand for ldc
        ldc                 ; >> ERROR: Missing operand for ldc

; ERROR 5: unexpected operand for add
        add     5           ; >> ERROR: Unexpected operand for add

; ERROR 6: extra text after operand
        ldc     5, 6        ; >> ERROR: Extra text after operand

; ERROR 7: invalid mnemonic
        jump    loop        ; >> ERROR: Invalid mnemonic: jump

; ERROR 8: data with no value
baddata: data              ; >> ERROR: Missing operand for data

; WARNING 2: unlabeled data (assembler should warn)
        data    99          ; >> WARNING: Unlabeled data

; --- valid lines to show assembler keeps going after errors ---
        ldc     42
        HALT