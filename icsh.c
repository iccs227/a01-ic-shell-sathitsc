/* ICCS227: Project 1: icsh
 * Name: Chinanard Sathitseth
 * StudentID: 6481366
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

int main(int argc, char *argv[]) {
    char buffer[MAX_CMD_BUFFER];
    char original_buffer[MAX_CMD_BUFFER];  // Milestone 5: original buffer for repeated command
    char prev_command[MAX_CMD_BUFFER] = "";
    char *args[MAX_ARGS];
    int exit_code = -1;

    implement_signal_handlers();  // Milestone 4: set up SIGINT and SIGTSTP

    FILE *input = stdin;

    // Script mode if a filename is passed
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "icsh: cannot open script file %s\n", argv[1]);
            return 1;
        }
    } else {
        // Interactive mode
        printf(" ⊱  ۫ ׅ ✧ ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── Starting IC shell ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── ⊱  ۫ ׅ ✧ \n"
               "  / ᐢ⑅ᐢ \\   ♡   ₊˚  \n"
               "꒰˶• ༝ •˶꒱   ♡‧₊˚    ♡\n"
               "./づ~ :¨·.·¨:     ₊˚  \n"
               "           `·..·‘    ₊˚   ♡\n");
    }

    while (1) {

        if (input == stdin) {
            printf("icsh $ ");
            fflush(stdout);
        }

        // Read input
        if (!fgets(buffer, MAX_CMD_BUFFER, input)) {
            if (input != stdin) break;
            printf("\n");
            break;
        }

        //newline
        buffer[strcspn(buffer, "\n")] = '\0';

        // Handle repeat command (!!)
        if (strcmp(buffer, "!!") == 0) {
            if (strlen(prev_command) == 0) continue;
            strncpy(buffer, prev_command, MAX_CMD_BUFFER);
            printf("%s\n", buffer);  // Show repeated command
        }

        // Save command before tokenizing (Milestone 5 Handler)
        strncpy(original_buffer, buffer, MAX_CMD_BUFFER);
        strncpy(prev_command, original_buffer, MAX_CMD_BUFFER);

        // Tokenize input
        int i = 0;
        args[i] = strtok(buffer, " ");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            args[++i] = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Handle `echo $?`
        if (args[0] && strcmp(args[0], "echo") == 0 &&
            args[1] && strcmp(args[1], "$?") == 0) {
            printf("%d\n", exit_code == -1 ? 0 : exit_code);  // -1 means no previous command
            continue;
        }

        // Check and execute built-in commands
        if (handle_builtin(args, &exit_code)) {
            if (args[0] && strcmp(args[0], "exit") == 0) break;
            continue;
        }

        // Run external command
        if (args[0] != NULL) {
            int ret = run_external_command(args, &exit_code);
            if (ret != 0) {
                printf("bad command ૮(˶╥︿╥)ა \n");
            }
        }
    }

    // Close input file in script mode
    if (input != stdin) {
        fclose(input);
    }

    return exit_code >= 0 ? exit_code : 0;
}
