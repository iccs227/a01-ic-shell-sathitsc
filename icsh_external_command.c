/* ICCS227: Project 1: icsh
 * Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "icsh_jobs.h"

extern pid_t fg_pid;

#define MAX_ARGS 100

// Safely close file descriptors if open
static void close_file_descriptor(int input_fd, int output_fd) {
    if (input_fd != -1) close(input_fd);
    if (output_fd != -1) close(output_fd);
}

// Check if a string is a redirection operator
static bool is_redirection_op(const char *arg) {
    return (strcmp(arg, "<") == 0 || strcmp(arg, ">") == 0 || strcmp(arg, ">>") == 0);
}

static int handle_redirection(char *args[], char *exec_args[],
                              int *input_fd, int *output_fd, int *exit_code) {
    int exec_index = 0;
    *input_fd = -1;
    *output_fd = -1;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] != NULL && !is_redirection_op(args[i + 1])) {
                *input_fd = open(args[i + 1], O_RDONLY);
                if (*input_fd < 0) {
                    perror("Error opening input file");
                    *exit_code = 1;
                    return -1;
                }
                i++; // Skip filename after '<'
            } else {
                exec_args[exec_index++] = args[i];
            }

        } else if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (args[i + 1] != NULL && !is_redirection_op(args[i + 1])) {
                int flags = O_WRONLY | O_CREAT;
                flags |= (strcmp(args[i], ">>") == 0) ? O_APPEND : O_TRUNC;

                *output_fd = open(args[i + 1], flags, 0644);
                if (*output_fd < 0) {
                    perror("(ෆ˙ᵕ˙ෆ)♡ Cannot open output file");
                    *exit_code = 1;
                    return -1;
                }
                i++; // Skip filename after '>' or '>>'
            } else {
                exec_args[exec_index++] = args[i];
            }

        } else {
            exec_args[exec_index++] = args[i];
        }
    }

    exec_args[exec_index] = NULL; // Null-terminate exec_args
    return 0;
}


int run_external_command(char *args[], int *exit_code) {
    int input_fd = -1, output_fd = -1;
    char *exec_args[MAX_ARGS] = {0};

    // Parse I/O redirections
    if (handle_redirection(args, exec_args, &input_fd, &output_fd, exit_code) != 0) {
        close_file_descriptor(input_fd, output_fd);
        return -1;
    }

    bool is_background = false;
    int argc = 0;
    while (exec_args[argc] != NULL) argc++;
    if (argc > 0 && strcmp(exec_args[argc - 1], "&") == 0) {
        is_background = true;
        exec_args[argc - 1] = NULL; // Remove '&' from args
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        *exit_code = 1;
        close_file_descriptor(input_fd, output_fd);
        return -1;
    }

    if (pid == 0) {
        setpgid(0, 0); // New process group

        if (input_fd != -1) dup2(input_fd, STDIN_FILENO);
        if (output_fd != -1) dup2(output_fd, STDOUT_FILENO);
        close_file_descriptor(input_fd, output_fd);

        execvp(exec_args[0], exec_args);
        fprintf(stderr, ".·°՞(っ-ᯅ-ς)՞°·.: bad command\n");
        exit(127);
    }

    // Parent process
    if (is_background) {
        // Build command string for jobs list
        char full_cmd[256] = {0};
        for (int i = 0; exec_args[i] != NULL; i++) {
            strcat(full_cmd, exec_args[i]);
            if (exec_args[i + 1] != NULL) strcat(full_cmd, " ");
        }

        add_job(pid, full_cmd, Running, true);
        printf("[%d] %d\n", get_next_job_id() - 1, pid);
        fg_pid = -1; // No foreground job
        close_file_descriptor(input_fd, output_fd);
        *exit_code = 0;
        return 0;
    }
    // Foreground
    fg_pid = pid;
    int status;
    int wait_result = waitpid(pid, &status, WUNTRACED);
    if (wait_result == -1) {
        if (errno == ECHILD) {
            fg_pid = -1;
            *exit_code = 127;
            close_file_descriptor(input_fd, output_fd);
            return 0;
        }
        perror("waitpid failed");
        fg_pid = -1;
        *exit_code = 1;
        close_file_descriptor(input_fd, output_fd);
        return -1;
    }

    fg_pid = -1;
    close_file_descriptor(input_fd, output_fd);

    if (WIFSTOPPED(status)) {
        update_job_status(pid, Stopped);
        printf("\n[Process Stopped ( ˶ˆᗜˆ˵ ) ⭑.ᐟ] %s\n", exec_args[0]);
        *exit_code = 0;
    } else if (WIFEXITED(status)) {
        remove_job(pid);
        *exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        remove_job(pid);
        *exit_code = WTERMSIG(status);
    } else {
        *exit_code = 1;
    }

    return 0;
}
