/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */
#ifndef ICSH_BUILTINS_H
#define ICSH_BUILTINS_H

#include <stdbool.h>

bool handle_builtin(char **args, int *exit_code);

bool handle_repeat_command(char *buffer, const char *prev_command);

#endif
