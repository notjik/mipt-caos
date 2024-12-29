.intel_syntax noprefix
.text
.global _start

_start:
    // Сохраняем указатель стека
    push rbp
    mov rbp, rsp

    // Выделяем память с помощью системного вызова mmap
    // rax = 9 (номер системного вызова mmap)
    // rdi = NULL (позволяем системе выбрать адрес)
    // rsi = 32MB (размер буфера)
    // rdx = 3 (PROT_READ | PROT_WRITE - права на чтение и запись)
    // r10 = 34 (MAP_PRIVATE | MAP_ANONYMOUS - приватная анонимная память)
    // r8 = -1 (файловый дескриптор не используется)
    // r9 = 0 (смещение в файле не используется)
    mov rax, 9           
    xor rdi, rdi        
    mov rsi, 33554432   // 32MB = 32 * 1024 * 1024
    mov rdx, 3          
    mov r10, 34         
    mov r8, -1          
    xor r9, r9          
    syscall

    // Проверяем успешность выделения памяти
    cmp rax, -1
    je exit_error

    // Инициализируем указатели для работы с буфером
    // r12 = начало буфера
    // r13 = текущая позиция для записи
    // r14 = общее количество прочитанных байт
    mov r12, rax        
    mov r13, rax        
    xor r14, r14        

read_loop:
    // Читаем данные из stdin блоками по 8KB
    // rax = 0 (системный вызов read)
    // rdi = 0 (stdin)
    // rsi = текущая позиция в буфере
    // rdx = 8192 (размер блока чтения)
    mov rax, 0          
    xor rdi, rdi        
    mov rsi, r13        
    mov rdx, 8192       
    syscall

    // Проверяем конец файла или ошибку чтения
    test rax, rax
    jle read_done

    // Обновляем указатели после успешного чтения
    add r13, rax        // Сдвигаем позицию записи
    add r14, rax        // Увеличиваем счетчик прочитанных байт
    jmp read_loop

read_done:
    // Подготавливаем указатели для обработки буфера в обратном порядке
    // r13 = текущая позиция (с конца)
    // r15 = конец текущей строки
    mov r13, r12        
    add r13, r14        
    mov r15, r13        
    dec r13             

find_lines:
    // Проверяем, не дошли ли до начала буфера
    cmp r13, r12
    jl print_remaining

    // Ищем символ новой строки
    cmp byte ptr [r13], 10
    je found_line

    dec r13
    jmp find_lines

found_line:
    // Сохраняем указатели текущей строки
    push r13
    push r15

    // Выводим строку с помощью системного вызова write
    // rax = 1 (системный вызов write)
    // rdi = 1 (stdout)
    // rsi = адрес начала строки (после \n)
    // rdx = длина строки
    mov rax, 1          
    mov rdi, 1          
    lea rsi, [r13 + 1]  
    mov rdx, r15
    sub rdx, r13
    dec rdx             
    syscall

    // Выводим символ новой строки
    mov rax, 1
    mov rdi, 1
    push 10
    mov rsi, rsp
    mov rdx, 1
    syscall
    add rsp, 8

    // Восстанавливаем указатели и продолжаем поиск
    pop r15
    pop r13
    mov r15, r13
    dec r13
    jmp find_lines

print_remaining:
    // Проверяем, осталась ли первая строка
    cmp r15, r12
    jle cleanup

    // Выводим первую строку (последнюю в исходном тексте) без \n
    mov rax, 1
    mov rdi, 1
    mov rsi, r12
    mov rdx, r15
    sub rdx, r12
    syscall

cleanup:
    // Освобождаем выделенную память через munmap
    // rax = 11 (системный вызов munmap)
    // rdi = адрес начала буфера
    // rsi = размер буфера
    mov rax, 11         
    mov rdi, r12        
    mov rsi, 33554432   
    syscall

exit_program:
    // Завершаем программу с кодом 0 (успех)
    mov rax, 60         
    xor rdi, rdi        
    syscall

exit_error:
    // Завершаем программу с кодом 1 (ошибка)
    mov rax, 60         
    mov rdi, 1          
    syscall