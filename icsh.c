/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */


/**
source :
https://medium.com/@santiagobedoa/coding-a-shell-using-c-1ea939f10e7e
https://medium.com/@WinnieNgina/guide-to-code-a-simple-shell-in-c-bd4a3a4c41cd
https://www.shecodes.io/athena/68870-how-to-write-a-unix-command-line-interpreter-in-c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "icsh_builtins.h"

#define MAX_CMD_BUFFER 255
#define MAX_ARGS 64


int main() {
    char buffer[MAX_CMD_BUFFER];
    char prev_command[MAX_CMD_BUFFER] = "";
    char *args[MAX_ARGS];
    int exit_code = -1; // -1 = continue running

    printf(" ⊱  ۫ ׅ ✧ ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── Starting IC shell ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── ⊱  ۫ ׅ ✧ \n"
"  / ᐢ⑅ᐢ \\   ♡   ₊˚  \n"
"꒰˶• ༝ •˶꒱   ♡‧₊˚    ♡\n"
"./づ~ :¨·.·¨:     ₊˚  \n"
"           `·..·‘    ₊˚   ♡\n");

    while (exit_code == -1) {
        printf("icsh $ ");
        fflush(stdout);

        if (!fgets(buffer, MAX_CMD_BUFFER, stdin)) {
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

        if (handle_builtin(args, &exit_code)) {
            continue;
        }
        printf("bad command\n");
    }
    return exit_code >= 0 ? exit_code : 0;
}
