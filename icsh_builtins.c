/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icsh_builtins.h"

bool is_redirection(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if ((strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0)) {
            // Check if next token exists and is not another operator
            if (args[i + 1] != NULL && strcmp(args[i + 1], ">") != 0 &&
                strcmp(args[i + 1], ">>") != 0 && strcmp(args[i + 1], "<") != 0) {
                return true;  // Valid redirection only
                }
        }
    }
    return false;
}


bool handle_repeat_command(char *buffer, const char *prev_command) {
    if (strcmp(buffer, "!!") == 0) {
        if (strlen(prev_command) == 0) {
            buffer[0] = '\0';
            return true;
        }
        strcpy(buffer, prev_command);
        return true;
    }
    return false;
}

bool handle_builtin(char **args, int *exit_code) {
    if (args[0] == NULL) {
        return true;
    }

    // Built-in echo as usual from Milestone 1
    if (strcmp(args[0], "echo") == 0 && !is_redirection(args)) {
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s", args[i]);
            if (args[i + 1] != NULL) printf(" ");
        }
        printf("\n");
        return true;
    }

    if (strcmp(args[0], "exit") == 0) {
        int code = 0;
        if (args[1] != NULL) {
            code = atoi(args[1]) & 0xFF;
        }
        printf("⊱  ۫ ׅ ✧ ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── bye ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── ⊱  ۫ ׅ ✧ \n");
        *exit_code = code;
        return true;
    }

    return false;
}
