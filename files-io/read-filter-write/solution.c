#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        write(STDERR_FILENO, "Usage: ./program input_file output_digits_file output_other_file\n", 64);
        _exit(1);
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        _exit(1);
    }

    int output_digits_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_digits_fd == -1) {
        close(input_fd);
        _exit(2);
    }

    int output_other_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_other_fd == -1) {
        close(input_fd);
        close(output_digits_fd);
        _exit(2);
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(input_fd, buffer, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (isdigit((unsigned char) buffer[i])) {
                if (write(output_digits_fd, &buffer[i], 1) == -1) {
                    close(input_fd);
                    close(output_digits_fd);
                    close(output_other_fd);
                    _exit(3);
                }
            } else {
                if (write(output_other_fd, &buffer[i], 1) == -1) {
                    close(input_fd);
                    close(output_digits_fd);
                    close(output_other_fd);
                    _exit(3);
                }
            }
        }
    }

    if (bytes_read == -1) {
        close(input_fd);
        close(output_digits_fd);
        close(output_other_fd);
        _exit(3);
    }

    close(input_fd);
    close(output_digits_fd);
    close(output_other_fd);

    return 0;
}
