## Milestone Implementation Summary

Milestones 1 & 2: Basic Shell Loop and Script Mode


Originally, the shell used while (exit_code == -1) to control the main loop. However, any valid or failed command would update exit_code, prematurely exiting the shell.


Fix: Replaced with while (1) to run indefinitely until the user explicitly types exit.

Added argc checks to support both interactive and script mode:

argc == 1: Interactive mode using stdin

argc > 1: Script mode; input is read from the file passed as an argument


Echo Command

Implemented echo $? to display the last command’s exit code, mimicking basic shell behavior.


echo was initially treated as a built-in, but caused issues with I/O redirection (>/>>).


Fix: Treated echo as an external command when used with redirection.


Milestone 5: I/O Redirection


Added support for <, >, and >>.


Implemented handle_redirection() and helper functions to manage file descriptors safely. 

Ensures redirection operators are followed by a valid filename.


Milestone 6: Background Jobs and Job Control

Introduced background job support using &, jobs, fg, bg.

Set setpgid(0, 0) in child processes to manage process groups.

Handled signals (SIGCHLD, SIGINT, SIGTSTP) with sigaction() for robust job control.

waitpid() used with WUNTRACED to detect if a process stops (e.g., via Ctrl+Z).

Job list management included storing command strings and job statuses.


Signals and Resilience
Used sigaction() with SA_RESTART to automatically retry interrupted system calls (e.g., read(), wait()).
Ensured shell remains interactive and handles unexpected signals.

source

https://medium.com/@santiagobedoa/coding-a-shell-using-c-1ea939f10e7e  

https://medium.com/@WinnieNgina/guide-to-code-a-simple-shell-in-c-bd4a3a4c41cd  

https://www.shecodes.io/athena/68870-how-to-write-a-unix-command-line-interpreter-in-c 

https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf  

https://shapeshed.com/unix-exit-codes/  

https://stackoverflow.com/questions/2485028/signal-handling-in-c  

https://stackoverflow.com/questions/22865730/wexitstatuschildstatus-returns-0-but-waitpid-returns-1  

https://www.ibm.com/docs/en/zos/3.1.0?topic=functions-wait-wait-child-process-end  

https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/wait.html  

https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-sigemptyset-initialize-signal-mask-exclude-all-signals  

https://www.gnu.org/software/libc/manual/html_node/Foreground-and-Background.html  

