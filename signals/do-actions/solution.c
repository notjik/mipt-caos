#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;

void handle_sigusr1(int signum) {
    counter += 1; 
    printf("%d\n", counter);
    fflush(stdout); 
}


void handle_sigusr2(int signum) {
    counter *= -1; 
    printf("%d\n", counter);
    fflush(stdout); 
}

void handle_termination(int signum) {
    exit(0); 
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <initial_value>\n", argv[0]);
        return 1;
    }

    counter = atoi(argv[1]);

    printf("%d\n", getpid());
    fflush(stdout); 

    struct sigaction sa_usr1;
    struct sigaction sa_usr2;
    struct sigaction sa_term;

    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Error setting SIGUSR1 handler");
        exit(1);
    }

    sa_usr2.sa_handler = handle_sigusr2;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1) {
        perror("Error setting SIGUSR2 handler");
        exit(1);
    }

    sa_term.sa_handler = handle_termination;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("Error setting SIGTERM handler");
        exit(1);
    }
    if (sigaction(SIGINT, &sa_term, NULL) == -1) {
        perror("Error setting SIGINT handler");
        exit(1);
    }

    while (1) {
        pause(); 
    }

    return 0;
}