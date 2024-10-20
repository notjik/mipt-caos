.global f

f:
    // x0 хранит A
    // x1 хранит B
    // x2 хранит C
    // x3 хранит x

    mul x0, x0, x0
    mul x0, x0, x0
    
    mul x1, x1, x0 

    add x0, x0, x1
    add x0, x0, x2

    ret
