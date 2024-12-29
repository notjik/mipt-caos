#include <sys/syscall.h>

long syscall(long number, ...);

void _start() {
    const char hello[] = "Hello, World!\n";
    syscall(SYS_write, 1, hello, sizeof(hello) - 1); 
    syscall(SYS_exit_group, 0);
}
