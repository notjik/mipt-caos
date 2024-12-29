# CAOS: Архитектура компьютеров и операционных систем

## 1. Взаимодействие с памятью

### Теоретические основы:

#### 1. Организация памяти:
- **Адресное пространство**
  - Линейная последовательность байтов
  - Каждый байт имеет уникальный адрес
  - В x86 используется little-endian порядок байтов

- **Сегментация памяти**
  - Сегмент кода (.text)
  - Сегмент данных (.data)
  - Сегмент стека (.stack)
  - Сегмент кучи (.heap)

- **Выравнивание данных**
  - Word (2 байта) - по границе 2 байт
  - Double word (4 байта) - по границе 4 байт
  - Quad word (8 байт) - по границе 8 байт

#### 2. Способы адресации:

1. **Регистровая адресация**
   ```assembly
   mov eax, ebx        ; Копирование из регистра в регистр
   xor edx, edx        ; Очистка регистра
   ```

2. **Непосредственная адресация**
   ```assembly
   mov eax, 42         ; Загрузка константы
   add ecx, 100        ; Прибавление константы
   ```

3. **Прямая адресация**
   ```assembly
   mov eax, [address]  ; Загрузка из конкретного адреса
   mov [var], ebx      ; Сохранение по адресу переменной
   ```

4. **Косвенная адресация**
   ```assembly
   mov eax, [ebx]      ; Загрузка из адреса в ebx
   mov [esi], cl       ; Сохранение в адрес в esi
   ```

5. **Базовая адресация с индексом**
   ```assembly
   mov eax, [ebx + esi*4]     ; Доступ к элементу массива
   mov edx, [ebp - 8]         ; Доступ к локальной переменной
   ```

### Основные инструкции:

#### 1. MOV (Move)
```assembly
section .data
    var dd 42

section .text
    mov eax, ebx            ; Регистр в регистр
    mov [var], eax         ; Регистр в память
    mov eax, [var]         ; Память в регистр
    mov byte [var], 42     ; Константа в память
    movzx eax, bl          ; Расширение с нулями
    movsx eax, bl          ; Расширение со знаком
```

#### 2. LEA (Load Effective Address)
```assembly
section .data
    array dd 1,2,3,4

section .text
    lea eax, [array]       ; Загрузка адреса массива
    lea edx, [eax + ebx*4] ; Вычисление сложного адреса
    lea ecx, [eax + 8]     ; Быстрое сложение
```

#### 3. PUSH/POP
```assembly
; Работа со стеком
push eax                ; Сохранение в стек
push dword [var]        ; Сохранение значения из памяти
pop  ebx                ; Извлечение из стека
pusha                   ; Сохранение всех регистров
popa                    ; Восстановление всех регистров
```

#### 4. XCHG (Exchange)
```assembly
xchg eax, ebx          ; Обмен значениями регистров
xchg [var], eax        ; Обмен с памятью
```

### Практические примеры:

#### 1. Работа с массивом:
```assembly
section .data
    array dd 1, 2, 3, 4, 5
    len   equ 5

section .text
global _start
_start:
    mov esi, array          ; Указатель на начало массива
    mov ecx, len           ; Счетчик элементов
    xor eax, eax          ; Сумма элементов

array_sum:
    add eax, [esi]        ; Добавление текущего элемента
    add esi, 4            ; Следующий элемент
    loop array_sum        ; Уменьшение ecx и переход если не 0
```

#### 2. Копирование строки:
```assembly
section .data
    source db "Hello", 0
    dest   times 6 db 0

section .text
global _start
_start:
    mov esi, source      ; Источник
    mov edi, dest        ; Назначение
    mov ecx, 6          ; Длина + 1 (для нуля)
    
copy_loop:
    mov al, [esi]       ; Загрузка байта
    mov [edi], al       ; Сохранение байта
    inc esi             ; Следующий байт источника
    inc edi             ; Следующий байт назначения
    loop copy_loop
```

#### 3. Работа с структурами:
```assembly
struc Person
    .name   resb 32
    .age    resd 1
    .salary resd 1
endstruc

section .data
    person1:
        istruc Person
            at Person.name,   db "John Doe", 0
            at Person.age,    dd 30
            at Person.salary, dd 50000
        iend

section .text
    ; Доступ к полям структуры
    mov eax, [person1 + Person.age]    ; Загрузка возраста
    add dword [person1 + Person.salary], 1000  ; Увеличение зарплаты
```

### Оптимизация и лучшие практики:

1. **Выравнивание данных:**
```assembly
section .data align=16
    vector dd 1.0, 2.0, 3.0, 4.0   ; Выровненный вектор для SSE
```

2. **Минимизация обращений к памяти:**
```assembly
; Плохо
mov eax, [var]
inc eax
mov [var], eax

; Хорошо
mov eax, [var]
inc eax
add eax, ebx
mov [var], eax  ; Одно сохранение в память
```

3. **Использование LEA для арифметики:**
```assembly
; Вместо
mov eax, ebx
add eax, ecx
add eax, 5

; Используем
lea eax, [ebx + ecx + 5]
```

### Отладка и типичные ошибки:

1. **Segmentation Fault:**
```assembly
; Неправильно - обращение к неверному адресу
mov eax, [0]  

; Правильно - проверка адреса
cmp ebx, 0
je error_handler
mov eax, [ebx]
```

2. **Проблемы выравнивания:**
```assembly
; Может вызвать проблемы на некоторых процессорах
movaps xmm0, [unaligned_data]

; Правильно
align 16
aligned_data: dd 1.0, 2.0, 3.0, 4.0
movaps xmm0, [aligned_data]
```

## 2. Условные переходы в ассемблере

### Теоретические основы:

