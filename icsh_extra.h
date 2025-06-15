/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

// icsh_extra.c For milestone 7

#ifndef ICSH_EXTRA_H
#define ICSH_EXTRA_H


void print_help();
void trim_whitespace(char *str);
char **parse_command(char *input, int *argc);
void create_pipes(int num_pipes, int pipefds[][2]);
void close_pipes(int num_pipes, int pipefds[][2]);
void wait_for_children(int num_pipes);
void execute_piped_commands(char *input_line);

#endif