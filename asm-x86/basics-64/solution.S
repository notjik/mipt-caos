.intel_syntax noprefix
.section .data
    input_fmt:  .asciz "%d"             // Формат для scanf
    output_fmt: .asciz "%lld "            // Формат для printf (lld для 64-битных)

.section .bss
    input_val:  .skip 4                   // Изменено на 8 байт для хранения long long

.section .text
    .global very_important_function
    .extern A

very_important_function:
    // Входные параметры:
    // RDI - N (размер массива A)
    mov rax, rdi
    xor rcx, rcx // size_t i = 0

.loop:
    cmp rcx, rax
    jae .end_loop


    // Ввод числа с клавиатуры
    lea rsi, [input_val]        // Загружаем адрес input_val в rsi
    lea rdi, [input_fmt]        // Загружаем адрес формата для scanf
    call scanf                  // Вызов scanf

    inc rcx
    jmp .loop

.end_loop:
    ret