#### 1. Регистр флагов (FLAGS/EFLAGS):
- **ZF (Zero Flag)**
  - Устанавливается, если результат равен нулю
  - Сбрасывается в противном случае

- **SF (Sign Flag)**
  - Устанавливается, если результат отрицательный
  - Копирует старший бит результата

- **CF (Carry Flag)**
  - Устанавливается при переносе из старшего бита
  - Используется в операциях без знака

- **OF (Overflow Flag)**
  - Устанавливается при арифметическом переполнении
  - Важен для операций со знаком

#### 2. Инструкции сравнения:

1. **CMP (Compare)**
   ```assembly
   cmp eax, ebx        ; Вычитает второй операнд из первого
   cmp ecx, 100        ; Сравнение с константой
   cmp byte [var], 0   ; Сравнение байта в памяти
   ```

2. **TEST (Logical Compare)**
   ```assembly
   test eax, eax       ; Проверка на ноль
   test al, 1          ; Проверка младшего бита
   test dx, 0x8000     ; Проверка знакового бита
   ```

### Типы условных переходов:

#### 1. Переходы по равенству:
```assembly
je  label    ; Jump if Equal (ZF=1)
jne label    ; Jump if Not Equal (ZF=0)
jz  label    ; Jump if Zero (ZF=1)
jnz label    ; Jump if Not Zero (ZF=0)
```

#### 2. Переходы по знаку:
```assembly
js  label    ; Jump if Sign (SF=1)
jns label    ; Jump if Not Sign (SF=0)
```

#### 3. Переходы для чисел без знака:
```assembly
ja  label    ; Jump if Above (CF=0 and ZF=0)
jae label    ; Jump if Above or Equal (CF=0)
jb  label    ; Jump if Below (CF=1)
jbe label    ; Jump if Below or Equal (CF=1 or ZF=1)
```

#### 4. Переходы для чисел со знаком:
```assembly
jg  label    ; Jump if Greater (ZF=0 and SF=OF)
jge label    ; Jump if Greater or Equal (SF=OF)
jl  label    ; Jump if Less (SF≠OF)
jle label    ; Jump if Less or Equal (ZF=1 or SF≠OF)
```

### Практические примеры:

#### 1. Простое ветвление:
```assembly
section .text
global check_number
check_number:
    mov eax, [esp+4]    ; Получаем параметр
    cmp eax, 0          ; Сравниваем с нулем
    jg  positive        ; Если больше нуля
    jl  negative        ; Если меньше нуля
    
zero:
    mov eax, 0          ; Возвращаем 0
    ret
    
positive:
    mov eax, 1          ; Возвращаем 1
    ret
    
negative:
    mov eax, -1         ; Возвращаем -1
    ret
```

#### 2. Цикл с условием:
```assembly
section .data
    array dd 1, 2, 3, 4, 5
    len   equ 5

section .text
global find_element
find_element:
    mov ecx, len        ; Счетчик
    mov esi, array      ; Указатель на массив
    mov edx, [esp+4]    ; Искомое значение

search_loop:
    mov eax, [esi]      ; Загружаем текущий элемент
    cmp eax, edx        ; Сравниваем с искомым
    je  found           ; Если равны - нашли
    add esi, 4          ; Следующий элемент
    loop search_loop    ; Уменьшаем счетчик и повторяем
    
    xor eax, eax        ; Не найдено - возвращаем 0
    ret

found:
    mov eax, 1          ; Найдено - возвращаем 1
    ret
```

#### 3. Сложное условие:
```assembly
; if (x > 0 && y < 10) || z == 0
section .text
complex_condition:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp+8]    ; x
    mov ebx, [ebp+12]   ; y
    mov ecx, [ebp+16]   ; z
    
    ; Проверка x > 0
    cmp eax, 0
    jle check_z
    
    ; Проверка y < 10
    cmp ebx, 10
    jge check_z
    
    jmp condition_true
    
check_z:
    ; Проверка z == 0
    test ecx, ecx
    jz condition_true
    
condition_false:
    xor eax, eax        ; return false
    jmp end
    
condition_true:
    mov eax, 1          ; return true
    
end:
    mov esp, ebp
    pop ebp
    ret
```

### Оптимизация условных переходов:

#### 1. Предсказание переходов:
```assembly
; Лучше
cmp eax, 0          ; Чаще встречающийся случай
jge positive        ; должен идти первым
; Обработка отрицательного случая
jmp end
positive:
; Обработка положительного случая
end:
```

#### 2. Избегание переходов:
```assembly
; Вместо условного перехода
mov eax, 0          ; Начальное значение
cmp edx, 0          ; Сравнение
setg al             ; Установка AL если больше
```

#### 3. Использование CMOVcc:
```assembly
; Условное копирование (доступно на современных процессорах)
mov eax, value1
mov ebx, value2
cmp ecx, 0
cmovg eax, ebx      ; eax = ebx если ecx > 0
```

### Отладка и типичные ошибки:

1. **Бесконечные циклы:**
```assembly
; Неправильно - возможен бесконечный цикл
loop_start:
    inc eax
    cmp eax, ebx
    jl loop_start

; Правильно - с защитой от зацикливания
    mov ecx, max_iterations
loop_start:
    inc eax
    cmp eax, ebx
    jge loop_end
    loop loop_start    ; Безопасный выход по ecx
loop_end:
```

2. **Правильное сравнение:**
```assembly
; Неправильно для чисел со знаком
cmp eax, ebx
ja greater           ; Использование ja вместо jg

; Правильно
cmp eax, ebx
jg greater          ; Использование jg для знаковых чисел
```

## 3. Системный вызов fork()

### Теоретические основы:

#### 1. Концепция процессов:
- **Процесс** - это:
  - Выполняющаяся программа
  - Изолированное адресное пространство
  - Контекст выполнения
  - Набор системных ресурсов

