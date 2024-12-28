.global sum

sum:
    // w0: x0
    // x1: N
    // x2: *X

    mov x3, 0  // x3:  i

loop:
    cmp x3, x1
    bge end_loop

    ldr w4, [x2, x3, lsl #2]
    add w0, w0, w4
    add x3, x3, 1
    b loop

end_loop:
    ret
