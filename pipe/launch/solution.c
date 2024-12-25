#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s CMD IN\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *cmd = argv[1];
    char *infile = argv[2];

    int input_fd = open(infile, O_RDONLY);
    if (input_fd < 0) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Create a pipe to capture the command's stdout
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        close(input_fd);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        close(input_fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child process

        // Redirect stdin to the input file
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            perror("dup2 failed for stdin");
            exit(EXIT_FAILURE);
        }
        close(input_fd);

        // Redirect stdout to the write end of the pipe
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 failed for stdout");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // Execute the command using /bin/sh -c to handle command strings
        execl("/bin/sh", "sh", "-c", cmd, (char *) NULL);

        // If execl returns, an error occurred
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process

        // Close the write end of the pipe as it's not needed
        close(pipe_fd[1]);
        close(input_fd);

        // Read from the read end of the pipe and count bytes
        ssize_t bytes;
        size_t total_bytes = 0;
        char buffer[4096];

        while ((bytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            total_bytes += bytes;
        }

        if (bytes < 0) {
            perror("Error reading from pipe");
            close(pipe_fd[0]);
            return EXIT_FAILURE;
        }

        close(pipe_fd[0]);

        // Wait for the child process to finish
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }

        // Print the total number of bytes written to stdout by the command
        printf("%zu\n", total_bytes);
    }

    return EXIT_SUCCESS;
}