- **Атрибуты процесса:**
  ```c
  struct task_struct {
      pid_t pid;              // ID процесса
      pid_t ppid;             // ID родительского процесса
      uid_t uid, euid;        // User ID
      gid_t gid, egid;        // Group ID
      struct mm_struct *mm;   // Адресное пространство
      struct files_struct *files;  // Открытые файлы
      // ...
  };
  ```

#### 2. Механизм fork():

- **Copy-on-Write (COW):**
  - Страницы памяти помечаются как read-only
  - При записи создается новая копия страницы
  - Экономия памяти и ускорение создания процесса

- **Наследуемые ресурсы:**
  - Файловые дескрипторы
  - Рабочая директория
  - Маски сигналов
  - Лимиты ресурсов

### Практические примеры:

#### 1. Базовый пример:
```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Начало программы: PID=%d\n", getpid());
    
    int x = 100;
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Ребёнок: x=%d, PID=%d, PPID=%d\n", 
               x, getpid(), getppid());
        x = 200;
    } else {
        // Родительский процесс
        printf("Родитель: x=%d, PID=%d, child PID=%d\n", 
               x, getpid(), pid);
        x = 300;
    }
    
    printf("PID %d: Конечное значение x=%d\n", getpid(), x);
    return 0;
}
```

#### 2. Пример с файловыми дескрипторами:
```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    int fd = open("test.txt", O_RDWR | O_CREAT, 0644);
    write(fd, "Before fork\n", 12);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс
        write(fd, "Child writes\n", 13);
        lseek(fd, 0, SEEK_SET);  // Перемещение указателя в начало
    } else {
        // Родительский процесс
        write(fd, "Parent writes\n", 14);
    }
    
    close(fd);
    return 0;
}
```

#### 3. Пример с разделяемой памятью:
```c
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // Создание разделяемой памяти
    int *shared = mmap(NULL, sizeof(int),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS,
                      -1, 0);
    *shared = 100;
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Child: before=%d\n", *shared);
        *shared = 200;
        printf("Child: after=%d\n", *shared);
    } else {
        // Родительский процесс
        sleep(1);  // Ждем изменения от ребенка
        printf("Parent: value=%d\n", *shared);
    }
    
    munmap(shared, sizeof(int));
    return 0;
}
```

### Особые случаи и обработка ошибок:

#### 1. Обработка ошибок fork():
```c
pid_t pid = fork();
if (pid == -1) {
    // Анализ errno
    switch (errno) {
        case EAGAIN:  // Достигнут лимит процессов
            fprintf(stderr, "Resource limit reached\n");
            break;
        case ENOMEM:  // Нехватка памяти
            fprintf(stderr, "Out of memory\n");
            break;
        default:
            perror("fork");
    }
    exit(1);
}
```

#### 2. Зомби-процессы:
```c
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        exit(42);  // Дочерний процесс завершается
    } else {
        int status;
        // Ожидание завершения потомка
        pid_t terminated_pid = wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Child exited with code %d\n", 
                   WEXITSTATUS(status));
        }
    }
    return 0;
}
```

### Оптимизация и лучшие практики:

1. **Избегание утечек ресурсов:**
```c
int main() {
    FILE *f = fopen("test.txt", "w");
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний использует файл
        fprintf(f, "Child\n");
        fclose(f);  // Важно закрыть в обоих процессах
        exit(0);
    }
    
    // Родительский тоже использует файл
    fprintf(f, "Parent\n");
    fclose(f);
    wait(NULL);  // Ожидание завершения потомка
    return 0;
}
```

2. **Обработка сигналов:**
```c
#include <signal.h>

void handle_child(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    // Установка обработчика SIGCHLD
    struct sigaction sa;
    sa.sa_handler = handle_child;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
    
    // Далее fork() и основной код
}
```

### Типичные ошибки:

1. **Буферизация stdout:**
```c
printf("Before fork");  // Может появиться дважды!
pid_t pid = fork();

// Правильно:
printf("Before fork\n");  // С \n
fflush(stdout);          // Или явный сброс
pid_t pid = fork();
```

2. **Race conditions:**
```c
// Неправильно:
if (fork() == 0) {
    sleep(1);
    exit(0);
}
exit(0);  // Родитель может завершиться раньше потомка

// Правильно:
if (fork() == 0) {
    sleep(1);
    exit(0);
}
wait(NULL);  // Ожидание завершения потомка
exit(0);
```

## 4. Связка fork + exec

### Теоретические основы:

#### 1. Семейство exec():
- **execl** - список аргументов
- **execv** - массив аргументов
- **execle** - список + окружение
- **execve** - массив + окружение
- **execlp/execvp** - поиск в PATH

```c
// Прототипы функций:
int execl(const char *path, const char *arg0, ..., NULL);
int execv(const char *path, char *const argv[]);
int execle(const char *path, const char *arg0, ..., NULL, char *const envp[]);
int execve(const char *path, char *const argv[], char *const envp[]);
int execlp(const char *file, const char *arg0, ..., NULL);
int execvp(const char *file, char *const argv[]);
```

#### 2. Что сохраняется при exec():
- PID процесса
- PPID
- Файловые дескрипторы (если не установлен FD_CLOEXEC)
- Real/Effective User/Group ID
- Текущая директория
- Маски сигналов
- Nice значение
- Resource limits

#### 3. Что заменяется при exec():
- Код программы
- Данные
- Стек
- Heap
- Память и ресурсы
- Сигнальные обработчики

### Практические примеры:

