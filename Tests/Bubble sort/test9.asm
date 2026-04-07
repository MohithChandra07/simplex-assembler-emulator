; test10.asm - Bubble Sort
; Tests: ldc, a2sp, adj, ldnl, stl, ldl, stnl, sub, brlz, brz, br, adc, HALT
; Array: [5,3,8,1,9,2,7,4,6,0]  N=10
; Expected sorted: [0,1,2,3,4,5,6,7,8,9] stored back in arr
;
; Algorithm: standard bubble sort, outer*inner passes
;   outer = N-1 down to 0
;   inner = outer down to 0, walking pointer p through array
;   if arr[j+1] - arr[j] < 0 (arr[j] > arr[j+1]): swap
;
; Stack layout after adj -4:
;   mem[SP+0] = outer   (outer loop counter)
;   mem[SP+1] = inner   (inner loop counter)
;   mem[SP+2] = p       (pointer to arr[j], walks forward)
;   mem[SP+3] = tmp     (temp during swap = arr[j])
;
; Comparison sequence (critical - stl side-effect must be handled):
;   ldl 2    => A=p
;   ldnl 0   => A=arr[j],    B=garbage  (ldnl only changes A)
;   stl 3    => tmp=arr[j],  A=B(garbage) -- p is LOST from A here!
;   ldl 2    => A=p          (must reload p before ldnl 1)
;   ldnl 1   => A=arr[j+1],  B=p
;   ldl 3    => B=arr[j+1],  A=tmp=arr[j]
;   sub      => A=arr[j+1]-arr[j]  (sub: A=B-A)
;   brlz do_swap  (if arr[j+1]-arr[j]<0 means arr[j]>arr[j+1], swap!)
;   br   no_swap  (already in order)
;
; Swap sequence:
;   ldl 2    => A=p
;   ldnl 1   => A=arr[j+1], B=p_prev
;   ldl 2    => B=arr[j+1], A=p
;   stnl 0   => mem[p+0]=arr[j+1]   arr[j]   = arr[j+1] done
;   ldl 3    => B=p, A=tmp=arr[j]
;   ldl 2    => B=arr[j], A=p
;   stnl 1   => mem[p+1]=arr[j]     arr[j+1] = arr[j]   done
;
; Label addresses: outer=7 inner=13 do_swap=24 no_swap=31 dec_outer=38 done=42 n=44 arr=45
; Branch offsets:  brz done=+33  brz dec_outer=+23  brlz do_swap=+1
;                  br no_swap=+7  br inner=-25  br outer=-35
        ldc     0x1000
        a2sp
        adj     -4
        ldc     n
        ldnl    0
        adc     -1
        stl     0
outer:  ldl     0
        brz     done
        ldc     arr
        stl     2
        ldl     0
        stl     1
inner:  ldl     1
        brz     dec_outer
        ldl     2
        ldnl    0
        stl     3
        ldl     2
        ldnl    1
        ldl     3
        sub
        brlz    do_swap
        br      no_swap
do_swap:ldl     2
        ldnl    1
        ldl     2
        stnl    0
        ldl     3
        ldl     2
        stnl    1
no_swap:ldl     2
        adc     1
        stl     2
        ldl     1
        adc     -1
        stl     1
        br      inner
dec_outer:ldl   0
        adc     -1
        stl     0
        br      outer
done:   adj     4
        HALT
n:      data    10
arr:    data    5
        data    3
        data    8
        data    1
        data    9
        data    2
        data    7
        data    4
        data    6
        data    0