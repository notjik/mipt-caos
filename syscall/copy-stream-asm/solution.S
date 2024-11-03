#include <sys/syscall.h>
.intel_syntax noprefix
.text
    .global _start

_start:
    sub rsp, 32

read:
    mov rax, SYS_read
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 32
    syscall

    cmp rax, 0
    je end

    mov rdx, rax
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, rsp
    syscall

    jmp read

end:
    add rsp, 32
    mov rax, SYS_exit
    mov rdi, 0
    syscall
