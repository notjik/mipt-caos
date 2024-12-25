#include <stdio.h>
#include <stdlib.h>

#define MAX_WORDS 255
#define MAX_WORD_LENGTH 4096

int main() {
    int word_count = 0;
    char word[MAX_WORD_LENGTH];

    while (word_count < MAX_WORDS && scanf("%4095s", word) == 1) {
        word_count++;
    }

    printf("%d\n", word_count);

    return 0;
}