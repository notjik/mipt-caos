#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

#define BUFFER_SIZE (64 * 1024)

// Функция сравнения для qsort
int cmp(const void *a, const void *b) {
    int32_t int_a = *((int32_t*)a);
    int32_t int_b = *((int32_t*)b);

    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

int32_t le32toh_custom(int32_t val) {
    unsigned char *ptr = (unsigned char *)&val;
    return ((int32_t)ptr[0] << 0) | ((int32_t)ptr[1] << 8) | ((int32_t)ptr[2] << 16) | ((int32_t)ptr[3] << 24);
}

int32_t htole32_custom(int32_t val) {
    unsigned char *ptr = (unsigned char *)&val;
    return ((int32_t)ptr[0] << 0) | ((int32_t)ptr[1] << 8) | ((int32_t)ptr[2] << 16) | ((int32_t)ptr[3] << 24);
}

int32_t read_int_le(int fd) {
    int32_t val;
    ssize_t n = read(fd, &val, sizeof(val));
    if (n == 0) {
        return -1;
    }
    if (n != sizeof(val)) {
        if (n < 0) {
            perror("read");
        } else {
            fprintf(stderr, "Unexpected end of file\n");
        }
        exit(1);
    }
    return le32toh_custom(val);
}

void write_int_le(int fd, int32_t val) {
    val = htole32_custom(val);
    ssize_t n = write(fd, &val, sizeof(val));
    if (n != sizeof(val)) {
        perror("write");
        exit(1);
    }
}

void sort_and_write_block(int *fd_in, int *fd_out, size_t block_size) {
    int32_t *block = (int32_t *)malloc(block_size * sizeof(int32_t));
    if (!block) {
        perror("malloc");
        exit(1);
    }

    size_t i = 0;
    while (i < block_size) {
        int32_t val = read_int_le(*fd_in);
        if (val == -1) break;

        block[i++] = val;
    }

    qsort(block, i, sizeof(int32_t), cmp);

    for (size_t j = 0; j < i; j++) {
        write_int_le(*fd_out, block[j]);
    }

    free(block);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0) {
        perror("open input file");
        return 1;
    }

    int fd_out = open("temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        perror("open output file");
        close(fd_in);
        return 1;
    }

    size_t block_size = BUFFER_SIZE / sizeof(int32_t);
    sort_and_write_block(&fd_in, &fd_out, block_size);

    close(fd_out);

    fd_out = open(argv[1], O_WRONLY | O_TRUNC);
    if (fd_out < 0) {
        perror("open input file for writing");
        close(fd_in);
        return 1;
    }

    fd_in = open("temp", O_RDONLY);
    if (fd_in < 0) {
        perror("open temporary file");
        close(fd_out);
        return 1;
    }

    off_t offset;
    ssize_t n;
    char buffer[BUFFER_SIZE];
    while ((n = read(fd_in, buffer, sizeof(buffer))) > 0) {
        if (write(fd_out, buffer, n) != n) {
            perror("write to input file");
            close(fd_in);
            close(fd_out);
            return 1;
        }
    }

    if (n < 0) {
        perror("read from temporary file");
    }

    close(fd_in);
    close(fd_out);

    unlink("temp");

    return 0;
}