#### 1. Базовый пример fork + exec:
```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Child process (PID=%d) about to exec ls\n", getpid());
        
        execl("/bin/ls", "ls", "-l", NULL);
        // Если мы здесь, значит произошла ошибка
        perror("execl failed");
        exit(1);
    } else {
        // Родительский процесс
        printf("Parent waiting for child (PID=%d)\n", pid);
        int status;
        wait(&status);
        printf("Child finished with status %d\n", 
               WEXITSTATUS(status));
    }
    return 0;
}
```

#### 2. Пример с передачей окружения:
```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Создаем новое окружение
    char *new_env[] = {
        "PATH=/bin:/usr/bin",
        "USER=test",
        "HOME=/home/test",
        NULL
    };
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Аргументы для новой программы
        char *args[] = {
            "env",     // argv[0] - имя программы
            NULL
        };
        
        // Запуск программы env с новым окружением
        execve("/usr/bin/env", args, new_env);
        perror("execve failed");
        exit(1);
    }
    
    wait(NULL);
    return 0;
}
```

#### 3. Пример с перенаправлением ввода/вывода:
```c
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Открываем файл для записи
        int fd = open("output.txt", 
                     O_WRONLY | O_CREAT | O_TRUNC, 
                     0644);
        
        if (fd == -1) {
            perror("open failed");
            exit(1);
        }
        
        // Перенаправляем stdout в файл
        dup2(fd, STDOUT_FILENO);
        close(fd);
        
        // Запускаем ls
        execlp("ls", "ls", "-l", NULL);
        perror("execlp failed");
        exit(1);
    }
    
    wait(NULL);
    return 0;
}
```

### Оптимизация и лучшие практики:

#### 1. Правильная обработка ошибок:
```c
#include <errno.h>

pid_t pid = fork();
if (pid == 0) {
    execl("/path/to/program", "program", NULL);
    
    // Детальный анализ ошибок exec
    switch (errno) {
        case ENOENT:
            fprintf(stderr, "Program not found\n");
            break;
        case EACCES:
            fprintf(stderr, "Permission denied\n");
            break;
        default:
            perror("exec failed");
    }
    exit(1);
}
```

#### 2. Закрытие ненужных файловых дескрипторов:
```c
#include <limits.h>
#include <fcntl.h>

void close_all_fds(void) {
    int max_fd = sysconf(_SC_OPEN_MAX);
    for (int fd = 3; fd < max_fd; fd++) {
        close(fd);
    }
}

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        close_all_fds();  // Закрываем все лишние дескрипторы
        execl("/path/to/program", "program", NULL);
        exit(1);
    }
}
```

#### 3. Установка сигналов по умолчанию:
```c
void reset_signals(void) {
    for (int i = 1; i < NSIG; i++) {
        signal(i, SIG_DFL);
    }
}

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        reset_signals();
        execl("/path/to/program", "program", NULL);
        exit(1);
    }
}
```

### Типичные ошибки:

1. **Незакрытые файловые дескрипторы:**
```c
// Неправильно:
FILE *f = fopen("log.txt", "w");
if (fork() == 0) {
    execl("/bin/ls", "ls", NULL);
}

// Правильно:
FILE *f = fopen("log.txt", "w");
if (fork() == 0) {
    fclose(f);  // Закрываем перед exec
    execl("/bin/ls", "ls", NULL);
}
fclose(f);  // Закрываем в родителе
```

2. **Некорректная обработка путей:**
```c
// Неправильно:
execl("program", "program", NULL);  // Может не найти программу

// Правильно:
execl("/full/path/to/program", "program", NULL);
// Или
execlp("program", "program", NULL);  // Поиск в PATH
```

3. **Отсутствие проверки возврата wait:**
```c
// Неправильно:
wait(NULL);

// Правильно:
int status;
pid_t terminated_pid = wait(&status);
if (terminated_pid == -1) {
    perror("wait failed");
} else {
    if (WIFEXITED(status)) {
        printf("Exit code: %d\n", WEXITSTATUS(status));
    }
}
```

## 5. Неименованные каналы (Pipes)

### Теоретические основы:

#### 1. Структура pipe:
- **Буфер в ядре:**
  - Размер по умолчанию (обычно 64KB)
  - FIFO (First In, First Out) организация
  - Атомарные операции чтения/записи до PIPE_BUF

- **Два файловых дескриптора:**
  ```c
  int fd[2];  // fd[0] - чтение, fd[1] - запись
  ```

#### 2. Характеристики:
- Однонаправленная передача
- Блокирующие операции по умолчанию
- Автоматическая синхронизация
- Сигналы SIGPIPE при записи в закрытый пайп

### Практические примеры:

#### 1. Базовый пример использования pipe:
```c
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buf[100];
    const char *message = "Hello through pipe!";
    
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {  // Дочерний процесс
        close(pipefd[1]);  // Закрываем запись
        
        ssize_t bytes_read = read(pipefd[0], buf, sizeof(buf));
        printf("Child received: %.*s\n", (int)bytes_read, buf);
        
        close(pipefd[0]);
    } else {  // Родительский процесс
        close(pipefd[0]);  // Закрываем чтение
        
        write(pipefd[1], message, strlen(message));
        close(pipefd[1]);
        
        wait(NULL);
    }
    
    return 0;
}
```

#### 2. Неблокирующий режим:
```c
#include <fcntl.h>

int main() {
    int pipefd[2];
    pipe(pipefd);
    
    // Установка неблокирующего режима
    int flags = fcntl(pipefd[0], F_GETFL);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    
    char buf[100];
    ssize_t bytes_read;
    
    while (1) {
        bytes_read = read(pipefd[0], buf, sizeof(buf));
        if (bytes_read < 0) {
            if (errno == EAGAIN) {
                // Данных нет - делаем что-то другое
                usleep(100000);  // 100ms
                continue;
            }
            perror("read error");
            break;
        }
        // Обработка данных
        process_data(buf, bytes_read);
    }
}
```

