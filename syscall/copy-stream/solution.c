#include <sys/syscall.h>

long syscall(long number, ...);

void _start() {
    char buffer[1024];
    long bytes_read;
    
    while (1) {
        bytes_read = syscall(SYS_read, 0, buffer, sizeof(buffer));
        
        if (bytes_read <= 0) {
            break;
        }

        syscall(SYS_write, 1, buffer, bytes_read);
    }

    syscall(SYS_exit, 0);
}
