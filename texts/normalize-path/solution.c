#include <stdio.h>

void normalize_path(char *path) {
    char *src = path, *dst = path;

    while (*src) {
        if (*src == '/') {
            *dst++ = '/';
            while (*src == '/') {
                src++;
            }
        } else if (*src == '.' && (src[1] == '/' || src[1] == '\0')) {
            src += (src[1] == '/' ? 2 : 1);
        } else if (*src == '.' && src[1] == '.' && (src[2] == '/' || src[2] == '\0')) {
            src += (src[2] == '/' ? 3 : 2);
            if (dst > path + 1) {
                dst--;
                while (dst > path && *(dst - 1) != '/') {
                    dst--;
                }
            }
        } else {
            *dst++ = *src++;
        }
    }

    if (dst > path + 1 && *(dst - 1) == '/') {
        dst--;
    }

    *dst = '\0';
}
