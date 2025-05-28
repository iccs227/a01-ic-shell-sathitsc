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

/**
 * 1) This function handles execution of external commands.
 *    - `args[]` is tokenized.
 *    - Uses a pointer `exit_code` to store the exit status of the command.
 *
 * 2) Forks a new process so that the shell doesn't replace itself by calling `execvp()` directly.
 *    - The child runs the external command using `execvp`.
 *
 * 3) Problem I faced:
 *    - In earlier versions (Milestone 1 & 2), the shell's main loop used:
 *          while (exit_code == -1)
 *    - But when an external command runs, `exit_code` is updated. --> loop ends (I don't want that)
 * For valid command exit_code = 0, else = other error numbers (1)
 * Anything other than -1 (like 0, 1, or 127), loop stops .·°՞(っ-ᯅ-ς)՞°·.
 *
 * 4) Fix (in `icsh.c`):
 *    - Changed the loop to: `while (1)`
 *    - This allows the shell to keep running until explicitly types `exit`.
 *
 * 5) Milestone 5: I/O Redirection
 *    - Based from I/O Redirection Resources on A1
 *    - Handle appending
 *    - add function to handle closing the open file descriptors for input/output redirection, if valid.
 *    - add handle_redirection
 *
 */

void close_file_descriptor(int input_fd, int output_fd) { //make sure to safely close the input and output files if they are open.
    if (input_fd != -1) close(input_fd);
    if (output_fd != -1) close(output_fd);
}

int handle_redirection(char *args[], char *exec_argument[],
                       int *input_file_descriptor, int *output_file_descriptor, int *exit_code) {
    int j = 0;
    *input_file_descriptor = -1; // Stores file descriptor
    *output_file_descriptor = -1; // Stores file descriptor

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
                // Not a valid redirection, treat "<" as normal built in from Milestone 1
                exec_argument[j++] = args[i];
            }

        // Handle output redirection
        } else if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (args[i + 1] != NULL &&
                strcmp(args[i + 1], "<") != 0 &&
                strcmp(args[i + 1], ">") != 0 &&
                strcmp(args[i + 1], ">>") != 0) {

                int flags = O_WRONLY | O_CREAT;
                if (strcmp(args[i], ">>") == 0)
                    flags |= O_APPEND;
                else
                    flags |= O_TRUNC;

                *output_file_descriptor = open(args[i + 1], flags, 0644);
                if (*output_file_descriptor < 0) {
                    perror("(ෆ˙ᵕ˙ෆ)♡ Cannot open output file");
                    *exit_code = 1;
                    return -1;
                }
                i++; // Skip filename
            } else {
                // Not a valid redirection, treat ">" or ">>" as built-in from Milestone1
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

    //Parse and open redirection files
    if (handle_redirection(args, exec_argument, &input_file_descriptor, &output_file_descriptor, exit_code) != 0) {
        close_file_descriptor(input_file_descriptor, output_file_descriptor);
        return -1;
    }

    //Fork a new process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        *exit_code = 1;
        close_file_descriptor(input_file_descriptor, output_file_descriptor);
        return -1;
    }
    if (pid == 0) { // Child process
        setpgid(0, 0); // Create new process group

        if (input_file_descriptor != -1) {
            dup2(input_file_descriptor, STDIN_FILENO);
        }
        if (output_file_descriptor != -1) {
            dup2(output_file_descriptor, STDOUT_FILENO); //Output will go in the file
        }
        close_file_descriptor(input_file_descriptor, output_file_descriptor);

        execvp(exec_argument[0], exec_argument);
        fprintf(stderr, ".·°՞(っ-ᯅ-ς)՞°·.: bad command\n");
        exit(127); // if execvp fails
        //  "command not found" exit code
        // https://linuxconfig.org/how-to-fix-bash-127-error-return-code
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
    // Clear foreground process
    //https://stackoverflow.com/questions/22865730/wexitstatuschildstatus-returns-0-but-waitpid-returns-1
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
