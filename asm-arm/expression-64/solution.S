.global f

f:
    // x0: A
    // x1: B
    // x2: C
    // x3: x

    mul x4, x3, x3
    mul x4, x4, x0

    mul x5, x1, x3

    add x4, x4, x5
    add x0, x4, x2

    ret