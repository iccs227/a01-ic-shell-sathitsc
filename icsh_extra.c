/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
* StudentID: 6481366
*/

// icsh_extra.c For milestone 7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX_COMMANDS 10
#define MAX_ARGS 64

char *trim_whitespace(char *str) {
    if (str == NULL) return NULL;

    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0')  // Empty string after trimming leading space
        return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return str;
}

// Parse a command string
void parse_command(char *command, char **argv) {
    int argc = 0;
    char *token = strtok(command, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = token;
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
}

// Helper: create pipes for all but the last command
int create_pipes(int pipefds[], int num_pipes) { //num_pipes = Number of Pipes Needed
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipefds + i*2) < 0) {
            perror("pipe");
            return -1;
        }
    }
    return 0;
}

// close all pipe file descriptors
void close_pipes(int pipefds[], int count) {
    for (int i = 0; i < count; i++) {
        close(pipefds[i]);
    }
}

// wait for all child processes
void wait_for_children(int num_children) {
    for (int i = 0; i < num_children; i++) {
        wait(NULL);
    }
}

// Execute multiple piped commands
void execute_piped_commands(char *input_line) {
    char *commands[MAX_COMMANDS];
    int num_commands = 0;

    // Split input_line by '|', trim whitespace for each command
    char *token = strtok(input_line, "|");
    while (token != NULL && num_commands < MAX_COMMANDS) {
        commands[num_commands++] = trim_whitespace(token);
        token = strtok(NULL, "|");
    }

    if (num_commands == 0) return;  // Nothing to execute

    int num_pipes = num_commands - 1;
    int pipefds[2 * num_pipes];

    if (num_pipes > 0 && create_pipes(pipefds, num_pipes) == -1) {
        return;
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            if (num_pipes > 0) close_pipes(pipefds, 2 * num_pipes);
            return;
        }

        if (pid == 0) { // Child process
            // If not first command, redirect stdin to previous pipe read end
            if (i != 0) {
                if (dup2(pipefds[(i - 1)*2], STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
            }

            // If not last command, redirect stdout to current pipe write end
            if (i != num_commands - 1) {
                if (dup2(pipefds[i*2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe fds in child after redirecting
            if (num_pipes > 0) close_pipes(pipefds, 2 * num_pipes);

            // Parse command into argv and execute
            char *argv[MAX_ARGS];
            parse_command(commands[i], argv);
            execvp(argv[0], argv);

            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent closes all pipe file descriptors
    if (num_pipes > 0) close_pipes(pipefds, 2 * num_pipes);

    // Parent waits for all children to finish
    wait_for_children(num_commands);
}

void print_help() {
    const char *help_msg =
        "₊✩‧₊˚౨ৎ˚₊✩‧₊⸜(｡˃ ᵕ ˂ )⸝♡  welcome to icsh! ₊✩‧₊˚౨ৎ˚₊✩‧₊\n"
        "✨ here are some magical commands you can use:\n"
        "  echo <text>           ✦ say something out loud (prints your message)\n"
        "  !!                    ✦ repeat the last thing you said (previous output)\n"
        "  exit                  ✦ gracefully leave the shell (bye-bye!)\n"
        "  fg %%<job_id>          ✦ bring a background task into the spotlight\n"
        "  jobs                  ✦ show all the cool stuff running in the background\n"
        "  bg %%<job_id>          ✦ send a paused task back into action in the background\n"
        "  help                  ✦ need a hand? here's this guide again ☆\n";
    printf("%s", help_msg);
}
