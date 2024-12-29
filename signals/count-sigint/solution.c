#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int signum) {
    sigint_count++;
}

void handle_sigterm(int signum) {
    printf("%d\n", sigint_count);
    fflush(stdout); 
    _exit(0);       
}

int main() {
    pid_t pid = getpid();
    printf("%d\n", pid);
    fflush(stdout); 

    struct sigaction sa_int;
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0; 
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("Error setting SIGINT handler");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_term;
    sa_term.sa_handler = handle_sigterm;
    sa_term.sa_flags = 0; 
    sigemptyset(&sa_term.sa_mask);
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("Error setting SIGTERM handler");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause(); 
    }

    return 0;
}