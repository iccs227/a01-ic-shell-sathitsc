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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "icsh_builtins.h"

#define MAX_CMD_BUFFER 255
#define MAX_ARGS 64

int main(int argc, char *argv[]) { // main is changed to handle both interactive and script mode
    char buffer[MAX_CMD_BUFFER];
    char prev_command[MAX_CMD_BUFFER] = "";
    char *args[MAX_ARGS];
    int exit_code = -1;

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

    while (exit_code == -1) {
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

        if (strlen(buffer) == 0) {
            continue;
        }

        bool repeated = handle_repeat_command(buffer, prev_command);

        if (repeated && strlen(buffer) == 0) {
            // Milestone 1.2) !! with no previous command: do nothing, just next prompt
            continue;
        }

        if (!repeated) {
            strcpy(prev_command, buffer);
        }

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
            printf("%d\n", exit_code == -1 ? 0 : exit_code); //(exit_code == -1 means no previous exit code staus stored
        }

        if (handle_builtin(args, &exit_code)) { // From Milestone 1
            continue;
        }

        printf("bad command\n");
    }

    // If input == stdin, no need to close it unless it reads from a file (script mode activated)

    if (input != stdin) {
        fclose(input);
    }

    return exit_code >= 0 ? exit_code : 0;
}