#### 3. Конвейер команд (pipe chain):
```c
#include <unistd.h>
#include <stdio.h>

int main() {
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);
    
    if (fork() == 0) {  // Первый процесс (ls)
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        execlp("ls", "ls", "-l", NULL);
        exit(1);
    }
    
    if (fork() == 0) {  // Второй процесс (grep)
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        execlp("grep", "grep", "^d", NULL);
        exit(1);
    }
    
    if (fork() == 0) {  // Третий процесс (wc)
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        execlp("wc", "wc", "-l", NULL);
        exit(1);
    }
    
    // Закрываем все дескрипторы в родителе
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    
    // Ждем завершения всех процессов
    wait(NULL);
    wait(NULL);
    wait(NULL);
    
    return 0;
}
```

### Оптимизация и лучшие практики:

#### 1. Правильное закрытие дескрипторов:
```c
void close_pipe(int pipefd[2]) {
    if (pipefd[0] != -1) {
        close(pipefd[0]);
        pipefd[0] = -1;
    }
    if (pipefd[1] != -1) {
        close(pipefd[1]);
        pipefd[1] = -1;
    }
}

int main() {
    int pipefd[2] = {-1, -1};
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    
    // Использование pipe
    
    close_pipe(pipefd);
    return 0;
}
```

#### 2. Обработка SIGPIPE:
```c
#include <signal.h>

void handle_sigpipe(int sig) {
    // Обработка сигнала
    fprintf(stderr, "Broken pipe detected\n");
}

int main() {
    // Установка обработчика
    signal(SIGPIPE, handle_sigpipe);
    
    // Или игнорирование сигнала
    signal(SIGPIPE, SIG_IGN);
    
    // Работа с pipe
}
```

### Типичные ошибки:

1. **Забытые открытые дескрипторы:**
```c
// Неправильно:
if (fork() == 0) {
    write(pipefd[1], data, size);
    exit(0);
}

// Правильно:
if (fork() == 0) {
    close(pipefd[0]);  // Закрываем ненужный конец
    write(pipefd[1], data, size);
    close(pipefd[1]);
    exit(0);
}
```

2. **Deadlock при работе с pipe:**
```c
// Неправильно (возможен deadlock):
write(pipefd[1], large_data, huge_size);
read(pipefd[0], buffer, size);

// Правильно:
// Использовать разные процессы/потоки для чтения и записи
// Или использовать неблокирующий режим
```

3. **Недостаточная проверка ошибок:**
```c
// Неправильно:
write(pipefd[1], data, size);

// Правильно:
ssize_t written = 0;
while (written < size) {
    ssize_t ret = write(pipefd[1], 
                       data + written, 
                       size - written);
    if (ret == -1) {
        if (errno == EINTR) continue;
        perror("write error");
        break;
    }
    written += ret;
}
```

## 6. Неблокирующий ввод-вывод

### Теоретические основы:

#### 1. Модели ввод-вывода:
- **Блокирующий I/O**
  - Процесс ждет завершения операции
  - Простой в использовании
  - Неэффективное использование CPU

- **Неблокирующий I/O**
  - Немедленный возврат управления
  - Возможность параллельной работы
  - Требует постоянной проверки готовности

- **Мультиплексирование I/O**
  - select/poll/epoll
  - Одновременная работа с множеством дескрипторов
  - Эффективное использование ресурсов

#### 2. Флаги и режимы:
```c
// Основные флаги
O_NONBLOCK    // Неблокирующий режим
O_ASYNC       // Асинхронный I/O
FIONBIO       // Альтернативный способ установки неблокирующего режима
```

### Практические примеры:

#### 1. Базовое использование неблокирующего I/O:
```c
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }
    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }
    
    return 0;
}

int main() {
    char buffer[1024];
    
    // Установка stdin в неблокирующий режим
    if (set_nonblock(STDIN_FILENO) == -1) {
        return 1;
    }
    
    while (1) {
        ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
        
        if (bytes > 0) {
            // Есть данные
            write(STDOUT_FILENO, buffer, bytes);
        } else if (bytes == 0) {
            // Конец файла
            break;
        } else if (bytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Нет доступных данных
                printf("No data available\n");
                sleep(1);
                continue;
            }
            // Реальная ошибка
            perror("read error");
            break;
        }
    }
    
    return 0;
}
```

#### 2. Использование select():
```c
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    fd_set readfds;
    struct timeval tv;
    char buffer[1024];
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        // Таймаут 1 секунда
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        
        if (ret == -1) {
            perror("select error");
            break;
        } else if (ret == 0) {
            printf("Timeout occurred\n");
            continue;
        }
        
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
            if (bytes > 0) {
                write(STDOUT_FILENO, buffer, bytes);
            }
        }
    }
    
    return 0;
}
```

#### 3. Использование epoll (Linux):
```c
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_EVENTS 10

int main() {
    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        return 1;
    }
    
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }
    
    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }
        
        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == STDIN_FILENO) {
                char buffer[1024];
                ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (bytes > 0) {
                    write(STDOUT_FILENO, buffer, bytes);
                }
            }
        }
    }
    
    close(epollfd);
    return 0;
}
```

### Оптимизация и лучшие практики:

#### 1. Эффективная обработка ошибок:
```c
ssize_t read_nonblock(int fd, void *buf, size_t count) {
    ssize_t total = 0;
    
    while (total < count) {
        ssize_t bytes = read(fd, buf + total, count - total);
        
        if (bytes > 0) {
            total += bytes;
        } else if (bytes == 0) {
            return total;  // EOF
        } else if (bytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return total;  // Временно нет данных
            }
            if (errno == EINTR) {
                continue;  // Прерывание сигналом
            }
            return -1;  // Реальная ошибка
        }
    }
    
    return total;
}
```

