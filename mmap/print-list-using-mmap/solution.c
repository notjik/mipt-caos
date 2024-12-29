#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    size_t filesize = st.st_size;
    if (filesize < sizeof(struct Item)) {
        close(fd);
        return 0;
    }

    void *mapped = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    struct Item *items = (struct Item *)mapped;
    uint32_t offset = 0;

    do {
        if (offset >= filesize) {
            fprintf(stderr, "Invalid offset: %u\n", offset);
            munmap(mapped, filesize);
            return 1;
        }

        struct Item *item = (struct Item *)((char *)mapped + offset);
        printf("%d ", item->value);
        offset = item->next_pointer;
    } while (offset != 0);

    munmap(mapped, filesize);
    return 0;
}
