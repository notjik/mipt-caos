#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void find_substring(const char *filename, const char *search_str) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    // Получаем размер файла
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }

    size_t file_size = st.st_size;

    if (file_size == 0) {
        close(fd);
        return;
    }

    char *file_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        perror("Error mapping file to memory");
        close(fd);
        return;
    }

    size_t search_len = strlen(search_str);
    for (size_t i = 0; i <= file_size - search_len; i++) {
        if (memcmp(file_map + i, search_str, search_len) == 0) {
            printf("%zu ", i);
        }
    }

    munmap(file_map, file_size);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <search_string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    find_substring(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