#### 2. Обработка таймаутов:
```c
#include <sys/time.h>

int wait_for_data(int fd, int timeout_ms) {
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    return select(fd + 1, &readfds, NULL, NULL, &tv);
}
```

### Типичные ошибки:

1. **Игнорирование частичного чтения/записи:**
```c
// Неправильно:
write(fd, data, size);

// Правильно:
ssize_t write_all(int fd, const void *data, size_t size) {
    const char *ptr = data;
    size_t remaining = size;
    
    while (remaining > 0) {
        ssize_t written = write(fd, ptr, remaining);
        if (written < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Ждем возможности записи
                continue;
            }
            return -1;
        }
        ptr += written;
        remaining -= written;
    }
    
    return size;
}
```

2. **Неправильное использование select:**
```c
// Неправильно:
select(fd + 1, &readfds, NULL, NULL, &tv);
select(fd + 1, &readfds, NULL, NULL, &tv); // tv уже изменен!

// Правильно:
struct timeval original_tv, tv;
original_tv.tv_sec = 1;
original_tv.tv_usec = 0;

while (1) {
    tv = original_tv;  // Копируем значение
    select(fd + 1, &readfds, NULL, NULL, &tv);
}
```

3. **Забытые файловые дескрипторы:**
```c
// Неправильно:
epoll_create(1);  // Утечка fd

// Правильно:
int epollfd = epoll_create1(0);
if (epollfd != -1) {
    // Использование epoll
    close(epollfd);  // Закрытие после использования
}
```

## 7. Жесткие и символьные ссылки

### Теоретические основы:

#### 1. Структура файловой системы:
- **Inode (индексный дескриптор)**
  ```c
  struct inode {
      mode_t  i_mode;     // Права доступа и тип файла
      uid_t   i_uid;      // User ID владельца
      gid_t   i_gid;      // Group ID
      nlink_t i_nlink;    // Количество жестких ссылок
      off_t   i_size;     // Размер в байтах
      time_t  i_atime;    // Время последнего доступа
      time_t  i_mtime;    // Время последнего изменения
      time_t  i_ctime;    // Время последнего изменения inode
      // ...
  };
  ```

#### 2. Типы ссылок:

1. **Жесткие ссылки (Hard Links)**
   - Прямая ссылка на inode
   - Одинаковые inode номера
   - Счетчик ссылок в inode
   - Только в пределах одной ФС
   - Нельзя на директории

2. **Символьные ссылки (Symbolic Links)**
   - Специальный тип файла
   - Содержит путь к цели
   - Свой inode
   - Между ФС
   - Можно на директории

### Практические примеры:

#### 1. Создание и управление ссылками:
```c
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

int main() {
    // Создание файла
    FILE *f = fopen("original.txt", "w");
    fprintf(f, "Hello, Links!");
    fclose(f);
    
    // Создание жесткой ссылки
    if (link("original.txt", "hard.txt") == -1) {
        perror("hard link failed");
        return 1;
    }
    
    // Создание символьной ссылки
    if (symlink("original.txt", "soft.txt") == -1) {
        perror("symbolic link failed");
        return 1;
    }
    
    // Получение информации о файлах
    struct stat st_orig, st_hard, st_soft;
    
    stat("original.txt", &st_orig);
    stat("hard.txt", &st_hard);
    lstat("soft.txt", &st_soft);
    
    printf("Original: inode=%lu, links=%lu\n", 
           st_orig.st_ino, st_orig.st_nlink);
    printf("Hard link: inode=%lu, links=%lu\n", 
           st_hard.st_ino, st_hard.st_nlink);
    printf("Soft link: inode=%lu, type=%u\n", 
           st_soft.st_ino, S_ISLNK(st_soft.st_mode));
    
    return 0;
}
```

#### 2. Работа с символьными ссылками:
```c
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

int main() {
    // Создание цепочки символьных ссылок
    symlink("target.txt", "link1");
    symlink("link1", "link2");
    symlink("link2", "link3");
    
    // Чтение символьной ссылки
    char buf[PATH_MAX];
    ssize_t len = readlink("link3", buf, sizeof(buf)-1);
    if (len != -1) {
        buf[len] = '\0';
        printf("link3 points to: %s\n", buf);
    }
    
    // Разрешение всей цепочки символьных ссылок
    char resolved[PATH_MAX];
    if (realpath("link3", resolved) != NULL) {
        printf("Resolved path: %s\n", resolved);
    }
    
    return 0;
}
```

#### 3. Рекурсивный обход с обработкой ссылок:
```c
#include <ftw.h>
#include <stdio.h>

int process_file(const char *fpath, const struct stat *sb,
                int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_SL) {
        // Символьная ссылка
        char link_target[PATH_MAX];
        ssize_t len = readlink(fpath, link_target, sizeof(link_target)-1);
        if (len != -1) {
            link_target[len] = '\0';
            printf("Symlink %s -> %s\n", fpath, link_target);
        }
    } else if (typeflag == FTW_F) {
        // Обычный файл
        printf("File %s, links: %lu\n", fpath, sb->st_nlink);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (nftw((argc < 2) ? "." : argv[1], process_file, 
             20, FTW_PHYS) == -1) {
        perror("nftw");
        return 1;
    }
    return 0;
}
```

### Оптимизация и лучшие практики:

#### 1. Безопасная работа с символьными ссылками:
```c
int safe_open_symlink(const char *path, int flags) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        return -1;
    }
    
    // Проверка на символьную ссылку
    if (S_ISLNK(st.st_mode)) {
        char real_path[PATH_MAX];
        if (realpath(path, real_path) == NULL) {
            return -1;
        }
        // Дополнительные проверки пути
        if (access(real_path, F_OK) == -1) {
            return -1;
        }
    }
    
    return open(path, flags);
}
```

