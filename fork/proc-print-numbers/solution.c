#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void print_numbers(int current, int max) {
    if (current < max) {
        printf("%d ", current);
        fflush(stdout);
    } else {
        printf("%d\n", current);
        fflush(stdout);
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        print_numbers(current + 1, max);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);

    if (N <= 0) {
        exit(EXIT_FAILURE);
    }

    print_numbers(1, N);

    return 0;
}