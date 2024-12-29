.intel_syntax noprefix
.text

.text
    .global sum

sum: 
    // rdi = int N
    // rsi = const int *A 
    // rdx = const int *B 
    // rcx = int *R
    cmp rdi, 0
    jz end

.loop:
    mov eax, [rsi]
    add eax, [rdx]
    mov [rcx], eax

    add rsi, 4
    add rdx, 4
    add rcx, 4

    dec rdi
    jnz .loop

end:
    ret