#### 2. Атомарные операции со ссылками:
```c
int atomic_link_update(const char *link_path, const char *target) {
    char temp_path[PATH_MAX];
    snprintf(temp_path, sizeof(temp_path), 
             "%s.tmp.%d", link_path, getpid());
    
    // Создаем временную ссылку
    if (symlink(target, temp_path) == -1) {
        return -1;
    }
    
    // Атомарно заменяем старую ссылку
    if (rename(temp_path, link_path) == -1) {
        unlink(temp_path);
        return -1;
    }
    
    return 0;
}
```

### Типичные ошибки:

1. **Неправильная обработка символьных ссылок:**
```c
// Неправильно:
stat("symlink", &st);  // Следует по ссылке

// Правильно:
lstat("symlink", &st);  // Информация о самой ссылке
```

2. **Некорректная проверка существования файла:**
```c
// Неправильно (race condition):
if (access(path, F_OK) == 0) {
    fd = open(path, O_RDWR);
}

// Правильно:
fd = open(path, O_RDWR);
if (fd == -1) {
    // Обработка ошибки
}
```

3. **Игнорирование циклических ссылок:**
```c
// Неправильно:
while (is_symlink(path)) {
    read_symlink(path, new_path);
    path = new_path;
}

// Правильно:
int max_links = 20;  // Разумный лимит
while (is_symlink(path) && max_links-- > 0) {
    if (read_symlink(path, new_path) == -1) {
        break;
    }
    path = new_path;
}
if (max_links <= 0) {
    fprintf(stderr, "Too many symbolic links\n");
}
```

## 8. Конвенция о вызовах функций (Calling Convention)

### Теория:

Конвенция вызова функций определяет:
1. **Способ передачи параметров**
2. **Порядок сохранения и восстановления регистров**
3. **Очистку стека**
4. **Возврат результатов**

#### Основные типы конвенций:

1. **cdecl (C Declaration)**
   - Параметры передаются через стек справа налево
   - Вызывающая функция (caller) очищает стек
   - Результат в EAX/RAX
   - Подходит для функций с переменным числом аргументов
   ```assembly
   ; cdecl пример
   push    arg2
   push    arg1
   call    function
   add     esp, 8      ; caller очищает стек
   ```

2. **stdcall (Standard Call)**
   - Используется в Windows API
   - Параметры через стек справа налево
   - Вызываемая функция (callee) очищает стек
   ```assembly
   ; stdcall пример
   push    arg2
   push    arg1
   call    function    ; function сама очищает стек
   ```

3. **fastcall**
   - Первые параметры передаются через регистры
   - Остальные через стек
   - Быстрее cdecl из-за меньшего использования стека
   ```assembly
   ; fastcall пример
   mov     ecx, arg1   ; первый аргумент
   mov     edx, arg2   ; второй аргумент
   push    arg3        ; остальные через стек
   call    function
   ```

4. **System V AMD64 ABI** (Linux x64)
   - Параметры: RDI, RSI, RDX, RCX, R8, R9
   - Возврат значения в RAX
   - Вещественные числа через XMM0-XMM7
   ```assembly
   ; System V AMD64 пример
   mov     rdi, arg1
   mov     rsi, arg2
   call    function
   ```

### Практический пример:

```c
#include <stdio.h>

// cdecl функция
int __cdecl sum_cdecl(int a, int b, int c) {
    return a + b + c;
}

// stdcall функция
int __stdcall sum_stdcall(int a, int b) {
    return a + b;
}

// fastcall функция
int __fastcall sum_fastcall(int a, int b) {
    return a + b;
}

int main() {
    printf("cdecl: %d\n", sum_cdecl(1, 2, 3));
    printf("stdcall: %d\n", sum_stdcall(1, 2));
    printf("fastcall: %d\n", sum_fastcall(1, 2));
    return 0;
}
```

### Разберем на ассемблере:

```assembly
; cdecl вызов
section .text
global _sum_cdecl
_sum_cdecl:
    push    ebp
    mov     ebp, esp
    
    ; Доступ к параметрам через ebp
    mov     eax, [ebp + 8]     ; первый параметр
    add     eax, [ebp + 12]    ; второй параметр
    add     eax, [ebp + 16]    ; третий параметр
    
    mov     esp, ebp
    pop     ebp
    ret                        ; caller очищает стек

; fastcall вызов
global @sum_fastcall@8
@sum_fastcall@8:
    push    ebp
    mov     ebp, esp
    
    ; Параметры уже в ecx и edx
    mov     eax, ecx
    add     eax, edx
    
    mov     esp, ebp
    pop     ebp
    ret                        ; callee очищает стек
```

### Сохранение регистров:

1. **Caller-saved регистры** (вызывающая функция должна сохранить):
   - RAX, RCX, RDX
   - R8-R11
   - XMM0-XMM15

2. **Callee-saved регистры** (вызываемая функция должна сохранить):
   - RBX, RBP, RSP
   - R12-R15

### Пример с сохранением регистров:

```assembly
function:
    ; Пролог
    push    rbp
    mov     rbp, rsp
    push    rbx        ; Сохраняем callee-saved регистр
    
    ; Тело функции
    mov     rbx, rax   ; Используем rbx
    
    ; Эпилог
    pop     rbx        ; Восстанавливаем rbx
    mov     rsp, rbp
    pop     rbp
    ret
```

### ARM64 конвенция:

1. **Передача параметров:**
   - X0-X7: целочисленные/указатели
   - V0-V7: вещественные числа
   
