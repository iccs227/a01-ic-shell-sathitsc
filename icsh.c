/* ICCS227: Project 1: icsh
 * Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

/**
source :
M1 :
https://medium.com/@santiagobedoa/coding-a-shell-using-c-1ea939f10e7e
https://medium.com/@WinnieNgina/guide-to-code-a-simple-shell-in-c-bd4a3a4c41cd
https://www.shecodes.io/athena/68870-how-to-write-a-unix-command-line-interpreter-in-c
M2 :
https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
https://shapeshed.com/unix-exit-codes/
M4 :
https://stackoverflow.com/questions/2485028/signal-handling-in-c
https://stackoverflow.com/questions/22865730/wexitstatuschildstatus-returns-0-but-waitpid-returns-1
https://www.ibm.com/docs/en/zos/3.1.0?topic=functions-wait-wait-child-process-end
https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/wait.html
https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-sigemptyset-initialize-signal-mask-exclude-all-signals
M6:
https://www.gnu.org/software/libc/manual/html_node/Foreground-and-Background.html (not starting yet)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "icsh_builtins.h"
#include "icsh_external_command.h"
#include "icsh_signal_handler.h"

#define MAX_CMD_BUFFER 255
#define MAX_ARGS 64

int main(int argc, char *argv[]) { // main is changed to handle both interactive and script mode
    char buffer[MAX_CMD_BUFFER];
    char original_buffer[MAX_CMD_BUFFER]; //Milestone 5 handler
    char prev_command[MAX_CMD_BUFFER] = "";
    char *args[MAX_ARGS];
    int exit_code = -1;
    implement_signal_handlers(); //Milestone 4

    // in Milestone 2, I add argc into main to count command-line arguments.
    // set default: interactive mode input, user can type anything ≽^• ˕ • ྀི≼
    FILE *input = stdin;
    //For Milestone argc == 1 : "no arguments" = only ./icsh is assigned → use stdin.
    // if it is larger than 1, will consider it as a script mode
    // use ./icsh testMileStone2.sh to test
    if (argc > 1) {
        // Script mode activated because a file is passed, the shell will read from a file, line by line.
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "icsh: cannot open script file %s\n", argv[1]);
            return 1;
        } // check for validation and permission
    } else {
        // Interactive mode ₍^. .^₎⟆
        // If no file is passed → input stay as stdin (standard mode)
        // Stay the same as Milestone 1
        printf(" ⊱  ۫ ׅ ✧ ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── Starting IC shell ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── ⊱  ۫ ׅ ✧ \n"
               "  / ᐢ⑅ᐢ \\   ♡   ₊˚  \n"
               "꒰˶• ༝ •˶꒱   ♡‧₊˚    ♡\n"
               "./づ~ :¨·.·¨:     ₊˚  \n"
               "           `·..·‘    ₊˚   ♡\n");
    }
    /**
     * In Milestone 1 and 2, the main shell loop was :
     *      while (exit_code == -1)
     * ──── ୨୧ ────
     * This worked on m1,2 because `exit_code` remained -1 unless explicitly set by a command.
     * Once any command sets `exit_code` to something other than -1 (like 0 for success) check icsh_external_command.c,
     * the condition `exit_code == -1` becomes false and it stops running.
     * As a result, the shell loop exits—even if I did not type `exit`.
     * I replaced the loop with `while (1)`, which creates an infinite loop.
     * This ensures that the shell keeps running and *only* exits when the user explicitly types `exit`.
     * ──── ୨୧ ────
     */
    while (1) { // Keep the shell running forever — until I manually break the loop.
        if (input == stdin) {
            printf("icsh $ ");
            fflush(stdout);
        }

        if (!fgets(buffer, MAX_CMD_BUFFER, input)) {
            if (input != stdin) break;
            printf("\n");
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "!!") == 0) {
            if (strlen(prev_command) == 0) {
                continue;
            }
            strncpy(buffer, prev_command, MAX_CMD_BUFFER);
            printf("%s\n", buffer); // show repeated command
        }

        // Milestone 5: External command is not repeated so I create a pre-tokenized command
        strncpy(original_buffer, buffer, MAX_CMD_BUFFER);
        strncpy(prev_command, original_buffer, MAX_CMD_BUFFER);

        // Tokenize input
        int i = 0;
        args[i] = strtok(buffer, " ");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            args[i] = strtok(NULL, " ");
        }
        args[i] = NULL;

        // check echo $?
        // 1) user must type 'echo' as the first word and followed with "$?" first
        // 2) If true, prints the last exit code.
        // 3) https://unix.stackexchange.com/questions/501128/what-does-echo-do
        // 4) If there is no previous command to set exit code , just set it = 0
        if (args[0] && strcmp(args[0], "echo") == 0 && args[1] && strcmp(args[1], "$?") == 0) {
            printf("%d\n", exit_code == -1 ? 0 : exit_code);
            //(exit_code == -1 means no previous exit code status stored
            continue;
        }

        // Check for built-in commands
        if (handle_builtin(args, &exit_code)) {
            // if user types 'exit', stop the shell
            if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
                break;
            }
            continue;
        }

        // run external command
        if (args[0] != NULL) {
            int ret = run_external_command(args, &exit_code);
            if (ret != 0) {
                printf("icsh: error running command\n");
            }
        } else {
            printf("bad command ૮(˶╥︿╥)ა \n");
        }
    }

    // If input == stdin, no need to close it unless it reads from a file (script mode activated)
    if (input != stdin) {
        fclose(input);
    }

    return exit_code >= 0 ? exit_code : 0;
}
