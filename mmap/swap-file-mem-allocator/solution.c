#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#define MAX_BLOCKS 1024

typedef struct {
    off_t offset;
    size_t size;
    int free;
} Block;

static Block blocks[MAX_BLOCKS];
static size_t block_count = 0;
static int swap_file_fd = -1;
static void* swap_file_base = NULL;
static size_t swap_file_size = 0;

void myalloc_initialize(int fd) {
    swap_file_fd = fd;

    swap_file_size = lseek(fd, 0, SEEK_END);

    swap_file_base = mmap(NULL, swap_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, swap_file_fd, 0);
    if (swap_file_base == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    block_count = 1;
    blocks[0].offset = 0;
    blocks[0].size = swap_file_size;
    blocks[0].free = 1;
}

void myalloc_finalize() {
    if (swap_file_base) {
        munmap(swap_file_base, swap_file_size);
        swap_file_base = NULL;
    }

    if (swap_file_fd != -1) {
        close(swap_file_fd);
        swap_file_fd = -1;
    }

    memset(blocks, 0, sizeof(blocks));
    block_count = 0;
    swap_file_size = 0;
}

void* my_malloc(size_t size) {
    for (size_t i = 0; i < block_count; ++i) {
        if (blocks[i].free && blocks[i].size >= size) {
            if (blocks[i].size > size) {
                if (block_count >= MAX_BLOCKS) {
                    fprintf(stderr, "Too many blocks\n");
                    return NULL;
                }
                for (size_t j = block_count; j > i; --j) {
                    blocks[j] = blocks[j - 1];
                }
                blocks[i + 1].offset = blocks[i].offset + size;
                blocks[i + 1].size = blocks[i].size - size;
                blocks[i + 1].free = 1;
                block_count++;
            }
            blocks[i].size = size;
            blocks[i].free = 0;

            return (char*)swap_file_base + blocks[i].offset;
        }
    }
    return NULL;
}

void my_free(void* ptr) {
    if (!ptr) {
        return;
    }
    off_t offset = (char*)ptr - (char*)swap_file_base;

    for (size_t i = 0; i < block_count; ++i) {
        if (blocks[i].offset == offset && !blocks[i].free) {
            blocks[i].free = 1;

            if (i > 0 && blocks[i - 1].free) {
                blocks[i - 1].size += blocks[i].size;
                for (size_t j = i; j < block_count - 1; ++j) {
                    blocks[j] = blocks[j + 1];
                }
                block_count--;
                i--;
            }
            if (i < block_count - 1 && blocks[i + 1].free) {
                blocks[i].size += blocks[i + 1].size;
                for (size_t j = i + 1; j < block_count - 1; ++j) {
                    blocks[j] = blocks[j + 1];
                }
                block_count--;
            }
            return;
        }
    }
    fprintf(stderr, "Invalid free request: %p\n", ptr);
}
