/* ICCS227: Project 1: icsh
 * Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

extern pid_t fg_pid;

#define MAX_ARGS 100

// Safely close file descriptors if open
void close_file_descriptor(int input_fd, int output_fd) {
    if (input_fd != -1) close(input_fd);
    if (output_fd != -1) close(output_fd);
}

// Parse I/O redirection operators: < > >>
int handle_redirection(char *args[], char *exec_argument[],
                       int *input_file_descriptor, int *output_file_descriptor, int *exit_code) {
    int j = 0;
    *input_file_descriptor = -1;
    *output_file_descriptor = -1;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] != NULL &&
                strcmp(args[i + 1], "<") != 0 &&
                strcmp(args[i + 1], ">") != 0 &&
                strcmp(args[i + 1], ">>") != 0) {

                *input_file_descriptor = open(args[i + 1], O_RDONLY);
                if (*input_file_descriptor < 0) {
                    perror("Error: Invalid input ( ꩜ ᯅ ꩜;) Cannot read file");
                    *exit_code = 1;
                    return -1;
                }
                i++; // Skip filename
            } else {
                exec_argument[j++] = args[i]; // Not a valid redirection, treat "<" as normal built in from Milestone 1
            }

        } else if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (args[i + 1] != NULL &&
                strcmp(args[i + 1], "<") != 0 &&
                strcmp(args[i + 1], ">") != 0 &&
                strcmp(args[i + 1], ">>") != 0) {

                int flags = O_WRONLY | O_CREAT;
                flags |= (strcmp(args[i], ">>") == 0) ? O_APPEND : O_TRUNC;

                *output_file_descriptor = open(args[i + 1], flags, 0644);
                if (*output_file_descriptor < 0) {
                    perror("(ෆ˙ᵕ˙ෆ)♡ Cannot open output file");
                    *exit_code = 1;
                    return -1;
                }
                i++; // Skip filename
            } else {
                exec_argument[j++] = args[i];
            }

        } else {
            exec_argument[j++] = args[i];
        }
    }

    exec_argument[j] = NULL;
    return 0;
}


int run_external_command(char *args[], int *exit_code) {
    int input_file_descriptor = -1, output_file_descriptor = -1;
    char *exec_argument[100] = {0};

    // Parse for redirection
    if (handle_redirection(args, exec_argument, &input_file_descriptor, &output_file_descriptor, exit_code) != 0) {
        close_file_descriptor(input_file_descriptor, output_file_descriptor);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        *exit_code = 1;
        close_file_descriptor(input_file_descriptor, output_file_descriptor);
        return -1;
    }

    if (pid == 0) {
        setpgid(0, 0); // New process group

        if (input_file_descriptor != -1) {
            dup2(input_file_descriptor, STDIN_FILENO);
        }
        if (output_file_descriptor != -1) {
            dup2(output_file_descriptor, STDOUT_FILENO);
        }
        close_file_descriptor(input_file_descriptor, output_file_descriptor);

        execvp(exec_argument[0], exec_argument);
        fprintf(stderr, ".·°՞(っ-ᯅ-ς)՞°·.: bad command\n");
        exit(127);
    }

    // Parent process
    fg_pid = pid;
    int status;

    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("icsh: waitpid failed");
        fg_pid = -1;
        *exit_code = 1;
        close_file_descriptor(input_file_descriptor, output_file_descriptor);
        return -1;
    }

    fg_pid = -1;
    close_file_descriptor(input_file_descriptor, output_file_descriptor);

    if (WIFSTOPPED(status)) {
        *exit_code = 0;
        printf("\n・┆✦ Process Stopped ʚ♡ɞ ✦ ┆・ %s\n", exec_argument[0]);
    } else if (WIFEXITED(status)) {
        *exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        *exit_code = WTERMSIG(status);
    } else {
        *exit_code = 1;
    }

    return 0;
}
