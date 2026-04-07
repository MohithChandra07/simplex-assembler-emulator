; test05.asm - Sum of integers 1 to N
; Tests: ldc, a2sp, adj, ldnl, stl, ldl, brz, add, adc, br, stnl, HALT
; N = 10, expected: mem[result] = 55 (0x37)
; Label addresses: loop=8  done=18  n=23  result=24
; Branch offsets:  brz done = +8   br loop = -10 (0xFFFFF6)
        ldc     0x1000
        a2sp
        adj     -2
        ldc     n
        ldnl    0
        stl     0
        ldc     0
        stl     1
loop:   ldl     0
        brz     done
        ldl     1
        ldl     0
        add
        stl     1
        ldl     0
        adc     -1
        stl     0
        br      loop
done:   ldl     1
        ldc     result
        stnl    0
        adj     2
        HALT
n:      data    10
result: data    0