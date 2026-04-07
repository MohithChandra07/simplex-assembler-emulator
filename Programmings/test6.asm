; test07.asm - Multiply two numbers using repeated addition
; Tests: ldc, a2sp, adj, ldnl, stl, ldl, brz, add, adc, br, stnl, HALT
; a=6, b=7, expected result = 42 (0x2A)
;
; Algorithm: result=0; while(b>0){ result+=a; b--; }
;
; Stack layout after adj -3:
;   mem[SP+0] = counter  (b, counts down)
;   mem[SP+1] = addend   (a, fixed)
;   mem[SP+2] = result   (accumulates)
;
; Label addresses: loop=11  done=21  a=26  b=27  result=28
; Branch offsets:  brz done=+8   br loop=-10 (FFFFFFF6)
        ldc     0x1000
        a2sp
        adj     -3
        ldc     a
        ldnl    0
        stl     1
        ldc     b
        ldnl    0
        stl     0
        ldc     0
        stl     2
loop:   ldl     0
        brz     done
        ldl     2
        ldl     1
        add
        stl     2
        ldl     0
        adc     -1
        stl     0
        br      loop
done:   ldl     2
        ldc     result
        stnl    0
        adj     3
        HALT
a:      data    6
b:      data    7
result: data    0