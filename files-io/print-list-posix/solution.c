#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

void print_list(int fd) {
    struct Item item;
    uint32_t offset = 0;

    while (1) {
        if (lseek(fd, offset, SEEK_SET) == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_read = read(fd, &item, sizeof(item));
        if (bytes_read != sizeof(item)) {
            if (bytes_read == 0) {
                break;
            } else {
                perror("read");
                exit(EXIT_FAILURE);
            }
        }

        printf("%d ", item.value);

        if (item.next_pointer == 0) {
            break;
        }

        offset = item.next_pointer;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    print_list(fd);

    close(fd);

    return 0;
}