2. **Возврат значений:**
   - X0: целочисленные результаты
   - V0: вещественные результаты

3. **Сохранение регистров:**
   - X9-X15: caller-saved
   - X19-X28: callee-saved

```assembly
// ARM64 пример
.global function
function:
    // Пролог
    stp     x29, x30, [sp, #-16]!   // Сохраняем frame pointer и link register
    mov     x29, sp
    
    // Параметры уже в x0-x7
    add     x0, x0, x1              // Сложение первых двух параметров
    
    // Эпилог
    ldp     x29, x30, [sp], #16     // Восстанавливаем регистры
    ret
```

Понимание этого соглашения о вызовах имеет решающее значение для:
- Написание эффективного низкоуровневого кода
- Отладка проблем со стеком
- Взаимодействие с кодом на разных языках
- Реверс-инжиниринг

## 9. Числа с плавающей точкой (IEEE754)

### Теоретические основы:

#### 1. Формат IEEE754:
```
Одинарная точность (32 бита):
[1 бит знака] [8 бит экспоненты] [23 бита мантиссы]

Двойная точность (64 бита):
[1 бит знака] [11 бит экспоненты] [52 бита мантиссы]
```

#### 2. Представление в памяти:
```c
// Одинарная точность (float)
union float_bits {
    float f;
    struct {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } bits;
};

// Двойная точность (double)
union double_bits {
    double d;
    struct {
        unsigned long mantissa : 52;
        unsigned long exponent : 11;
        unsigned long sign : 1;
    } bits;
};
```

#### 3. Специальные значения:
- Нормализованные числа (экспонента ≠ 0)
- Денормализованные числа (экспонента = 0)
- Бесконечность (экспонента = все 1, мантисса = 0)
- NaN (экспонента = все 1, мантисса ≠ 0)
- Нули (экспонента = 0, мантисса = 0)

### Практические примеры:

#### 1. Анализ чисел:
```c
#include <stdio.h>
#include <math.h>

void analyze_float(float num) {
    union float_bits fb;
    fb.f = num;
    
    printf("Значение: %f\n", num);
    printf("Знак: %d\n", fb.bits.sign);
    printf("Экспонента: %d\n", fb.bits.exponent);
    printf("Мантисса: %d\n", fb.bits.mantissa);
    
    if (fb.bits.exponent == 0 && fb.bits.mantissa == 0) {
        printf("Тип: Ноль\n");
    } else if (fb.bits.exponent == 0xFF) {
        if (fb.bits.mantissa == 0) {
            printf("Тип: Бесконечность\n");
        } else {
            printf("Тип: NaN\n");
        }
    } else if (fb.bits.exponent == 0) {
        printf("Тип: Денормализованное число\n");
    } else {
        printf("Тип: Нормализованное число\n");
    }
    printf("\n");
}
```

#### 2. Безопасное сравнение:
```c
#include <math.h>
#include <float.h>

// Сравнение с абсолютной погрешностью
int float_equals(float a, float b) {
    return fabs(a - b) <= FLT_EPSILON;
}

// Сравнение с относительной погрешностью
int float_equals_relative(float a, float b, float epsilon) {
    float diff = fabs(a - b);
    float abs_a = fabs(a);
    float abs_b = fabs(b);
    float larger = (abs_b > abs_a) ? abs_b : abs_a;
    
    if (larger == 0.0f) {
        return diff < epsilon;
    }
    return diff / larger < epsilon;
}
```

#### 3. Работа с особыми случаями:
```c
#include <math.h>

void special_cases_demo() {
    // Создание специальных значений
    float pos_inf = INFINITY;
    float neg_inf = -INFINITY;
    float nan_value = NAN;
    
    // Проверка специальных значений
    printf("Is inf: %d\n", isinf(pos_inf));
    printf("Is nan: %d\n", isnan(nan_value));
    
    // Операции со специальными значениями
    float result1 = pos_inf + 1.0f;  // Всё ещё бесконечность
    float result2 = pos_inf - pos_inf;  // NaN
    float result3 = 0.0f * pos_inf;  // NaN
}
```

### Оптимизация и лучшие практики:

#### 1. Избегание накопления ошибок:
```c
// Плохо - накопление ошибок
float sum = 0.0f;
for (int i = 0; i < 1000; i++) {
    sum += 0.1f;
}

// Лучше - работа с целыми числами
int cents = 0;
for (int i = 0; i < 1000; i++) {
    cents += 10;
}
float sum = cents / 100.0f;
```

#### 2. Защита от деления на ноль:
```c
float safe_divide(float numerator, float denominator) {
    if (fabs(denominator) < FLT_EPSILON) {
        return INFINITY * (numerator >= 0 ? 1 : -1);
    }
    return numerator / denominator;
}
```

### Типичные ошибки:

1. **Прямое сравнение:**
```c
// Неправильно:
if (x == 0.1f) { ... }

// Правильно:
if (float_equals(x, 0.1f)) { ... }
```

2. **Неучет потери точности:**
```c
// Неправильно:
float x = 0.1f + 0.2f;
float y = 0.3f;
if (x == y) { ... }

// Правильно:
if (float_equals_relative(x, y, 1e-5f)) { ... }
```

3. **Игнорирование особых случаев:**
```c
// Неправильно:
float result = sqrt(x);

// Правильно:
float result;
if (x < 0.0f) {
    result = NAN;
} else {
    result = sqrt(x);
}
```

---

###### Материалы курса: [github.com](https://github.com/victor-yacovlev/fpmi-caos/blob/master/2021-2022-full-year-course.md).
###### Презентация: [docs.google.com](https://docs.google.com/presentation/d/1Yas8AB8spk3-HJ0HzRK8VMA3Pv1PLii7Ks1YU6K_4hc/edit?usp=sharing).