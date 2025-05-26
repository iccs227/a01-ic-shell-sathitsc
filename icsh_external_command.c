/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
 * For valid command exit_code = 0, else = other error numbers
 * Anything other than -1 (like 0, 1, or 127), loop stops .·°՞(っ-ᯅ-ς)՞°·.
 *
 * 4) Fix (in `icsh.c`):
 *    - Changed the loop to: `while (1)`
 *    - This allows the shell to keep running until explicitly types `exit`.
 */

int run_external_command(char *args[], int *exit_code) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        *exit_code = 1; //error
        return -1;
    }
    if (pid == 0) { // Replaces the child process with the external command using execvp()
        execvp(args[0], args);
        // https://www.digitalocean.com/community/tutorials/execvp-function-c-plus-plus
        perror("icsh");
        exit(127); // if execvp fails
        //  "command not found" exit code // https://linuxconfig.org/how-to-fix-bash-127-error-return-code
    }
    // Parent process
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        *exit_code = 1; //// waitpid failed
        return -1;
    }
    if (WIFEXITED(status)) {
        *exit_code = WEXITSTATUS(status);
    } else {
        *exit_code = 1;
    }
    return 0;
}
