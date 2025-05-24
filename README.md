[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/WIXYXthJ)
# ICSH


## Milestone 1: Interactive command-line interpreter

- Shows a prompt (icsh $)
- Reads user commands : !!(repeat previous command), echo and exit
- Prints "bad command" if input is not valid

Firstly, I declare `int exit_code = -1;`to control when to exit the shell and -1 means keep running.
  Once the shell starts it displays prompt (icsh $)
 
  
  To handle the requirements in Milestone 1:
    
I seperated into icsh.c icsh_builtins.c
icsh.c ➜ the main shell loop, input reading, and command processing.
icsh_builtins.c ➜ logic for built-in commands; exit, echo, and !!.

# 1. handle_repeat_command(char *buffer, const char *prev_command)
   
If !! is entered and there is no previous command stored, it clears the input buffer so nothing runs, and give a new prompt.
If there is a previous command, it replaces the current input buffer with that previous command to execute it again.

# 2. handle_builtin(char **args, int *exit_code)
   
This function checks if the parsed command matches any built-in commands and handles them internally without creating a new process.

Milestone 1 Built-ins include:
echo: Prints all the arguments given after the echo command
exit: Exits the shell (>= 0 = "Exit the shell with this code")

When a built-in command is executed, the function returns true indicating the command was handled; otherwise, it returns false to indicate it’s not a built-in and show "bad command".

The shell keeps running while exit_code == -1.
When exit_code is changed by a built-in command "exit", the loop ends and the program exits.
